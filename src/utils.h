//
// Created by lucas on 11/24/26.
//

#ifndef ELEKTRONICKY_ZAPISNIK_UTILS_H
#define ELEKTRONICKY_ZAPISNIK_UTILS_H

#endif //ELEKTRONICKY_ZAPISNIK_UTILS_H

#define MAX_TITLE 50
#define MAX_TEXT 300
#define CSV_PATH "../data/notebook.csv"

// l - vytvoření datové struktury pro poznámku
typedef struct {
    int id;
    char title[MAX_TITLE];
    char text[MAX_TEXT];
    bool completed; // 0 - uncompleted, 1 = completed
    char complete_date[11]; // YYYY-MM-DD
} Note;

void defaultMenu(char *input);
Note *readCSV(int *out_count);
int handleList();
void cls();
void printNoteOptions();
Note *getNote(int pid);
int firstDayOfWeek(int month, int year);
int getDaysInMonth(int month, int year);
void showNoteDetail();
int getNextID();
bool isLeapYear(int year);
int isCorrectDate(char text[]);