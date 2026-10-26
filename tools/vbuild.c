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
#define MKDIR(path) _mkdir(path)
#define EXE_SUFFIX ".exe"
#else
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0777)
#define EXE_SUFFIX ""
#endif

#define LINE_CAP 4096
#define PATH_CAP 2048

typedef struct {
    char name[256];
    char version[128];
    char source[PATH_CAP];
    char output[256];
    char compiler[PATH_CAP];
} Config;

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
            snprintf(cfg->name, sizeof cfg->name, "%s", value); got_name = 1;
        } else if (!strcmp(key, "project.version")) {
            snprintf(cfg->version, sizeof cfg->version, "%s", value);
        } else if (!strcmp(key, "source")) {
            snprintf(cfg->source, sizeof cfg->source, "%s", value); got_source = 1;
        } else if (!strcmp(key, "output")) {
            snprintf(cfg->output, sizeof cfg->output, "%s", value);
        } else if (!strcmp(key, "compiler")) {
            snprintf(cfg->compiler, sizeof cfg->compiler, "%s", value);
        } else {
            fprintf(stderr, "vbuild: unknown property '%s'.\n", key);
            fclose(f); return 0;
        }
    }
    fclose(f);
    if (!got_name || !cfg->name[0]) {
        fprintf(stderr, "vbuild: project.name is required.\n"); return 0;
    }
    if (!got_source || !cfg->source[0]) {
        fprintf(stderr, "vbuild: source is required.\n"); return 0;
    }
    if (!cfg->output[0]) snprintf(cfg->output, sizeof cfg->output, "%s", cfg->name);
    if (!safe_filename(cfg->output)) {
        fprintf(stderr, "vbuild: output must be a file name inside build/.\n"); return 0;
    }
    if (!cfg->version[0]) snprintf(cfg->version, sizeof cfg->version, "0.1.0");
    if (!cfg->compiler[0]) snprintf(cfg->compiler, sizeof cfg->compiler, "vnt");
    return 1;
}

static int exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static int ensure_build_dir(void) {
    if (exists("build")) return 1;
    if (MKDIR("build") == 0 || errno == EEXIST) return 1;
    fprintf(stderr, "vbuild: cannot create build/: %s\n", strerror(errno));
    return 0;
}

static int write_file(const char *path, const char *contents) {
    FILE *f = fopen(path, "wx");
    if (!f) {
        fprintf(stderr, "vbuild: refusing to overwrite %s (%s).\n", path, strerror(errno));
        return 0;
    }
    if (fputs(contents, f) < 0 || fclose(f) != 0) {
        fprintf(stderr, "vbuild: failed writing %s.\n", path); return 0;
    }
    return 1;
}

static int init_project(void) {
    const char *name = "hello";
    char properties[1024];
    char script[1024];
    if (exists("vbuild.properties") || exists("build.vbuild")) {
        fprintf(stderr, "vbuild: build.vbuild or vbuild.properties already exists; refusing to overwrite.\n");
        return 1;
    }
    snprintf(properties, sizeof properties,
        "project.name = %s\nproject.version = 0.1.0\nsource = src/main.vnt\noutput = hello\ncompiler = vnt\n", name);
    snprintf(script, sizeof script,
        "# VBuild task file (version 1)\n"
        "# Built-in tasks are implemented by the native vbuild executable.\n"
        "task build\n"
        "task run depends build\n"
        "task check\n"
        "task clean\n"
        "task test\n");
    if (!write_file("vbuild.properties", properties)) return 1;
    if (!write_file("build.vbuild", script)) return 1;
    if (!exists("src") && MKDIR("src") != 0) {
        fprintf(stderr, "vbuild: cannot create src/: %s\n", strerror(errno)); return 1;
    }
    if (!write_file("src/main.vnt", "print(\"Hello from VNT\")\n")) return 1;
    printf("Initialized native VBuild project '%s'.\n", name);
    return 0;
}

static int validate_script(void) {
    FILE *f = fopen("build.vbuild", "r");
    char line[LINE_CAP];
    int build = 0, run = 0, check = 0, clean = 0, test = 0;
    if (!f) {
        fprintf(stderr, "vbuild: missing build.vbuild.\n"); return 0;
    }
    while (fgets(line, sizeof line, f)) {
        char *p = line;
        trim(p);
        if (!p[0] || p[0] == '#') continue;
        if (!strcmp(p, "task build")) build = 1;
        else if (!strcmp(p, "task run depends build")) run = 1;
        else if (!strcmp(p, "task check")) check = 1;
        else if (!strcmp(p, "task clean")) clean = 1;
        else if (!strcmp(p, "task test")) test = 1;
        else {
            fprintf(stderr, "vbuild: invalid build.vbuild directive: %s\n", p);
            fclose(f); return 0;
        }
    }
    fclose(f);
    if (!build || !run || !check || !clean || !test) {
        fprintf(stderr, "vbuild: build.vbuild must declare build, run depends build, check, clean and test tasks.\n");
        return 0;
    }
    return 1;
}

static int run_compiler(Config *cfg, const char *mode) {
    char command[PATH_CAP * 3 + 512];
    char output[PATH_CAP];
    int rc;
    if (!exists(cfg->source)) {
        fprintf(stderr, "vbuild: source file not found: %s\n", cfg->source); return 1;
    }
    if (!ensure_build_dir()) return 1;
    snprintf(output, sizeof output, "build/%s%s", cfg->output, EXE_SUFFIX);
    /* Compiler/source paths are quoted for directories containing spaces. */
    if (!strcmp(mode, "check"))
        snprintf(command, sizeof command, "\"%s\" --dump-ir \"%s\"", cfg->compiler, cfg->source);
    else
        snprintf(command, sizeof command, "\"%s\" --compile \"%s\" -o \"%s\"", cfg->compiler, cfg->source, output);
    rc = system(command);
    if (rc != 0) {
        fprintf(stderr, "vbuild: compiler command failed (status %d).\n", rc);
        return 1;
    }
    if (!strcmp(mode, "build")) printf("Built %s\n", output);
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

int main(int argc, char **argv) {
    Config cfg = {{0}, {0}, {0}, {0}, {0}};
    const char *cmd;
    if (argc != 2) {
        fprintf(stderr, "Usage: vbuild <init|build|run|check|test|clean>\n");
        return 2;
    }
    cmd = argv[1];
    if (!strcmp(cmd, "init")) return init_project();
    if (!validate_script()) return 1;
    if (!load_properties(&cfg)) return 1;
    if (!strcmp(cmd, "build")) return run_compiler(&cfg, "build");
    if (!strcmp(cmd, "check")) return run_compiler(&cfg, "check");
    if (!strcmp(cmd, "run")) {
        char executable[PATH_CAP];
        int rc = run_compiler(&cfg, "build");
        if (rc) return rc;
        snprintf(executable, sizeof executable, "build/%s%s", cfg.output, EXE_SUFFIX);
        return system(executable);
    }
    if (!strcmp(cmd, "clean")) return clean_build();
    if (!strcmp(cmd, "test")) {
        fprintf(stderr, "vbuild: test task is declared but test discovery/execution is not implemented yet.\n");
        return 2;
    }
    fprintf(stderr, "vbuild: unknown command '%s'.\n", cmd);
    return 2;
}
