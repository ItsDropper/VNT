#include <vnt/modules.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char **paths;
    int count;
    int capacity;
} Loader;

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Module error: could not open '%s'.\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if (size < 0) {
        fclose(file);
        return NULL;
    }

    char *buffer = malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Module error: out of memory.\n");
        return NULL;
    }

    size_t bytes = fread(buffer, 1, (size_t)size, file);
    fclose(file);
    buffer[bytes] = '\0';
    return buffer;
}

static int seen(Loader *loader, const char *path) {
    for (int i = 0; i < loader->count; ++i)
        if (!strcmp(loader->paths[i], path))
            return 1;
    return 0;
}

static int remember(Loader *loader, const char *path) {
    if (seen(loader, path))
        return 1;

    if (loader->count == loader->capacity) {
        int cap = loader->capacity ? loader->capacity * 2 : 16;
        char **paths = realloc(loader->paths, sizeof(*paths) * cap);
        if (!paths)
            return 0;
        loader->paths = paths;
        loader->capacity = cap;
    }

    loader->paths[loader->count] = strdup(path);
    if (!loader->paths[loader->count])
        return 0;

    loader->count++;
    return 1;
}

static const char *directory_of(const char *path, char *buffer, size_t size) {
    const char *slash = strrchr(path, '/');
    const char *backslash = strrchr(path, '\\');
    if (backslash && (!slash || backslash > slash))
        slash = backslash;

    if (!slash) {
        snprintf(buffer, size, ".");
        return buffer;
    }

    size_t length = (size_t)(slash - path);
    if (length >= size)
        return NULL;

    memcpy(buffer, path, length);
    buffer[length] = '\0';
    return buffer;
}

static char *join_path(const char *directory, const char *name) {
    size_t a = strlen(directory);
    size_t b = strlen(name);
    char separator = '\\';

    char *path = malloc(a + b + 2);
    if (!path)
        return NULL;

    memcpy(path, directory, a);
    if (a && directory[a - 1] != '/' && directory[a - 1] != '\\')
        path[a++] = separator;
    memcpy(path + a, name, b + 1);
    return path;
}

static int is_import_line(const char *line, char *module, size_t module_size) {
    while (isspace((unsigned char)*line))
        line++;

    if (strncmp(line, "import", 6) != 0 ||
        !(isspace((unsigned char)line[6]) || line[6] == '"'))
        return 0;

    line += 6;
    while (isspace((unsigned char)*line))
        line++;

    if (*line != '"')
        return 0;
    line++;

    size_t length = 0;
    while (*line && *line != '"') {
        if (length + 1 >= module_size)
            return 0;
        module[length++] = *line++;
    }

    if (*line != '"')
        return 0;

    module[length] = '\0';
    return 1;
}

static int append_text(char **output, size_t *length, size_t *capacity,
                       const char *text) {
    size_t add = strlen(text);
    if (*length + add + 1 > *capacity) {
        size_t cap = *capacity ? *capacity : 4096;
        while (*length + add + 1 > cap)
            cap *= 2;

        char *grown = realloc(*output, cap);
        if (!grown)
            return 0;
        *output = grown;
        *capacity = cap;
    }

    memcpy(*output + *length, text, add);
    *length += add;
    (*output)[*length] = '\0';
    return 1;
}

static int load_recursive(Loader *loader, const char *path,
                          char **output, size_t *length, size_t *capacity) {
    if (seen(loader, path))
        return 1;

    if (!remember(loader, path)) {
        fprintf(stderr, "Module error: out of memory.\n");
        return 0;
    }

    char *source = read_file(path);
    if (!source)
        return 0;

    char directory[4096];
    if (!directory_of(path, directory, sizeof(directory))) {
        free(source);
        fprintf(stderr, "Module error: path is too long.\n");
        return 0;
    }

    char *cursor = source;
    while (*cursor) {
        char *line_end = strchr(cursor, '\n');
        if (!line_end)
            line_end = cursor + strlen(cursor);

        size_t line_length = (size_t)(line_end - cursor);
        char *line = malloc(line_length + 1);
        if (!line) {
            free(source);
            return 0;
        }

        memcpy(line, cursor, line_length);
        line[line_length] = '\0';

        char module[4096];
        if (is_import_line(line, module, sizeof(module))) {
            char *module_path = join_path(directory, module);
            if (!module_path ||
                !load_recursive(loader, module_path, output, length, capacity)) {
                free(module_path);
                free(line);
                free(source);
                return 0;
            }
            free(module_path);
        } else {
            if (!append_text(output, length, capacity, line) ||
                !append_text(output, length, capacity, "\n")) {
                free(line);
                free(source);
                return 0;
            }
        }

        free(line);
        cursor = *line_end ? line_end + 1 : line_end;
    }

    free(source);
    return 1;
}

char *vnt_load_project_source(const char *entry_path) {
    Loader loader = {0};
    char *output = NULL;
    size_t length = 0;
    size_t capacity = 0;

    if (!load_recursive(&loader, entry_path, &output, &length, &capacity)) {
        free(output);
        output = NULL;
    }

    for (int i = 0; i < loader.count; ++i)
        free(loader.paths[i]);
    free(loader.paths);

    return output;
}
