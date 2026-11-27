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

        case 'u': {
            updateNote(id);
            int c;
            while ((c=getchar()) != '\n' && c!=EOF);
            printf("\n Press Enter to return!");
            getchar();
            return 2;
        }

        case 'd': {
            deleteNote(id);
            int c;
            while ((c=getchar()) != '\n' && c!=EOF);
            printf("\n Press Enter to return!");
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

void showCalendar() {
    cls();
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int month = t->tm_mon + 1;
    int year = t->tm_year + 1900;
    int firstDay = firstDayOfWeek(month, year);
    int daysInMonth = getDaysInMonth(month,  year);
    int dayPos = firstDay;

    // Centrovani hlavicky
    char headerBuffer[30];
    int textLength = sprintf(headerBuffer, "%d / %d", month, year);
    int padding = (27 - textLength) / 2;
    printf("\n%*s%s\n", padding, "", headerBuffer);

    printf("Mon Tue Wed Thu Fri Sat Sun\n");

    int out_count = 0;
    Note *notes = readCSV(&out_count);

    // Vytisknout padding
    for (int i=0; i < firstDay; i++) {
        printf("    ");
    }
    for (int i=1; i <= daysInMonth; i++) {
        char currentDate[11];
        sprintf(currentDate, "%04d-%02d-%02d", year, month, i);
        int hasNote = 0;
        for (int j=0; j < out_count; j++) {
            if (strcmp(notes[j].complete_date, currentDate) == 0) {
                hasNote = 1;
                break;
            }
        }
        if (hasNote) {
            printf("%3d*", i);
        }else {
            printf("%3d ", i);
        }
        dayPos++;
        if (dayPos % 7 == 0) printf("\n");
    }
    printf("\n\n");

    char currentMonthPrefix[8];
    sprintf(currentMonthPrefix, "%04d-%02d", year, month);

    printf("---------- Notes ----------\n");
    printf(" id    Date      Title\n");
    for (int i=0; i < out_count; i++) {
        if (strncmp(notes[i].complete_date, currentMonthPrefix, 7) == 0) {
            printf("[%d] %s - %s\n", notes[i].id, notes[i].complete_date, notes[i].title);
        }
    }
    // uvolnit dynamickou pamet
    free(notes);

    // vycistit buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // pockat na interakci uzivatele
    printf("\nPress enter to go back...");
    getchar();

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

//aaby sme predisli chybam, user takto nemoze zadat duplicitne ID, vzdy sa automataicky vygeneruje nove na zaklade predoslej poznamky
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

    //zlyha ak sa nepodari nacitat 3 cisla 
    if (sscanf(text,"%d-%d-%d",&r,&m,&d) != 3){
        return 0;
    }
    
    if (r<1000 || r>9999){
        return 0;
    }

    if (m<1 || m>12){
        return 0;
    }

    //ochrana aj pred leap year
    int maxDni=getDaysInMonth(m,r);

    if (d<1 || d>maxDni){
        return 0;
    }
    return 1;
}

void addNote(){
    int id=getNextID();
    char title [100];
    char text [300];
    char date [20];

    printf("\n ---NEW NOTE---\n");
    printf("ID: %d\n",id);

    printf("Enter note title: ");
    scanf(" %[^\n]s", title);

    printf("Enter your note: ");
    scanf(" %[^\n]s", text);

    while (1){
        printf("Enter date: (YYYY-MM-DD): ");
        scanf("%s",date);

        if (isCorrectDate(date)==1){
            break;
        }

        printf("Incorrect date format! Enter your date as YYYY-MM-DD (Example: 2026-11-2O).");
    }

    FILE *fp = fopen(CSV_PATH, "a");

    if (fp != NULL) { 
        fprintf(fp, "%d|%s|%s|0|%s\n", id, title, text, date); 
        fclose(fp); 
        printf("The note has been successfuly saved!\n");
    } 
    else { 
        printf("Error: File could not be opened!\n");
    } 
}

void updateNote(int id) { 
    Note *n = getNote(id); 
    if (n == NULL) { 
        printf("\nNote [ID: %d] does not exist!\n", id);
        return; 
    } 

    //stlacenim enteru preskocime policko co nechceme menit
    printf("\n--- EDIT NOTE (ID: %d) ---\n", id);
    printf("(Press Enter to skip)\n\n");

    //musime vycistit buffer, inak sa po stalceni enteru rovno prejde na zmenu textu
    int c;
    while ((c=getchar()) != '\n' && c!=EOF);

    char new_title[100];
    char new_text[500];
    char new_date[20];

    strcpy(new_title,n->title);
    strcpy(new_text,n->text);
    strcpy(new_date,n->complete_date);

    printf("Title [%s]: ",n->title);
    char input_title[100];

    if (fgets(input_title,sizeof(input_title),stdin)){
        if (input_title[0]!='\n'){
            input_title[strcspn(input_title,"\r\n")]='\0';
            strcpy(new_title,input_title);
        }
    }
    
    printf("Text [%s]: ",n->text);
    char input_text[500];

    if (fgets(input_text,sizeof(input_text),stdin)){
        if (input_text[0]!='\n'){
            input_text[strcspn(input_text,"\r\n")]='\0';
            strcpy(new_text,input_text);
        }
    }

    //loop bezi pokial uzivael nezada spravny datum
    while (1){
        printf("Date (YYYY-MM-DD) [%s]: ", n->complete_date);
        char input_date[50];

        if (fgets(input_date,sizeof(input_date),stdin)){
        if (input_date[0]=='\n'){
            break;
        }
        input_date[strcspn(input_date,"\r\n")]='\0';
        if (isCorrectDate(input_date)==1){
            strcpy(new_date,input_date);
            break;
        }
        printf("Incorrect date format! Enter your date as YYYY-MM-DD (Example: 2026-11-2O).\n");
        } 
    }

    printf("Complete (1)/Incomplete(0) [%d]: ",n->completed);
    char input_completed[10];
    int new_completed=n->completed;

    if (fgets(input_completed,sizeof(input_completed),stdin)){
        if (input_completed[0]!='\n'){
            new_completed=atoi(input_completed);
        }
    }

    strcpy(n->title,new_title);
    strcpy(n->text,new_text);
    strcpy(n->complete_date,new_date);
    n->completed=new_completed;
    
    //praca s temp suborom
    FILE *fp = fopen(CSV_PATH, "r"); 
    FILE *temp = fopen("../data/temp.csv", "w"); 

    if (fp == NULL || temp == NULL) { 
        printf("Error: The file could not be opened!\n");
        if (fp) fclose(fp); 
        if (temp) fclose(temp); 
        free(n); 
        return; 
    } 
    
    //ak narazime na id poznamky, ktoru chceme editovat, zmeny zapiseme do temp, zvysne prekopirujeme bez zmeny
    char line[1024]; 
    while (fgets(line, sizeof(line), fp)) { 
        int current_id; 
        if (sscanf(line, "%d|", &current_id) == 1 && current_id == id) { 
            fprintf(temp, "%d|%s|%s|%d|%s\n", n->id, n->title, n->text, n->completed, n->complete_date); 
        } 
        else { 
            fputs(line, temp); 
        } 
    } 

    fclose(fp); 
    fclose(temp); 

    remove(CSV_PATH); 
    rename("../data/temp.csv",CSV_PATH);

    printf("\n--------------------------\n");
    printf("\nThe note [ID:%d] has been updated!\n",id);
    printf("Press Enter to go back\n");
    
    //musime zase vycistit buffer, inak sa userovi nevypise "Press Enter to go back" a nevedel by co robit
    int d;
    while ((d=getchar()) != '\n' && d != EOF);
    free(n);
}

void deleteNote(int id){
    //potvrdenie uzivatelom ci naozaj chce poznamku mazat
    char confirm=' ';
    printf("Do you really want to delete this note? (Press y to confirm, any key to cancel): ");
    scanf(" %c",&confirm);

    if (confirm != 'y' && confirm != 'Y'){
        printf("Action cancelled.\n");
        return;
    }

    //praca s twmp suborom
    FILE *fp=fopen(CSV_PATH,"r");
    FILE *temp=fopen("../data/temp.csv","w");

    //ak nevieme nacitat oba, zatvorime ten ktory sa nacitst podarilo
    if (fp==NULL || temp==NULL){
        printf("Error: The file could not be opened!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char line[1024];
    int found=0;

    //ak anrazime na id poznakmy ktoru chceme vymazat, nezapiseme ju do temp suboru
    while (fgets(line,sizeof(line),fp)){
        int current_id;
        if (sscanf(line,"%d|",&current_id)==1){
            if (current_id==id){
                found=1;
                continue;
            }
        }
        fputs(line,temp);
    }
    fclose(fp);
    fclose(temp);

    //mazeme povodny file a premenujeme novy temp
    remove(CSV_PATH);
    rename("../data/temp.csv",CSV_PATH);

    if (found){
        printf("\nNote [ID: %d] has been deleted!\n",id);
    }

    else{
        printf("\nNote [ID: %d] has not been found!",id);
    }

}