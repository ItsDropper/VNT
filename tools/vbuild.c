#define _POSIX_C_SOURCE 200809L
/*
 * Copyright (c) 2026 ItsDropper
 * SPDX-License-Identifier: MIT
 *
 * VBuild: native project build driver for VNT.
 */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#include <windows.h>
#include <io.h>
#define MKDIR(path) _mkdir(path)
#define EXE_SUFFIX ".exe"
#define WAIT_STATUS(rc) (rc)
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0777)
#define EXE_SUFFIX ""
#define WAIT_STATUS(rc) (WIFEXITED(rc) ? WEXITSTATUS(rc) : 1)
#endif

#define LINE_CAP 4096
#define PATH_CAP 2048
#define TASK_CAP 64
#define TASK_NAME_CAP 128
#define DEP_CAP 8

typedef struct {
    char name[256];
    char version[128];
    char source[PATH_CAP];
    char output[256];
    char compiler[PATH_CAP];
    char toolchain[PATH_CAP];
    char target[64];
} Config;

static char project_root[PATH_CAP];
static int force_build;
static char selected_target[64] = "exe";
static char repositories[16][PATH_CAP];
static int repository_count;
static char module_dependencies[64][PATH_CAP];
static int module_dependency_count;
static char resolved_dependencies[64][PATH_CAP];

typedef enum { ACT_BUILD, ACT_RUN, ACT_CHECK, ACT_CLEAN, ACT_TEST, ACT_NONE } Action;
typedef struct {
    char name[TASK_NAME_CAP];
    Action action;
    char deps[DEP_CAP][TASK_NAME_CAP];
    int dep_count;
    int state;
} Task;

static Task tasks[TASK_CAP];
static int task_count;

static void trim(char *s) {
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    size_t start = 0;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start) memmove(s, s + start, strlen(s + start) + 1);
}

static int safe_filename(const char *s) {
    return s[0] && !strchr(s, '/') && !strchr(s, '\\') &&
           strcmp(s, ".") && strcmp(s, "..");
}

static int safe_relative_path(const char *s) {
    const char *p = s;
    if (!s[0] || s[0] == '/' || s[0] == '\\' || strchr(s, ':')) return 0;
    while (*p) {
        const char *start = p;
        while (*p && *p != '/' && *p != '\\') p++;
        if ((p - start == 2 && start[0] == '.' && start[1] == '.') ||
            (p - start == 1 && start[0] == '.')) return 0;
        if (*p) p++;
    }
    return 1;
}

static int parse_string_call(const char *line, const char *function, char *out, size_t cap) {
    size_t fnlen = strlen(function);
    const char *p = line;
    while (isspace((unsigned char)*p)) p++;
    if (strncmp(p, function, fnlen) || p[fnlen] != '(') return 0;
    p += fnlen + 1;
    while (isspace((unsigned char)*p)) p++;
    if (*p++ != '"') return 0;
    size_t n = 0;
    while (*p && *p != '"') {
        if (*p == '\\' || n + 1 >= cap) return 0;
        out[n++] = *p++;
    }
    if (*p++ != '"') return 0;
    while (isspace((unsigned char)*p)) p++;
    if (*p++ != ')') return 0;
    while (isspace((unsigned char)*p)) p++;
    if (*p && *p != '#') return 0;
    out[n] = '\0';
    return n > 0;
}

static int copy_value(char *dst, size_t cap, const char *src, const char *key) {
    if (strlen(src) >= cap) {
        fprintf(stderr, "vbuild: property '%s' is too long.\n", key);
        return 0;
    }
    strcpy(dst, src);
    return 1;
}

static int load_properties(Config *cfg) {
    FILE *f = fopen("vbuild.properties", "r");
    char line[LINE_CAP];
    int got_name = 0, got_source = 0;
    if (!f) {
        fprintf(stderr, "vbuild: missing vbuild.properties; run 'vbuild init'.\n");
        return 0;
    }
    while (fgets(line, sizeof line, f)) {
        char *eq, *key, *value;
        if (!strchr(line, '\n') && !feof(f)) {
            fprintf(stderr, "vbuild: property line exceeds %d bytes.\n", LINE_CAP - 1);
            fclose(f); return 0;
        }
        trim(line);
        if (!line[0] || line[0] == '#' || line[0] == '!') continue;
        eq = strchr(line, '=');
        if (!eq) {
            fprintf(stderr, "vbuild: invalid properties line: %s\n", line);
            fclose(f); return 0;
        }
        *eq = '\0'; key = line; value = eq + 1;
        trim(key); trim(value);
        if (value[0] == '"' && strlen(value) >= 2 &&
            value[strlen(value) - 1] == '"') {
            value[strlen(value) - 1] = '\0'; value++;
        }
        if (!strcmp(key, "project.name")) {
            if (!copy_value(cfg->name, sizeof cfg->name, value, key)) { fclose(f); return 0; }
            got_name = 1;
        } else if (!strcmp(key, "project.version")) {
            if (!copy_value(cfg->version, sizeof cfg->version, value, key)) { fclose(f); return 0; }
        } else if (!strcmp(key, "source")) {
            if (!copy_value(cfg->source, sizeof cfg->source, value, key)) { fclose(f); return 0; }
            got_source = 1;
        } else if (!strcmp(key, "output")) {
            if (!copy_value(cfg->output, sizeof cfg->output, value, key)) { fclose(f); return 0; }
        } else if (!strcmp(key, "compiler")) {
            if (!copy_value(cfg->compiler, sizeof cfg->compiler, value, key)) { fclose(f); return 0; }
        } else if (!strcmp(key, "toolchain")) {
            if (!copy_value(cfg->toolchain, sizeof cfg->toolchain, value, key)) { fclose(f); return 0; }
        } else {
            fprintf(stderr, "vbuild: unknown property '%s'.\n", key);
            fclose(f); return 0;
        }
    }
    if (ferror(f)) { fprintf(stderr, "vbuild: error reading vbuild.properties.\n"); fclose(f); return 0; }
    fclose(f);
    if (!got_name || !cfg->name[0]) {
        fprintf(stderr, "vbuild: project.name is required.\n"); return 0;
    }
    if (!got_source || !cfg->source[0]) {
        fprintf(stderr, "vbuild: source is required.\n"); return 0;
    }
    if (!cfg->output[0] && !copy_value(cfg->output, sizeof cfg->output, cfg->name, "output")) return 0;
    if (!safe_filename(cfg->output)) {
        fprintf(stderr, "vbuild: output must be a file name inside build/.\n"); return 0;
    }
    strcpy(cfg->target, selected_target[0] ? selected_target : "exe");
    if (!cfg->version[0]) strcpy(cfg->version, "0.1.0");
    if (!cfg->compiler[0]) strcpy(cfg->compiler, "vnt");
    if (!cfg->toolchain[0]) {
        const char *home = getenv("VNT_HOME");
        if (!copy_value(cfg->toolchain, sizeof cfg->toolchain, home && home[0] ? home : ".", "toolchain")) return 0;
    }
    return 1;
}

