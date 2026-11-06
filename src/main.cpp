#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define MAX_TITLE 50
#define MAX_TEXT 300
#define CSV_PATH "../data/notebook.csv"

// l - vytvoření datové struktury pro poznámku
typedef struct {
    int id;
    char title[MAX_TITLE];
    char text[MAX_TEXT];
    bool completed; // 0 - uncompleted, 1 = completed
    char complate_date[11]; // YYYY-MM-DD
} Note;

void defaultMenu(char *input);
Note *readCSV();
int handleList();
void cls();
void printNoteOptions();
int chooseNoteOptions(char choice, int id);
Note *getNote(int pid);
void showCalendar();
int firstDayOfWeek(int month, int year);

// Hlavní runtime aplikace
int main() {
    char input = ' ';
    while (true) {
        defaultMenu(&input);
        switch (input) {
            case 'a':
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
        char *complate_date = strtok(NULL, "|");

        // ujisteni ze v kazdem poli neco je
        if (id && title && content && completed && complate_date) {

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

            strncpy(ptr[count].complate_date, complate_date, sizeof(ptr[count].complate_date) - 1);
            ptr[count].complate_date[sizeof(ptr[count].complate_date) - 1] = '\0';

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
            printf("| %-5d | %-20s | %-10s | %-1i\n", ptr[i].id, ptr[i].title, ptr[i].complate_date, ptr[i].completed);
        }
        free(ptr);

        char input[5];
        int choice;
        char inputN = ' ';
        printf("\n");

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
        }while (chooseNoteOptions == 0);
    }
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
    printf("Delete[r]\n");
    printf("Back[b]\n");
    printf("Exit[e]\n\n");
}
// l - handler pro akce se souborem
int chooseNoteOptions(char choice, int id) {
    switch (choice) {
        case 'r': {
            printf("Read");
            Note *note = getNote(id);
            printf("%d", note->id);
            break;
        }
        case 'u':
            printf("Update");
            break;
        case 'd':
            printf("Delete");
            break;
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
// l - vraci hodnotu jedne poznamky vybranou pomoci id(pid)
Note *getNote(int pid) {
    // vytvorim pointer na strukturu
    Note *ptr = (Note *) malloc(sizeof(Note));
    if (ptr == NULL) {
        printf("Not enough memory\n");
        exit(1);
    }
    FILE *fp = fopen(CSV_PATH, "r");

    // If the file does not exist, create it with a header line and return NULL
    if (fp == NULL) {
        printf("Can't open file.\n");
        exit(1);
    }
    char buffer[1024];
    fgets(buffer, sizeof(buffer), fp);
    while (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        char *id = strtok(buffer, "|");

        // pokud poznamka id neni stejna jako parametr tak preskoci iteraci na dalsi radek
        if (atoi(id) != pid) continue;
        // ziskal jsem poznamku a ted jsi ulozim do docasnych promenych
        char *title = strtok(NULL, "|");
        char *content = strtok(NULL, "|");
        char *completed = strtok(NULL, "|");
        char *compleate_date = strtok(NULL, "|");

        // ulozim z dosasnych promenych do struktury a pointer vratim
        ptr->id = atoi(id);

        strncpy(ptr->title, title, sizeof(ptr->title) - 1);
        ptr->title[sizeof(ptr->title) - 1] = '\0';

        strncpy(ptr->complate_date, compleate_date, sizeof(ptr->complate_date) - 1);
        ptr->complate_date[sizeof(ptr->complate_date) - 1] = '\0';

        strncpy(ptr->text, content, sizeof(ptr->text));
        ptr->text[sizeof(ptr->text) - 1] = '\0';

        ptr->completed = (atoi(completed) != 0);
        return ptr;
    }
    return nullptr;
}
void showCalendar() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int firstDay = firstDayOfWeek(t->tm_mon + 1, t->tm_year + 1900);
    printf("First day: %d", firstDay);
}
int firstDayOfWeek(int month, int year) {
    int q = 1; // q

    int K = year % 100;
    int J = year / 100;
    int n = 7;

    // Leden a unor jsou pocitane jako 13 a 14 predchoziho roku
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }
    int h = ((q + (13*(month+1)/5) + K + (K/4) + (J/4) -2*J)) % n;

    // pokud modulo vyjde zaporne tak prictu 7 a preklopim do pozitivnich cisel
    if (h < 0) h += n;

    // set first day to Monday ;when monday h=1
    h = ((h + 5) % 7) + 1;

    return h;
}
