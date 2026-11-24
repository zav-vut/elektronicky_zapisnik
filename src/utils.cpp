//
// Created by lucas on 11/24/26.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "utils.h"
// l - vypsání menu a požádání o vstup
void defaultMenu(char *input) {
    cls();
    printf("\n----- Menu -----\n");
    printf("[a] Add new note\n");
    printf("[c] Show calendar\n");
    printf("[l] List notes\n");
    printf("[e] Exit\n\n");

    printf("Enter character from menu without brackets: ");
    scanf(" %c", input);
}
// l - čtení celého CSV souboru a vracení pointeru na list struktur
// potřeba uvlonění pointeru to zavolání funkce free(fp)
Note *readCSV(int *out_count) {
    FILE *fp = fopen(CSV_PATH, "r");

    // check jestli nemohu precist CSV
    if (fp == NULL) {
        printf("Can't open file. Creating new empty CSV.\n");
        fp = fopen(CSV_PATH, "w");
        // pokud neni vytvoreny soubor, vytvoriho a prida k nemu hlavicku
        if (fp != NULL) {
            fprintf(fp, "id|title|content|completed|due_date\n");
            fclose(fp);
        }
        if (out_count) *out_count = 0;
        return NULL;
    }

    // nastavy zakladni kapacitu na 5 poznamek a nesledne pokud nestaci kapacita tak se 2x zvetsi
    int capacity = 5;
    int count = 0; // Track total valid records loaded (0-indexed)

    Note *ptr = (Note *) malloc(capacity * sizeof(Note));
    if (ptr == NULL) {
        printf("Not enough memory\n");
        fclose(fp);
        exit(1);
    }

    char buffer[1024];

    // preskocit 1. radek (header)
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // odstraneni znaku \n pro novy radek a \r
        buffer[strcspn(buffer, "\r\n")] = 0;

        // preskoceni prazdnych radku
        if (buffer[0] == '\0') continue;

        // rozdelit do pole pomoci delimetru |
        char *id = strtok(buffer, "|");
        char *title = strtok(NULL, "|");
        char *content = strtok(NULL, "|");
        char *completed = strtok(NULL, "|");
        char *complete_date = strtok(NULL, "|");

        // ujisteni ze v kazdem poli neco je
        if (id && title && content && completed && complete_date) {

            // rozsireni dynamicke pameti kdyz nestaci
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

            // prirazeni hodnot z lokalich promenych do struktury dynamicke pameti
            ptr[count].id = atoi(id);

            strncpy(ptr[count].title, title, sizeof(ptr[count].title) - 1);
            ptr[count].title[sizeof(ptr[count].title) - 1] = '\0';

            strncpy(ptr[count].complete_date, complete_date, sizeof(ptr[count].complete_date) - 1);
            ptr[count].complete_date[sizeof(ptr[count].complete_date) - 1] = '\0';

            ptr[count].completed = (atoi(completed) != 0);

            count++; // pocitani kolik poznamek tam je
        }
    }

    fclose(fp);

    // predani poctu poznamemek pokud uzivatel preda pointer na out_count
    if (out_count) {
        *out_count = count;
    }

    return ptr;
}

// l - vycisteni obrazovky
void cls() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// l - vypsani nabidky pro jednotlivou poznamku
void printNoteOptions() {
    cls();
    printf("Read[r]\n");
    printf("Update[u]\n");
    printf("Delete[d]\n");
    printf("Back[b]\n");
    printf("Exit[e]\n\n");
}
// l - vraci hodnotu jedne poznamky vybranou pomoci id(pid)
Note *getNote(int pid) {
    FILE *fp = fopen(CSV_PATH, "r");
    if (fp == NULL) {
        printf("Error: File could not be opened!\n");
        return NULL;
    }
    char buffer[1024];
    // Preskočíme prvý riadok s hlavičkou
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Odstránenie konca riadka
        buffer[strcspn(buffer, "\r\n")] = 0;

        // Rozdelenie riadka podľa zvislej čiary |
        char *id = strtok(buffer, "|");
        char *title = strtok(NULL, "|");
        char *content = strtok(NULL, "|");
        char *completed = strtok(NULL, "|");
        char *complete_date = strtok(NULL, "|");

        // Kontrola, či sa načítali všetky časti
        if (id && title && content && completed && complete_date) {
            if (atoi(id) == pid) {
                // Našli sme hľadanú poznámku, vytvoríme pre ňu pamäť
                Note *ptr = (Note *) malloc(sizeof(Note));
                if (ptr == NULL) {
                    fclose(fp);
                    return NULL;
                }

                ptr->id = atoi(id);
                strncpy(ptr->title, title, sizeof(ptr->title) - 1);
                ptr->title[sizeof(ptr->title) - 1] = '\0';

                strncpy(ptr->text, content, sizeof(ptr->text) - 1);
                ptr->text[sizeof(ptr->text) - 1] = '\0';

                strncpy(ptr->complete_date, complete_date, sizeof(ptr->complete_date) - 1);
                ptr->complete_date[sizeof(ptr->complete_date) - 1] = '\0';

                ptr->completed = (atoi(completed) != 0);

                fclose(fp);
                return ptr; // Úspešne vrátime nájdenú poznámku
            }
        }
    }

    fclose(fp);
    return NULL; // Poznámka s daným ID sa v súbore nenašla
}
// https://en.wikipedia.org/wiki/Zeller%27s_congruence
int firstDayOfWeek(int month, int year) {
    int q = 1; // q

    // Leden a unor jsou pocitane jako 13 a 14 predchoziho roku
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }
    int K = year % 100;
    int J = year / 100;
    int n = 7;

    int h = ((q + (13*(month+1)/5) + K + (K/4) + (J/4) -2*J)) % n;

    // pokud modulo vyjde zaporne tak prictu 7 a preklopim do pozitivnich cisel
    if (h < 0) h += n;

    // set first day to Monday ;when monday h=1
    h = ((h + 5) % 7);

    return h;
}
bool isLeapYear(int year) {
    return (year % 4 == 0) && (year % 100 != 0) || year % 400 == 0;
}
int getDaysInMonth(int month, int year) {
    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2) {
        if (isLeapYear(year)) daysInMonth[1] = 29;
    }
    return daysInMonth[month - 1];
}

int getNextID(){
    int out_count=0;
    Note* notes=readCSV(&out_count);

    if (notes==NULL || out_count==0){
        if (notes!=NULL){
            free(notes);
        }
        return 1;
    }

    int nextID=notes[out_count-1].id+1;

    free(notes);
    return nextID;
}

int isCorrectDate(char text[]){
    int r;
    int m;
    int d;

    if (sscanf(text,"%d-%d-%d",&r,&m,&d) != 3){
        return 0;
    }

    if (r<1000 || r>9999){
        return 0;
    }

    if (m<1 || m>12){
        return 0;
    }

    int maxDni=getDaysInMonth(m,r);

    if (d<1 || d>maxDni){
        return 0;
    }
    return 1;
}