static int path_is_absolute(const char *path) {
#ifdef _WIN32
    return path[0] == '/' || path[0] == '\\' ||
           (isalpha((unsigned char)path[0]) && path[1] == ':');
#else
    return path[0] == '/';
#endif
}

static int absolute_path(const char *path, char *out, size_t cap) {
    int n;
    if (path_is_absolute(path)) n = snprintf(out, cap, "%s", path);
    else n = snprintf(out, cap, "%s/%s", project_root, path);
    if (n < 0 || (size_t)n >= cap) {
        fprintf(stderr, "vbuild: path is too long: %s\n", path);
        return 0;
    }
    return 1;
}

static int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
#ifdef _WIN32
    return (st.st_mode & _S_IFDIR) != 0;
#else
    return S_ISDIR(st.st_mode);
#endif
}

static int exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static int configure_module_path(void) {
    char search_path[PATH_CAP * 4] = {0};
    size_t used = 0;
    int i, j;
    for (i = 0; i < repository_count; i++) {
        char root[PATH_CAP];
        int n;
        if (!absolute_path(repositories[i], root, sizeof root)) return 0;
        n = snprintf(search_path + used, sizeof search_path - used, "%s%s",
#ifdef _WIN32
                     used ? ";" : "",
#else
                     used ? ":" : "",
#endif
                     root);
        if (n < 0 || (size_t)n >= sizeof search_path - used) {
            fprintf(stderr, "vbuild: combined repository path is too long.\n");
            return 0;
        }
        used += (size_t)n;
    }
    for (i = 0; i < module_dependency_count; i++) {
        int found = 0;
        for (j = 0; j < repository_count; j++) {
            char root[PATH_CAP];
            char candidate[PATH_CAP];
            int n;
            if (!absolute_path(repositories[j], root, sizeof root)) return 0;
            n = snprintf(candidate, sizeof candidate, "%s/%s", root, module_dependencies[i]);
            if (n < 0 || (size_t)n >= sizeof candidate) {
                fprintf(stderr, "vbuild: dependency path is too long: %s\n", module_dependencies[i]);
                return 0;
            }
            if (exists(candidate) && !is_directory(candidate)) {
                if (!copy_value(resolved_dependencies[i], sizeof resolved_dependencies[i],
                                candidate, "dependency")) return 0;
                found = 1;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "vbuild: dependency '%s' was not found in any local repository.\n",
                    module_dependencies[i]);
            return 0;
        }
    }
#ifdef _WIN32
    if (_putenv_s("VNT_MODULE_PATH", search_path) != 0) {
#else
    if (setenv("VNT_MODULE_PATH", search_path, 1) != 0) {
#endif
        fprintf(stderr, "vbuild: could not configure VNT_MODULE_PATH.\n");
        return 0;
    }
    return 1;
}


static int ensure_build_dir(void) {
    struct stat st;
    if (stat("build", &st) == 0) {
#ifdef _WIN32
        if (st.st_mode & _S_IFDIR) return 1;
#else
        if (S_ISDIR(st.st_mode)) return 1;
#endif
        fprintf(stderr, "vbuild: build exists but is not a directory.\n");
        return 0;
    }
    if (MKDIR("build") == 0 || errno == EEXIST) return 1;
    fprintf(stderr, "vbuild: cannot create build/: %s\n", strerror(errno));
    return 0;
}

static int write_file(const char *path, const char *contents) {
    FILE *f = fopen(path, "wx");
    int ok;
    if (!f) {
        fprintf(stderr, "vbuild: refusing to overwrite %s (%s).\n", path, strerror(errno));
        return 0;
    }
    ok = fputs(contents, f) >= 0;
    if (fclose(f) != 0) ok = 0;
    if (!ok) fprintf(stderr, "vbuild: failed writing %s.\n", path);
    return ok;
}

static int init_project(void) {
    const char *name = "hello";
    const char *properties =
        "project.name = hello\nproject.version = 0.1.0\nsource = src/main.vnt\noutput = hello\ncompiler = vnt\ntoolchain = .\n";
    const char *script =
        "# VBuild task file (version 1)\n"
        "# Select: exe, elf, asm, or freestanding-asm\n"
        "target exe\n"
        "repositories {\n"
        "    local(\"libs\")\n"
        "}\n"
        "dependencies {\n"
        "    # implementation(\"math_helpers.vnt\")\n"
        "}\n"
        "task build = build\n"
        "task run = run depends build\n"
        "task check = check\n"
        "task clean = clean\n"
        "task test = test\n";
    if (exists("vbuild.properties") || exists("build.vbuild")) {
        fprintf(stderr, "vbuild: build.vbuild or vbuild.properties already exists; refusing to overwrite.\n");
        return 1;
    }
    if (!write_file("vbuild.properties", properties)) return 1;
    if (!write_file("build.vbuild", script)) { remove("vbuild.properties"); return 1; }
    if (!exists("src") && MKDIR("src") != 0) {
        fprintf(stderr, "vbuild: cannot create src/: %s\n", strerror(errno));
        remove("build.vbuild"); remove("vbuild.properties"); return 1;
    }
    if (!write_file("src/main.vnt", "print(\"Hello from VNT\")\n")) {
        remove("build.vbuild"); remove("vbuild.properties"); return 1;
    }
    printf("Initialized native VBuild project '%s'.\n", name);
    return 0;
}

static int parse_action(const char *s, Action *action) {
    if (!strcmp(s, "build")) *action = ACT_BUILD;
    else if (!strcmp(s, "run")) *action = ACT_RUN;
    else if (!strcmp(s, "check")) *action = ACT_CHECK;
    else if (!strcmp(s, "clean")) *action = ACT_CLEAN;
    else if (!strcmp(s, "test")) *action = ACT_TEST;
    else if (!strcmp(s, "none")) *action = ACT_NONE;
    else return 0;
    return 1;
}

static const char *action_label(Action action) {
    switch (action) {
        case ACT_BUILD: return "build";
        case ACT_RUN: return "run";
        case ACT_CHECK: return "check";
        case ACT_CLEAN: return "clean";
        case ACT_TEST: return "test";
        case ACT_NONE: return "none";
    }
    return "invalid";
}

static int find_task(const char *name) {
    int i;
    for (i = 0; i < task_count; i++) if (!strcmp(tasks[i].name, name)) return i;
    return -1;
}

/* Grammar: task NAME = ACTION [depends NAME[, NAME...]] */
static int load_tasks(void) {
    FILE *f = fopen("build.vbuild", "r");
    char line[LINE_CAP];
    int lineno = 0, section = 0, target_seen = 0;
    if (!f) { fprintf(stderr, "vbuild: missing build.vbuild.\n"); return 0; }
    task_count = 0;
    repository_count = 0;
    module_dependency_count = 0;
    while (fgets(line, sizeof line, f)) {
        char *p, *name, *action_name, *depends;
        Task *task;
        lineno++;
        if (!strchr(line, '\n') && !feof(f)) {
            fprintf(stderr, "vbuild: build.vbuild:%d: line too long.\n", lineno);
            fclose(f); return 0;
        }
        trim(line); p = line;
        if (!p[0] || p[0] == '#') continue;
        if (!strcmp(p, "}")) {
            if (!section) { fprintf(stderr, "vbuild: build.vbuild:%d: unexpected '}'.\n", lineno); fclose(f); return 0; }
            section = 0; continue;
        }
        if (!strcmp(p, "repositories {")) {
            if (section) { fprintf(stderr, "vbuild: build.vbuild:%d: nested blocks are unsupported.\n", lineno); fclose(f); return 0; }
            section = 1; continue;
        }
        if (!strcmp(p, "dependencies {")) {
            if (section) { fprintf(stderr, "vbuild: build.vbuild:%d: nested blocks are unsupported.\n", lineno); fclose(f); return 0; }
            section = 2; continue;
        }
        if (section) {
            char value[PATH_CAP];
            if (section == 1) {
                if (!parse_string_call(p, "local", value, sizeof value) || !safe_relative_path(value) || repository_count >= 16) {
                    fprintf(stderr, "vbuild: build.vbuild:%d: expected local(\"relative-directory\") in repositories block.\n", lineno);
                    fclose(f); return 0;
                }
                strcpy(repositories[repository_count++], value);
            } else {
                if (!parse_string_call(p, "implementation", value, sizeof value) || !safe_relative_path(value) || module_dependency_count >= 64) {
                    fprintf(stderr, "vbuild: build.vbuild:%d: expected implementation(\"relative-module.vnt\") in dependencies block.\n", lineno);
                    fclose(f); return 0;
                }
                strcpy(module_dependencies[module_dependency_count++], value);
            }
            continue;
        }
        if (!strncmp(p, "target ", 7)) {
            char *target = p + 7;
            if (target_seen) {
                fprintf(stderr, "vbuild: build.vbuild:%d: target may only be declared once.\\n", lineno);
                fclose(f); return 0;
            }
            trim(target);
            if (strcmp(target, "exe") && strcmp(target, "elf") &&
                strcmp(target, "asm") && strcmp(target, "freestanding-asm")) {
                fprintf(stderr, "vbuild: build.vbuild:%d: target must be exe, elf, asm, or freestanding-asm.\n", lineno);
                fclose(f); return 0;
            }
            if (strlen(target) >= sizeof selected_target) { fclose(f); return 0; }
            strcpy(selected_target, target);
            target_seen = 1;
            continue;
        }
        if (strncmp(p, "task ", 5) != 0) {
            fprintf(stderr, "vbuild: build.vbuild:%d: expected 'task'.\n", lineno);
            fclose(f); return 0;
        }
        name = p + 5;
        while (*name && !isspace((unsigned char)*name) && *name != '=') name++;
        if (!*name) { fprintf(stderr, "vbuild: build.vbuild:%d: missing action.\n", lineno); fclose(f); return 0; }
        if ((size_t)(name - (p + 5)) >= TASK_NAME_CAP) {
            fprintf(stderr, "vbuild: build.vbuild:%d: task name too long.\n", lineno); fclose(f); return 0;
        }
        {
            char saved = *name;
            *name = '\0';
            if (find_task(p + 5) >= 0) {
                fprintf(stderr, "vbuild: build.vbuild:%d: duplicate task '%s'.\n", lineno, p + 5);
                fclose(f); return 0;
            }
            if (task_count >= TASK_CAP) { fprintf(stderr, "vbuild: task limit (%d) exceeded.\n", TASK_CAP); fclose(f); return 0; }
            task = &tasks[task_count++];
            memset(task, 0, sizeof *task);
            strcpy(task->name, p + 5);
            *name = saved;
        }
        while (*name && isspace((unsigned char)*name)) name++;
        if (*name != '=') { fprintf(stderr, "vbuild: build.vbuild:%d: expected '=' after task name.\n", lineno); fclose(f); return 0; }
        action_name = name + 1;
        while (*action_name && isspace((unsigned char)*action_name)) action_name++;
        depends = strstr(action_name, " depends ");
        if (depends) { *depends = '\0'; depends += 9; trim(action_name); trim(depends); }
        if (!parse_action(action_name, &task->action)) {
            fprintf(stderr, "vbuild: build.vbuild:%d: unknown action '%s'.\n", lineno, action_name);
            fclose(f); return 0;
        }
        if (depends && depends[0]) {
            char *token = depends;
            while (token) {
                char *comma = strchr(token, ',');
                if (comma) *comma = '\0';
                trim(token);
                if (!token[0] || task->dep_count >= DEP_CAP || strlen(token) >= TASK_NAME_CAP) {
                    fprintf(stderr, "vbuild: build.vbuild:%d: invalid/too many dependencies.\n", lineno);
                    fclose(f); return 0;
                }
                strcpy(task->deps[task->dep_count++], token);
                token = comma ? comma + 1 : NULL;
            }
        }
    }
    if (section) { fprintf(stderr, "vbuild: build.vbuild: unterminated block.\n"); fclose(f); return 0; }
    if (ferror(f)) { fprintf(stderr, "vbuild: error reading build.vbuild.\n"); fclose(f); return 0; }
    fclose(f);
    if (!task_count) { fprintf(stderr, "vbuild: build.vbuild defines no tasks.\n"); return 0; }
    {
        int i, j;
        for (i = 0; i < task_count; i++)
            for (j = 0; j < tasks[i].dep_count; j++)
                if (find_task(tasks[i].deps[j]) < 0) {
                    fprintf(stderr, "vbuild: task '%s' depends on undefined task '%s'.\n", tasks[i].name, tasks[i].deps[j]);
                    return 0;
                }
    }
    return 1;
}

static int run_process(char *const argv[], const char *working_directory) {
#ifdef _WIN32
    char old_directory[PATH_CAP];
    int rc;
    if (!_getcwd(old_directory, sizeof old_directory)) {
        fprintf(stderr, "vbuild: cannot read current directory: %s\n", strerror(errno)); return 127;
    }
    if (working_directory && _chdir(working_directory) != 0) {
        fprintf(stderr, "vbuild: cannot enter working directory '%s': %s\n", working_directory, strerror(errno)); return 127;
    }
    rc = _spawnvp(_P_WAIT, argv[0], (const char * const *)argv);
    if (_chdir(old_directory) != 0) {
        fprintf(stderr, "vbuild: failed to restore working directory '%s'.\n", old_directory);
        return 127;
    }
    if (rc == -1) { fprintf(stderr, "vbuild: cannot launch '%s': %s\n", argv[0], strerror(errno)); return 127; }
    return rc;
#else
    pid_t pid = fork();
    int status;
    if (pid < 0) { fprintf(stderr, "vbuild: fork failed: %s\n", strerror(errno)); return 127; }
    if (pid == 0) {
        if (working_directory && chdir(working_directory) != 0) {
            fprintf(stderr, "vbuild: cannot enter working directory '%s': %s\n", working_directory, strerror(errno));
            _exit(127);
        }
        execvp(argv[0], argv);
        fprintf(stderr, "vbuild: cannot launch '%s': %s\n", argv[0], strerror(errno));
        _exit(127);
    }
    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR) continue;
        fprintf(stderr, "vbuild: waitpid failed: %s\n", strerror(errno)); return 127;
    }
    return WAIT_STATUS(status);
