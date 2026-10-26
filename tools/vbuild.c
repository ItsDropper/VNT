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
} Config;

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
    if (!cfg->version[0]) strcpy(cfg->version, "0.1.0");
    if (!cfg->compiler[0]) strcpy(cfg->compiler, "vnt");
    return 1;
}

static int exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
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
        "project.name = hello\nproject.version = 0.1.0\nsource = src/main.vnt\noutput = hello\ncompiler = vnt\n";
    const char *script =
        "# VBuild task file (version 1)\n"
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

static int find_task(const char *name) {
    int i;
    for (i = 0; i < task_count; i++) if (!strcmp(tasks[i].name, name)) return i;
    return -1;
}

/* Grammar: task NAME = ACTION [depends NAME[, NAME...]] */
static int load_tasks(void) {
    FILE *f = fopen("build.vbuild", "r");
    char line[LINE_CAP];
    int lineno = 0;
    if (!f) { fprintf(stderr, "vbuild: missing build.vbuild.\n"); return 0; }
    task_count = 0;
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

static int run_process(char *const argv[]) {
#ifdef _WIN32
    int rc = _spawnvp(_P_WAIT, argv[0], (const char * const *)argv);
    if (rc == -1) { fprintf(stderr, "vbuild: cannot launch '%s': %s\n", argv[0], strerror(errno)); return 127; }
    return rc;
#else
    pid_t pid = fork();
    int status;
    if (pid < 0) { fprintf(stderr, "vbuild: fork failed: %s\n", strerror(errno)); return 127; }
    if (pid == 0) { execvp(argv[0], argv); fprintf(stderr, "vbuild: cannot launch '%s': %s\n", argv[0], strerror(errno)); _exit(127); }
    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR) continue;
        fprintf(stderr, "vbuild: waitpid failed: %s\n", strerror(errno)); return 127;
    }
    return WAIT_STATUS(status);
#endif
}

static int run_compiler(Config *cfg, int check) {
    char output[PATH_CAP];
    char *args[6];
    if (!exists(cfg->source)) {
        fprintf(stderr, "vbuild: source file not found: %s\n", cfg->source); return 1;
    }
    if (!check && !ensure_build_dir()) return 1;
    snprintf(output, sizeof output, "build/%s%s", cfg->output, EXE_SUFFIX);
    args[0] = cfg->compiler;
    if (check) {
        args[1] = "--dump-ir"; args[2] = cfg->source; args[3] = NULL;
    } else {
        args[1] = "--compile"; args[2] = cfg->source; args[3] = "-o"; args[4] = output; args[5] = NULL;
    }
    if (run_process(args) != 0) {
        fprintf(stderr, "vbuild: compiler command failed.\n"); return 1;
    }
    if (!check && !exists(output)) {
        fprintf(stderr, "vbuild: compiler returned success but output is missing: %s\n", output); return 1;
    }
    if (!check) printf("Built %s\n", output);
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

static int execute_action(Action action, Config *cfg) {
    if (action == ACT_BUILD) return run_compiler(cfg, 0);
    if (action == ACT_CHECK) return run_compiler(cfg, 1);
    if (action == ACT_CLEAN) return clean_build();
    if (action == ACT_TEST) {
        fprintf(stderr, "vbuild: test discovery/execution is not implemented yet.\n"); return 2;
    }
    if (action == ACT_RUN) {
        char output[PATH_CAP];
        char *args[2];
        snprintf(output, sizeof output, "build/%s%s", cfg->output, EXE_SUFFIX);
        if (!exists(output)) {
            fprintf(stderr, "vbuild: output not found; add 'depends build' to this task or run the build task first.\\n");
            return 1;
        }
        args[0] = output; args[1] = NULL;
        return run_process(args);
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
    Config cfg = {{0}, {0}, {0}, {0}, {0}};
    int index;
    if (argc != 2) {
        fprintf(stderr, "Usage: vbuild <task-name|init|tasks>\n");
        return 2;
    }
    if (!strcmp(argv[1], "init")) return init_project();
    if (!load_tasks()) return 1;
    if (!strcmp(argv[1], "tasks")) {
        int i;
        for (i = 0; i < task_count; i++) printf("%-20s action=%d dependencies=%d\n", tasks[i].name, (int)tasks[i].action, tasks[i].dep_count);
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
