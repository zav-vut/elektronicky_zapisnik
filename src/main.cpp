#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

#define MAX_TITLE 50
#define MAX_TEXT 300
#define CSV_PATH "../data/notebook.csv"

typedef struct {
    int id;
    char title[MAX_TITLE];
    char text[MAX_TEXT];
    bool completed; // 0 - uncompleted, 1 = completed
    char complate_date[11]; // YYYY-MM-DD
} Note;

void defaultMenu(char *input);
Note *readCSV();
void handleList();
void cls();

int main() {
    char input = ' ';
    while (true) {
        defaultMenu(&input);
        switch (input) {
            case 'a':
                break;
            case 'c':
                break;
            case 'l':
                handleList();
                break;
            case 'e':
                exit(1);
        }
    }
}

void defaultMenu(char *input) {
    cls();
    printf("\n----- Menu -----\n");
    printf("[a] Add new note\n");
    printf("[c] Show calendar\n");
    printf("[l] List notes\n");
    printf("[e] Exit\n\n");

    printf("Enter character from menu without brackets: ");
    scanf("%c", input);
}

Note *readCSV(int *out_count) {
    FILE *fp = fopen(CSV_PATH, "r");

    // If the file does not exist, create it with a header line and return NULL
    if (fp == NULL) {
        printf("Can't open file. Creating new empty CSV.\n");
        fp = fopen(CSV_PATH, "w");
        if (fp != NULL) {
            fprintf(fp, "id|title|content|completed|due_date\n");
            fclose(fp);
        }
        if (out_count) *out_count = 0;
        return NULL;
    }

    int capacity = 5;
    int count = 0; // Track total valid records loaded (0-indexed)

    Note *ptr = (Note *) malloc(capacity * sizeof(Note));
    if (ptr == NULL) {
        printf("Not enough memory\n");
        fclose(fp);
        exit(1);
    }

    char buffer[1024];

    // Skip header line (Line 1)
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Remove trailing newline character (\n or \r\n)
        buffer[strcspn(buffer, "\r\n")] = 0;

        // Skip empty lines
        if (buffer[0] == '\0') continue;

        // Tokenize columns using '|' delimiter
        char *id            = strtok(buffer, "|");
        char *title         = strtok(NULL, "|");
        char *content       = strtok(NULL, "|");
        char *completed     = strtok(NULL, "|");
        char *complate_date = strtok(NULL, "|");

        // Ensure all required fields exist before saving
        if (id && title && content && completed && complate_date) {

            // Expand dynamic array when full
            if (count >= capacity) {
                capacity *= 2;
                Note *tmp = (Note *) realloc(ptr, capacity * sizeof(Note));
                if (tmp == NULL) {
                    printf("Not enough memory during reallocation\n");
                    free(ptr);
                    fclose(fp);
                    exit(1);
                }
                ptr = tmp;
            }

            // Write values to the struct array at index 'count'
            ptr[count].id = atoi(id);

            strncpy(ptr[count].title, title, sizeof(ptr[count].title) - 1);
            ptr[count].title[sizeof(ptr[count].title) - 1] = '\0';

            strncpy(ptr[count].complate_date, complate_date, sizeof(ptr[count].complate_date) - 1);
            ptr[count].complate_date[sizeof(ptr[count].complate_date) - 1] = '\0';

            ptr[count].completed = (atoi(completed) != 0);

            count++; // Increment count for the next valid item
        }
    }

    fclose(fp);

    // Pass total count back to the caller if requested
    if (out_count) {
        *out_count = count;
    }

    return ptr;
}
void handleList() {
    int out_count = 0;
    Note *ptr =readCSV(&out_count);
    if (ptr == NULL) {
        printf("File is corrupted\n");
        free(ptr);
        exit(1);
    }
    cls();
    printf("\n| %-5s | %-20s | %-10s | %-1s\n", "id", "title", "due_date", "completed");
    printf("--------------------------------------------------------\n");
    // Verify all 4 expected fields were found before printing
    for (int i =0; i < out_count; i++) {
        printf("| %-5d | %-20s | %-10s | %-1i\n", ptr[i].id, ptr[i].title, ptr[i].complate_date, ptr[i].completed);
    }

    char input[5];
    int choice;

    printf("\n");
    while (fgets(input, sizeof(input), stdin)) {
        if (sscanf(input, "%d", &choice) == 1) {
            break;
        }else {
            printf("Select note[id]: ");

        }
    }
}
void cls() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