#endif
}

#define IMPORT_CAP 256
static char imported_files[IMPORT_CAP][PATH_CAP];
static int imported_file_count;

static int remember_import(const char *path) {
    int i;
    for (i = 0; i < imported_file_count; i++)
        if (!strcmp(imported_files[i], path)) return 2;
    if (imported_file_count >= IMPORT_CAP) {
        fprintf(stderr, "vbuild: import graph exceeds %d source files.\n", IMPORT_CAP);
        return 0;
    }
    if (!copy_value(imported_files[imported_file_count], PATH_CAP, path, "import path")) return 0;
    imported_file_count++;
    return 1;
}

static int resolve_import_for_build(const char *importer, const char *module,
                                    char *resolved, size_t resolved_cap) {
    char candidate[PATH_CAP];
    const char *slash = strrchr(importer, '/');
    const char *backslash = strrchr(importer, '\\');
    const char *separator = slash;
    size_t dir_len;
    int n, i;
    if (backslash && (!separator || backslash > separator)) separator = backslash;
    dir_len = separator ? (size_t)(separator - importer) : 0;
    n = separator ? snprintf(candidate, sizeof candidate, "%.*s/%s",
                              (int)dir_len, importer, module)
                  : snprintf(candidate, sizeof candidate, "%s", module);
    if (n >= 0 && (size_t)n < sizeof candidate && exists(candidate) && !is_directory(candidate))
        return copy_value(resolved, resolved_cap, candidate, "resolved import");
    for (i = 0; i < repository_count; i++) {
        char root[PATH_CAP];
        if (!absolute_path(repositories[i], root, sizeof root)) return 0;
        n = snprintf(candidate, sizeof candidate, "%s/%s", root, module);
        if (n < 0 || (size_t)n >= sizeof candidate) continue;
        if (exists(candidate) && !is_directory(candidate))
            return copy_value(resolved, resolved_cap, candidate, "resolved import");
    }
    fprintf(stderr, "vbuild: imported module '%s' (from '%s') was not found.\n", module, importer);
    return 0;
}

