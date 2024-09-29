#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_LINE 1024

int read_value_from_file(const char *filename, const char *category, const char *key, char *value_out, size_t value_size);
int modify_value_in_file(const char *filename, const char *category, const char *key, const char *new_value);
void list_keys_values(const char *filename, const char *category);
void list_categories(const char *filename);

int modify_value_in_file(const char *filename, const char *category, const char *key, const char *new_value) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        fclose(file);
        return -1;
    }

    char line[MAX_LINE];
    int in_category = 0, key_found = 0, category_found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') {
            if (strstr(line, category)) {
                in_category = 1;
                category_found = 1;
                fputs(line, temp_file); // Write the category line
            } else {
                if (in_category && !key_found && key && new_value) {
                    fprintf(temp_file, "%s=%s\n", key, new_value);
                }
                in_category = 0;
                fputs(line, temp_file); // Write the non-matching category line
            }
        } else if (in_category && strstr(line, key)) {
            fprintf(temp_file, "%s=%s\n", key, new_value);
            key_found = 1;
        } else {
            fputs(line, temp_file); // Write other lines
        }
    }

    if (!category_found) fprintf(temp_file, "[%s]\n%s=%s\n", category, key, new_value);
    else if (in_category && !key_found && key && new_value) fprintf(temp_file, "%s=%s\n", key, new_value);

    fclose(file);
    fclose(temp_file);
    remove(filename);
    rename("temp.txt", filename);
}

int read_value_from_file(const char *filename, const char *category, const char *key, char *value_out, size_t value_size) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    char line[MAX_LINE];
    int in_category = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') {
            if (strstr(line, category)) {
                in_category = 1;
            } else {
                in_category = 0;
            }
        }

        if (in_category && strstr(line, key)) {
            char *key_part = strtok(line, "=");
            char *value_part = strtok(NULL, "\n");
            if (key_part && value_part && strcmp(key_part, key) == 0) {
                strncpy(value_out, value_part, value_size - 1);
                value_out[value_size - 1] = '\0';
                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return -1;
}

void list_keys_values(const char *filename, const char *category) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file.\n");
        return;
    }

    char line[MAX_LINE];
    int in_category = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') {
            if (category) {
                if (strncmp(line + 1, category, strlen(category)) == 0 && line[strlen(category) + 1] == ']') {
                    in_category = 1; // Category found
                } else {
                    in_category = 0; // New category found
                }
            } else {
                in_category = 1; // No category specified, consider it as in-category
            }
        } else if (in_category && strchr(line, '=')) {
            printf("%s", line); // Print only keys and values
        }
    }

    fclose(file);
}

void list_categories(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file.\n");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') {
            char *start = strchr(line, '[') + 1;
            char *end = strchr(line, ']');
            if (end) *end = '\0';
            printf("%s\n", start);
        }
    }

    fclose(file);
}

void list_values(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("File not found.\n");
        return;
    }

    char line[MAX_LINE];
    int in_category = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') {
            if (strchr(line, ']')) {
                in_category = 1;
            } else {
                in_category = 0;
            }
        } else if (in_category && strchr(line, '=')) {
            printf("%s", line);
        }
    }

    fclose(file);
}

void print_help() {
    printf("dh INI helper v.0.1 (29.04.2024)\n");
    printf("Usage:\n");
    printf("  r  <filename> <category> <key>             - Reads the value of a key under a category\n");
    printf("  w  <filename> <category> <key> <new_value> - Updates the value of a key under a category\n");
    printf("  lk <filename> <category>                   - Lists all keys and values under a category (or all categories if none is given)\n");
    printf("  lc <filename>                              - Lists all categories in the file\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_help();
        return -1;
    }

    const char *command = argv[1];
    const char *filename = argv[2];
    const char *category = argc > 3 ? argv[3] : NULL;
    const char *key = argc > 4 ? argv[4] : NULL;
    const char *value = argc > 5 ? argv[5] : NULL;

    struct stat buffer;
    if (stat(filename, &buffer) != 0) {
        printf("File does not exist: %s\n", filename);
        return -1;
    }

    if (strcmp(command, "r") == 0) {
        if (category) {
            if (key) {
                char value_out[256];
                if (read_value_from_file(filename, category, key, value_out, sizeof(value_out)) == 0) {
                    printf("Value: %s\n", value_out);
                } else {
                    printf("Key not found.\n");
                }
            } else {
                list_keys_values(filename, category);
            }
        } else {
            list_values(filename);
        }
    }
    // write
    else if (strcmp(command, "w") == 0) {
        if (category) {
            modify_value_in_file(filename, category, key, value);
            printf("Value updated or category added.\n");
        } else {
            printf("Category is required for write command.\n");
        }
    // list keys
    } else if (strcmp(command, "lk") == 0) {
        list_keys_values(filename, category);
    // list categories
    } else if (strcmp(command, "lc") == 0) {
        list_categories(filename);
    // list values
    } else if (strcmp(command, "lv") == 0) {
        list_values(filename);
    } else {
        print_help();
    }

    return 0;
}