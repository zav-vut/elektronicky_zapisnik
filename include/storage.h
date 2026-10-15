//
// Created by lucas on 10/15/26.
//

#ifndef ELEKTRONICKY_ZAPISNIK_STORAGE_H
#define ELEKTRONICKY_ZAPISNIK_STORAGE_H

#define MAX_TITLE 50
#define MAX_TEXT 300

typedef struct {
    int id;
    char title[MAX_TITLE];
    char text[MAX_TEXT];
    int completed; // 0 - uncompleted, 1 = completed
    char complate_date[11]; // YYYY-MM-DD
} Note;

#endif //ELEKTRONICKY_ZAPISNIK_STORAGE_H