static int collect_import_graph(const char *path) {
    FILE *file;
    char line[LINE_CAP];
    {
        int remembered = remember_import(path);
        if (remembered == 0) return 0;
        /* Already visited: stop cycles and repeated imports. */
        if (remembered == 2) return 1;
    }
    file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "vbuild: cannot read imported source '%s': %s\n", path, strerror(errno));
        return 0;
    }
    while (fgets(line, sizeof line, file)) {
        char module[PATH_CAP], resolved[PATH_CAP];
        char *p = line;
        size_t len = 0;
        while (isspace((unsigned char)*p)) p++;
        if (strncmp(p, "import", 6) || !isspace((unsigned char)p[6])) continue;
        p += 6;
        while (isspace((unsigned char)*p)) p++;
        if (*p++ != '"') continue;
        while (*p && *p != '"' && len + 1 < sizeof module) {
            if (*p == '\\' && p[1]) p++;
            module[len++] = *p++;
        }
        if (*p != '"' || len == 0 || len + 1 >= sizeof module) continue;
        module[len] = '\0';
        if (!resolve_import_for_build(path, module, resolved, sizeof resolved) ||
            !collect_import_graph(resolved)) {
            fclose(file);
            return 0;
        }
    }
    if (ferror(file)) {
        fprintf(stderr, "vbuild: error reading source '%s'.\n", path);
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static int imports_are_current(const char *source, const char *output) {
    struct stat output_stat;
    int i;
    imported_file_count = 0;
    if (!collect_import_graph(source)) return -1;
    if (stat(output, &output_stat) != 0) return 0;
    for (i = 0; i < imported_file_count; i++) {
        struct stat input_stat;
        if (stat(imported_files[i], &input_stat) != 0 ||
            output_stat.st_mtime < input_stat.st_mtime) return 0;
    }
    return 1;
}

/* Inputs consumed after compilation by VNT's hosted runtime or freestanding linker. */
static int build_inputs_are_current(const char *output, const char *target, const char *toolchain) {
    static const char *const hosted_inputs[] = {
        "src/compiler/native_runtime.c",
        "src/compiler/native_runtime.h",
        "src/compiler/native_net.c",
        "src/compiler/native_async_net.c"
    };
    static const char *const elf_inputs[] = {
        "freestanding/x86_64.ld"
    };
    const char *const *inputs = NULL;
    size_t count = 0, i;
    struct stat output_stat;
    if (!strcmp(target, "exe")) {
        inputs = hosted_inputs;
        count = sizeof hosted_inputs / sizeof hosted_inputs[0];
    } else if (!strcmp(target, "elf")) {
        inputs = elf_inputs;
        count = sizeof elf_inputs / sizeof elf_inputs[0];
    } else {
        return 1;
    }
    if (stat(output, &output_stat) != 0) return 0;
    for (i = 0; i < count; i++) {
        char input_path[PATH_CAP];
        struct stat input_stat;
        int n = snprintf(input_path, sizeof input_path, "%s/%s", toolchain, inputs[i]);
        if (n < 0 || (size_t)n >= sizeof input_path) {
            fprintf(stderr, "vbuild: build input path is too long: %s\\n", inputs[i]);
            return -1;
        }
        if (stat(input_path, &input_stat) != 0) {
            fprintf(stderr, "vbuild: required build input is missing: %s\n", input_path);
            return -1;
        }
        if (output_stat.st_mtime < input_stat.st_mtime) return 0;
    }
    return 1;
}


/* FNV-1a content fingerprint: portable across the native Windows/Linux builds. */
static unsigned long long fingerprint_hash = 14695981039346656037ULL;

static void fingerprint_bytes(const unsigned char *data, size_t size) {
    size_t i;
    for (i = 0; i < size; i++) {
        fingerprint_hash ^= data[i];
        fingerprint_hash *= 1099511628211ULL;
    }
}

static int fingerprint_file(const char *path) {
    FILE *file = fopen(path, "rb");
    unsigned char buffer[8192];
    size_t count;
    if (!file) return 0;
    fingerprint_bytes((const unsigned char *)path, strlen(path));
    fingerprint_bytes((const unsigned char *)"\0", 1);
    while ((count = fread(buffer, 1, sizeof buffer, file)) > 0)
        fingerprint_bytes(buffer, count);
    if (ferror(file)) {
        fprintf(stderr, "vbuild: cannot fingerprint input '%s'.\n", path);
        fclose(file);
        return 0;
    }
    fclose(file);
    fingerprint_bytes((const unsigned char *)"\xff", 1);
    return 1;
}

static int compute_fingerprint(const char *source, const char *compiler,
                               const char *toolchain, const char *target,
                               char *result, size_t result_cap) {
    static const char *const hosted_inputs[] = {
        "src/compiler/native_runtime.c",
        "src/compiler/native_runtime.h",
        "src/compiler/native_net.c",
        "src/compiler/native_async_net.c"
    };
    static const char *const elf_inputs[] = { "freestanding/x86_64.ld" };
    const char *const *target_inputs = NULL;
    size_t target_count = 0, i;
    int input_index;
    char path[PATH_CAP];
    int n;
    fingerprint_hash = 14695981039346656037ULL;
    if (!fingerprint_file(source) ||
        !fingerprint_file("vbuild.properties") ||
        !fingerprint_file("build.vbuild")) return 0;
    if (exists(compiler)) {
        if (!fingerprint_file(compiler)) return 0;
    } else {
        /* A compiler resolved only through PATH cannot be fingerprinted portably. */
        fingerprint_bytes((const unsigned char *)compiler, strlen(compiler));
    }
    for (input_index = 0; input_index < imported_file_count; input_index++)
        if (!fingerprint_file(imported_files[input_index])) return 0;
    for (input_index = 0; input_index < module_dependency_count; input_index++)
        if (!fingerprint_file(resolved_dependencies[input_index])) return 0;
    if (!strcmp(target, "exe")) {
        target_inputs = hosted_inputs;
        target_count = sizeof hosted_inputs / sizeof hosted_inputs[0];
    } else if (!strcmp(target, "elf")) {
        target_inputs = elf_inputs;
        target_count = sizeof elf_inputs / sizeof elf_inputs[0];
    }
    for (i = 0; i < target_count; i++) {
        n = snprintf(path, sizeof path, "%s/%s", toolchain, target_inputs[i]);
        if (n < 0 || (size_t)n >= sizeof path) {
            fprintf(stderr, "vbuild: build input path is too long.\n");
            return 0;
        }
        if (!fingerprint_file(path)) {
            fprintf(stderr, "vbuild: required build input is missing or unreadable: %s\n", path);
            return 0;
        }
    }
    n = snprintf(result, result_cap, "%016llx", fingerprint_hash);
    if (n < 0 || (size_t)n >= result_cap) return 0;
    return 1;
}

static int manifest_matches(const char *output, const char *fingerprint) {
    char path[PATH_CAP], saved[96], expected[96];
    FILE *file;
    int n = snprintf(path, sizeof path, "%s.vbuild-inputs", output);
    if (n < 0 || (size_t)n >= sizeof path) return 0;
    file = fopen(path, "r");
    if (!file) return 0;
    if (!fgets(saved, sizeof saved, file)) {
        fclose(file);
        return 0;
    }
    fclose(file);
    trim(saved);
    n = snprintf(expected, sizeof expected, "vbuild-inputs-v1 %s", fingerprint);
    return n >= 0 && (size_t)n < sizeof expected && !strcmp(saved, expected);
}

static int write_manifest(const char *output, const char *fingerprint) {
    char path[PATH_CAP];
    FILE *file;
    int n = snprintf(path, sizeof path, "%s.vbuild-inputs", output);
    if (n < 0 || (size_t)n >= sizeof path) {
        fprintf(stderr, "vbuild: build manifest path is too long.\n");
        return 0;
    }
    file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "vbuild: cannot write build manifest '%s': %s\n", path, strerror(errno));
        return 0;
    }
    if (fprintf(file, "vbuild-inputs-v1 %s\n", fingerprint) < 0) {
        fclose(file);
        fprintf(stderr, "vbuild: failed writing build manifest '%s'.\n", path);
        return 0;
    }
    if (fclose(file) != 0) {
        fprintf(stderr, "vbuild: failed closing build manifest '%s'.\n", path);
        return 0;
    }
    return 1;
}

