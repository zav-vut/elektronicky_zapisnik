#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "utils.h"

int chooseNoteOptions(char choice, int id);
void showCalendar();
void addNote();
void updateNote(int id);
void deleteNote(int id);

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
        printf("\n");

        // cekani na vybrani poznamky
        while (fgets(input, sizeof(input), stdin)) {
            // pokud uzivatel zada b pro back tak se vrati do nabidky default menu
            if (input[0] == 'b' && (input[1] == '\n' || input[1] == '\0')) {
                return 0;
            }
            if (sscanf(input, "%d", &choice) == 1) {
                Note* check = getNote(choice);
                if (check != nullptr) {
                    free(check);
                    break;
                }else {
                    if (strchr(input, '\n') == NULL) {
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF); // vyprázdní zbytek řádku
                    }

                    printf("Select note[id]/back[b]: ");
                }
            }else {
                if (strchr(input, '\n') == NULL) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF); // vyprázdní zbytek řádku
                }

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

            // Vyčistenie vstupného buffera a počkanie na Enter
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\nPress enter to go back...");
            getchar();
            return 2; // Návrat späť do zoznamu poznámok
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

        printf("Incorrect date format! Year must be 4 digits long, month 2 digits long and date 2 digits long. (Example: 2026-11-2O).");
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
    // 1. Skontrolujeme, či poznámka existuje 
    Note *n = getNote(id); 
    if (n == NULL) { 
        printf("\nNote with ID: %d does not exist!\n", id);
        return; 
    } 
    printf("\n--- EDIT NOTE (ID: %d) ---\n", id);

    printf("Original title: %s\n", n->title);
    printf("Enter new title: ");
    scanf(" %[^\n]", n->title); 

    printf("Original text: %s\n", n->text);
    printf("Enter new text: ");
    scanf(" %[^\n]", n->text); 

    while (1){
        printf("Original date: (%s). Enter new date (YYYY-MM-DD): ", n->complete_date);
        scanf("%s",n->complete_date);

        if (isCorrectDate(n->complete_date)==1){
            break;
        }
        printf("Incorrect date format! Year must be 4 digits long, month 2 digits long and date 2 digits long. (Example: 2026-11-2O).");
    } 

    int temp_status=0;
    printf("Progress (1 = Completed, 0 = Incomplete). Enter new progress: ");
    scanf("%d",&temp_status); 
    n->completed=(temp_status !=0);
    
    // 2. Prepis v súbore pomocou temp.csv 
    FILE *fp = fopen(CSV_PATH, "r"); 
    FILE *temp = fopen("../data/temp.csv", "w"); 

    if (fp == NULL || temp == NULL) { 
        printf("Error: The file could not be opened!\n");
        if (fp) fclose(fp); 
        if (temp) fclose(temp); 
        free(n); 
        return; 
    } 
    
    char line[1024]; 
    while (fgets(line, sizeof(line), fp)) { 
        int current_id; 
        if (sscanf(line, "%d|", &current_id) == 1 && current_id == id) { // Ak je to upravovaná poznámka, zapíšeme nové hodnoty 
            fprintf(temp, "%d|%s|%s|%d|%s\n", n->id, n->title, n->text, n->completed, n->complete_date); 
        } 
        else { // Ostatné riadky skopírujeme bezo zmeny 
            fputs(line, temp); 
        } 
    } 

    fclose(fp); 
    fclose(temp); // Nahradenie pôvodného súboru remove(CSV\_PATH); 

    remove(CSV_PATH); 
    rename("../data/temp.csv",CSV_PATH);

    printf("\nThe note has been updated!!\n");
    free(n);    
}

void deleteNote(int id){
    FILE *fp=fopen(CSV_PATH,"r");
    FILE *temp=fopen("../data/temp.csv","w");

    if (fp==NULL || temp==NULL){
        printf("Error: The file could not be opened!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char line[1024];
    int found=0;

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

    remove(CSV_PATH);
    rename("../data/temp.csv",CSV_PATH);

    if (found){
        printf("\nNote [ID: %d] has been deleted!\n",id);
    }

    else{
        printf("\nNote [ID: %d] has not been found!",id);
    }
}