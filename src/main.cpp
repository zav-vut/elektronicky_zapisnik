#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "utils.h"

#define MAX_TITLE 50
#define MAX_TEXT 300
#define CSV_PATH "../data/notebook.csv"

// l - vytvoření datové struktury pro poznámku

void defaultMenu(char *input);
Note *readCSV();
int handleList();
void cls();
void printNoteOptions();
int chooseNoteOptions(char choice, int id);
Note *getNote(int pid);
void showCalendar();
int firstDayOfWeek(int month, int year);
int getDaysInMonth(int month, int year);
void addNote();
void showNoteDetail();
void updateNote(int id);
void deleteNote(int id);
int getNextID();
bool isLeapYear(int year);
int isCorrectDate(char text[]);

// Hlavní runtime aplikace
int main() {
    char input = ' ';
    while (true) {
        defaultMenu(&input);
        switch (input) {
            case 'a':
                addNote();
                break;
            case 'c':
                showCalendar();
                break;
            case 'l':
                handleList();
                break;
            case 'e':
                exit(1);
        }
    }
}


// l - vypisovani poznamek
// vybrani poznamky a nasledne reseni akce pro urcitou poznamku
int handleList() {
    int optionChoice = 2;
    // pouzivam zde loop proto kdyz chci jit zpatky s volbou back tak abych se objevil v menu souboru
    while (optionChoice == 2) {
        int out_count = 0;
        Note *ptr =readCSV(&out_count);
        if (ptr == NULL) {
            printf("File is corrupted\n");
            free(ptr);
            exit(1);
        }
        cls();
        // Vypsani poznamek z CSV
        printf("\n| %-5s | %-20s | %-10s | %-1s\n", "id", "title", "due_date", "completed");
        printf("--------------------------------------------------------\n");
        // Verify all 4 expected fields were found before printing
        for (int i =0; i < out_count; i++) {
            printf("| %-5d | %-20s | %-10s | %-1i\n", ptr[i].id, ptr[i].title, ptr[i].complete_date, ptr[i].completed);
        }
        free(ptr);

        char input[5];
        int choice;
        char inputN = ' ';
        printf("\nSelect note [id] or back [b]: ");

        // cekani na vybrani poznamky
        while (fgets(input, sizeof(input), stdin)) {
            // pokud uzivatel zada b pro back tak se vrati do nabidky default menu
            if (input[0] == 'b' && (input[1] == '\n' || input[1] == '\0')) {
                return 0;
            }
            // nacitam id
            if (sscanf(input, "%d", &choice) == 1) {
                break;
            }else {
                printf("Select note[id]/back[b]: ");
            }
        }
        // vypsani nabidky pro akce
        printNoteOptions();
        // cekani na uzivatelovu volbu akce
        // kdyz 2 tak se vrati do nabidky poznamek
        do {
            printf("Enter action: ");
            scanf(" %c", &inputN);
            optionChoice = chooseNoteOptions(inputN, choice);
        }while (optionChoice == 0);
    }
}


// l - handler pro akce se souborem
int chooseNoteOptions(char choice, int id) {
    switch (choice) {
        case 'r': {
            Note *note = getNote(id);
            if (note != NULL) {
                printf("\n--- NOTE DETAILS ---\n");
                printf("ID: %d\n", note->id);
                printf("Title: %s\n", note->title);
                printf("Note: %s\n", note->text);
                printf("Creation date: %s\n", note->complete_date);
                
                if (note->completed == 1) {
                    printf("Progress: COMPLETED\n");
                } else {
                    printf("Progress: INCOMPLETE\n");
                }
                
                free(note);
            } else {
                printf("Note [ID: %d] does not exist!\n", id);
            }

            // vycistenie bufferu, uzivatel musi stlacit enter
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\nPress enter to go back...");
            getchar();
            return 2; // vratime sa do zoznamu s notes
        }

        //pri u a d necisitm buffer, robim to v samotnych funkciach (user by ho inak musel stlacit 3 krat)
        case 'u': {
            updateNote(id);
            printf("\nPress Enter to return!");
            getchar();
            return 2;
        }

        case 'd': {
            deleteNote(id);
            printf("\nPress Enter to return!");
            getchar();
            return 2;
        }

        case 'b':
            // printf("Back");
            return 2;
            break;
        case 'e':
            printf("Exit");
            exit(1);
        default:
            return 0;
    }
    return 1;
}