static int run_compiler(Config *cfg, int check);
static int run_tests(Config *cfg);

static int run_compiler(Config *cfg, int check) {
    char output[PATH_CAP];
    char source[PATH_CAP];
    char compiler[PATH_CAP];
    char toolchain[PATH_CAP];
    char *args[6];
    const char *mode = NULL;
    const char *suffix = NULL;
    struct stat output_stat;
    char fingerprint[64];
    int fingerprint_ready;
    int import_graph_current;
    int build_inputs_current;
    if (!configure_module_path()) return 1;
    if (!exists(cfg->source)) {
        fprintf(stderr, "vbuild: source file not found: %s\n", cfg->source); return 1;
    }
    if (!check && !ensure_build_dir()) return 1;
    if (!absolute_path(cfg->source, source, sizeof source) ||
        !absolute_path(cfg->toolchain, toolchain, sizeof toolchain) ||
        !is_directory(toolchain)) {
        fprintf(stderr, "vbuild: toolchain directory not found or path invalid: %s\n", cfg->toolchain); return 1;
    }
    if (strchr(cfg->compiler, '/') || strchr(cfg->compiler, '\\')) {
        if (!absolute_path(cfg->compiler, compiler, sizeof compiler)) return 1;
    } else {
        char candidate[PATH_CAP];
        int n = snprintf(candidate, sizeof candidate, "%s/%s", toolchain, cfg->compiler);
        if (n >= 0 && (size_t)n < sizeof candidate && exists(candidate)) {
            if (!copy_value(compiler, sizeof compiler, candidate, "compiler")) return 1;
        } else {
#ifdef _WIN32
            n = snprintf(candidate, sizeof candidate, "%s/%s.exe", toolchain, cfg->compiler);
            if (n >= 0 && (size_t)n < sizeof candidate && exists(candidate)) {
                if (!copy_value(compiler, sizeof compiler, candidate, "compiler")) return 1;
            } else
#endif
            if (!copy_value(compiler, sizeof compiler, cfg->compiler, "compiler")) return 1;
        }
    }
    if (!strcmp(cfg->target, "exe")) { mode = "--compile"; suffix = EXE_SUFFIX; }
    else if (!strcmp(cfg->target, "elf")) { mode = "--build-freestanding"; suffix = ".elf"; }
    else if (!strcmp(cfg->target, "asm")) { mode = "--emit-asm"; suffix = ".s"; }
    else if (!strcmp(cfg->target, "freestanding-asm")) { mode = "--emit-freestanding-asm"; suffix = ".s"; }
    else { fprintf(stderr, "vbuild: unsupported target '%s'.\n", cfg->target); return 1; }
    if (!absolute_path("build", output, sizeof output)) return 1;
    {
        size_t len = strlen(output);
        const char *append = suffix;
        size_t name_len = strlen(cfg->output);
        if (!strcmp(cfg->target, "exe") && name_len >= 4 && !strcmp(cfg->output + name_len - 4, ".exe")) append = "";
        if (!strcmp(cfg->target, "elf") && name_len >= 4 && !strcmp(cfg->output + name_len - 4, ".elf")) append = "";
        if ((!strcmp(cfg->target, "asm") || !strcmp(cfg->target, "freestanding-asm")) &&
            name_len >= 2 && !strcmp(cfg->output + name_len - 2, ".s")) append = "";
        int n = snprintf(output + len, sizeof output - len, "/%s%s", cfg->output, append);
        if (n < 0 || (size_t)n >= sizeof output - len) {
            fprintf(stderr, "vbuild: output path is too long.\n"); return 1;
        }
    }
    import_graph_current = imports_are_current(source, output);
    if (import_graph_current < 0) return 1;
    build_inputs_current = build_inputs_are_current(output, cfg->target, toolchain);
    if (build_inputs_current < 0) return 1;
    fingerprint_ready = compute_fingerprint(source, compiler, toolchain, cfg->target,
                                            fingerprint, sizeof fingerprint);
    if (!fingerprint_ready) return 1;
    if (!check && !force_build && import_graph_current && build_inputs_current &&
        stat(output, &output_stat) == 0 &&
        manifest_matches(output, fingerprint)) {
        printf("Up to date: %s\n", output);
        return 0;
    }
    args[0] = compiler;
    if (check) {
        args[1] = "--dump-ir"; args[2] = source; args[3] = NULL;
    } else {
        args[1] = (char *)mode; args[2] = source; args[3] = "-o"; args[4] = output; args[5] = NULL;
    }
    if (run_process(args, toolchain) != 0) {
        fprintf(stderr, "vbuild: compiler command failed.\n"); return 1;
    }
    if (!check && !exists(output)) {
        fprintf(stderr, "vbuild: compiler returned success but output is missing: %s\n", output); return 1;
    }
    if (!check && !write_manifest(output, fingerprint)) return 1;
    if (!check) printf("Built %s (target=%s)\n", output, cfg->target);
    return 0;
}
static int clean_build(void) {
#ifdef _WIN32
    int rc = system("if exist build rmdir /s /q build");
#else
    int rc = system("rm -rf -- build");
#endif
    if (rc != 0) { fprintf(stderr, "vbuild: could not clean build/.\n"); return 1; }
    puts("Cleaned build/.");
    return 0;
}

