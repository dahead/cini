// =====================================================================================================================
//
// dh INI helper v.0.1 (29.04.2024)
//
// =====================================================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// =====================================================================================================================

#define MAX_LINE 1024

// =====================================================================================================================

int read_value_from_file(const char *filename, const char *category, const char *key, char *value_out, size_t value_size);
int modify_value_in_file(const char *filename, const char *category, const char *key, const char *new_value);
void print_help();
int main(int argc, char *argv[]);

// =====================================================================================================================

int modify_value_in_file(const char *filename, const char *category, const char *key, const char *new_value) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file.\n");
        return -1;
    }

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
    if (!file) {
        printf("Error: Could not open file.\n");
        return -1;
    }

    char line[MAX_LINE];
    int in_category = 0, found_value = 0;

    // Check if both category and key are NULL to list all values
    if (category == NULL && key == NULL) {
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == '[') {
                in_category = 1; // New category found
            } else if (in_category && strchr(line, '=')) {
                printf("%s", line); // Ausgabe im Format [Category] Key=Value
            }
        }
        fclose(file);
        return 0; // Success, all values listed
    }

    // Reset file pointer for further checks
    rewind(file);

    // Check if category is not NULL and key is NULL
    if (category != NULL && key == NULL) {
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == '[') {
                in_category = (strstr(line, category) != NULL) ? 1 : 0;
            } else if (in_category && strchr(line, '=')) {
                printf("%s", line); // Ausgabe aller Keys und Values der Kategorie
                found_value = 1;
            }
        }
        fclose(file);
        return found_value ? 0 : -1; // Return success or failure if no values found
    }

    // Reset file pointer to read values based on category and key
    rewind(file);

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') {
            in_category = (category && strstr(line, category)) ? 1 : 0;
        }

        if (in_category) {
            if (key) {
                if (strstr(line, key)) {
                    char *key_part = strtok(line, "=");
                    char *value_part = strtok(NULL, "\n");
                    if (key_part && value_part && strcmp(key_part, key) == 0) {
                        if (value_out) {
                            strncpy(value_out, value_part, value_size - 1);
                            value_out[value_size - 1] = '\0';
                        }
                        printf("%s\n", value_part); // Ausgabe des Wertes
                        found_value = 1;
                    }
                }
            } else {
                if (strchr(line, '=')) {
                    printf("%s", line); // Ausgabe aller Keys und Values in der Kategorie
                    found_value = 1;
                }
            }
        }
    }

    fclose(file);

    if (!found_value) {
        printf("Key '%s' not found.\n", key); // Fehlerausgabe
        return -1;
    }

    return 0;
}


// help function
void print_help() {
    printf("dh INI helper v.0.2 (29.04.2024)\n");
    printf("Usage:\n");
    printf("  r/read  <filename> <category> <key>             - Reads the value of a key under a category\n");
    printf("  w/write <filename> <category> <key> <new_value> - Updates the value of a key under a category\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_help();
        return -1;
    }

    // parse arguments
    const char *command = argv[1];
    const char *filename = argv[2];
    const char *category = argc > 3 ? argv[3] : NULL;
    const char *key = argc > 4 ? argv[4] : NULL;
    const char *value = argc > 5 ? argv[5] : NULL;

    // check if file exists
    struct stat buffer;
    if (stat(filename, &buffer) != 0) {
        printf("File does not exist: %s\n", filename);
        return -1;
    }

    // read
    if (strcmp(command, "r") == 0 || strcmp(command, "read") == 0) {
        read_value_from_file(filename, category, key, NULL, 0);
    }
    // write
    else if (strcmp(command, "w") == 0 || strcmp(command, "write") == 0) {
        if (category) {
            modify_value_in_file(filename, category, key, value);
        } else {
            printf("Error: Category is required for write command.\n");
        }
    } else {
        print_help();
    }

    return 0;
}