static int run_one_test(Config *cfg, const char *filename) {
    Config test_cfg = *cfg;
    char output_path[PATH_CAP];
    char source_path[PATH_CAP];
    char *args[2];
    size_t name_len = strlen(filename);
    char name[256];
    int n;
    if (name_len < 5 || strcmp(filename + name_len - 4, ".vnt")) return 0;
    if (name_len - 4 >= sizeof name) {
        fprintf(stderr, "vbuild: test filename too long: %s\n", filename);
        return 1;
    }
    memcpy(name, filename, name_len - 4);
    name[name_len - 4] = '\0';
    if (!safe_filename(name) || snprintf(source_path, sizeof source_path, "tests/%s", filename) >= (int)sizeof source_path) {
        fprintf(stderr, "vbuild: invalid test filename: %s\n", filename);
        return 1;
    }
    if (!copy_value(test_cfg.source, sizeof test_cfg.source, source_path, "source") ||
        !copy_value(test_cfg.output, sizeof test_cfg.output, name, "output") ||
        !copy_value(test_cfg.target, sizeof test_cfg.target, "exe", "target")) return 1;
    printf("[test] %s\n", filename);
    if (run_compiler(&test_cfg, 0) != 0) return 1;
    if (!absolute_path("build", output_path, sizeof output_path)) return 1;
    {
        size_t len = strlen(output_path);
        n = snprintf(output_path + len, sizeof output_path - len, "/%s%s", name, EXE_SUFFIX);
        if (n < 0 || (size_t)n >= sizeof output_path - len) {
            fprintf(stderr, "vbuild: test output path too long.\n");
            return 1;
        }
    }
    args[0] = output_path;
    args[1] = NULL;
    return run_process(args, project_root) == 0 ? 0 : 1;
}

static int run_tests(Config *cfg) {
    int count = 0, failures = 0;
#ifdef _WIN32
    WIN32_FIND_DATAA data;
    HANDLE handle = FindFirstFileA("tests\\*.vnt", &data);
    if (handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "vbuild: no tests found. Add passing .vnt programs under tests/.\n");
        return 1;
    }
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        if (strlen(data.cFileName) < 5 || strcmp(data.cFileName + strlen(data.cFileName) - 4, ".vnt")) continue;
        count++;
        if (run_one_test(cfg, data.cFileName) != 0) failures++;
    } while (FindNextFileA(handle, &data));
    FindClose(handle);
#else
    DIR *dir = opendir("tests");
    struct dirent *entry;
    if (!dir) {
        fprintf(stderr, "vbuild: no tests directory found. Add passing .vnt programs under tests/.\n");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len < 5 || strcmp(entry->d_name + len - 4, ".vnt")) continue;
        count++;
        if (run_one_test(cfg, entry->d_name) != 0) failures++;
    }
    closedir(dir);
#endif
    if (count == 0) {
        fprintf(stderr, "vbuild: tests/ contains no .vnt test programs.\n");
        return 1;
    }
    if (failures) fprintf(stderr, "vbuild: %d of %d test(s) failed.\n", failures, count);
    else printf("VBuild tests passed: %d\n", count);
    return failures ? 1 : 0;
}

static int execute_action(Action action, Config *cfg) {
    if (action == ACT_BUILD) return run_compiler(cfg, 0);
    if (action == ACT_CHECK) return run_compiler(cfg, 1);
    if (action == ACT_CLEAN) return clean_build();
    if (action == ACT_TEST) return run_tests(cfg);
    if (action == ACT_RUN) {
        char output[PATH_CAP];
        char *args[2];
        size_t name_len = strlen(cfg->output);
        const char *suffix = EXE_SUFFIX;
        if (strcmp(cfg->target, "exe")) {
            fprintf(stderr, "vbuild: run requires target exe; selected target is '%s'.\n", cfg->target);
            return 1;
        }
        if (name_len >= 4 && !strcmp(cfg->output + name_len - 4, ".exe")) suffix = "";
        if (!absolute_path("build", output, sizeof output)) return 1;
        {
            size_t len = strlen(output);
            int n = snprintf(output + len, sizeof output - len, "/%s%s", cfg->output, suffix);
            if (n < 0 || (size_t)n >= sizeof output - len) {
                fprintf(stderr, "vbuild: output path is too long.\n"); return 1;
            }
        }
        if (!exists(output)) {
            fprintf(stderr, "vbuild: output not found; add 'depends build' to this task or run the build task first.\n");
            return 1;
        }
        args[0] = output; args[1] = NULL;
        return run_process(args, project_root);
    }
    return 0;
}

static int execute_task(int index, Config *cfg) {
    int i, rc;
    Task *task = &tasks[index];
    if (task->state == 1) {
        fprintf(stderr, "vbuild: dependency cycle detected at task '%s'.\n", task->name); return 1;
    }
    if (task->state == 2) return 0;
    task->state = 1;
    for (i = 0; i < task->dep_count; i++) {
        int dep = find_task(task->deps[i]);
        rc = execute_task(dep, cfg);
        if (rc) return rc;
    }
    printf("VBuild task: %s\n", task->name);
    rc = execute_action(task->action, cfg);
    if (rc) return rc;
    task->state = 2;
    return 0;
}

int main(int argc, char **argv) {
    Config cfg = {0};
    int index;
#ifdef _WIN32
    if (!_getcwd(project_root, sizeof project_root)) {
#else
    if (!getcwd(project_root, sizeof project_root)) {
#endif
        fprintf(stderr, "vbuild: cannot determine project directory: %s\n", strerror(errno));
        return 1;
    }
    if (argc < 2 || argc > 3 || (argc == 3 && strcmp(argv[2], "--force"))) {
        fprintf(stderr, "Usage: vbuild <task-name|init|tasks> [--force]\n");
        return 2;
    }
    force_build = argc == 3;
    if (!strcmp(argv[1], "init")) return init_project();
    if (!load_tasks()) return 1;
    if (!strcmp(argv[1], "tasks")) {
        int i;
        for (i = 0; i < task_count; i++) printf("%-20s action=%s dependencies=%d\n", tasks[i].name, action_label(tasks[i].action), tasks[i].dep_count);
        return 0;
    }
    if (!load_properties(&cfg)) return 1;
    index = find_task(argv[1]);
    if (index < 0) {
        fprintf(stderr, "vbuild: unknown task '%s'; run 'vbuild tasks'.\n", argv[1]);
        return 2;
    }
    return execute_task(index, &cfg);
}
