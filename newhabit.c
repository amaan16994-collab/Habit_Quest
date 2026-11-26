#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_HABITS 50
#define MAX_DIARY 100
#define NAME_LEN 100
#define DATE_LEN 20
#define TEXT_LEN 256
#define LINE_LEN 512

typedef struct {
    int id;
    int completedToday;
    char name[NAME_LEN];
} Habit;

typedef struct {
    int id;
    char date[DATE_LEN];
    char text[TEXT_LEN];
} DiaryEntry;

Habit habits[MAX_HABITS];
DiaryEntry diary[MAX_DIARY];

int habitCount = 0, diaryCount = 0;
int nextHabitId = 1, nextDiaryId = 1;

int readInt(int min, int max);
int readString(char *buf, int size);

void addHabit(void);
void viewHabits(void);
void editHabit(void);
void removeHabit(void);
void markHabitComplete(void);
void showProgress(void);
void resetDailyStatus(void);

void addDiaryEntry(void);
void viewDiary(void);
void editDiaryEntry(void);
void deleteDiaryEntry(void);

void saveHabits(void);
void loadHabits(void);
void saveDiary(void);
void loadDiary(void);

void mainMenu(void);
void habitMenu(void);
void diaryMenu(void);


// INPUT FUNCTIONS by Amaan Masood 

int readInt(int min, int max){
    int val, ok;
    char buf[LINE_LEN], extra;

    while (1){
        printf("enter number (%d-%d): ", min, max);

        if(!fgets(buf, sizeof(buf), stdin))
            return min;

        ok = sscanf(buf, " %d %c", &val, &extra);

        if(ok == 1 && val >= min && val <= max)
            return val;

        printf("bad input, try again\n");
    }
}

int readString(char *buf, int size){
    size_t len;

    while(1){
        if(!fgets(buf, size, stdin)){
            buf[0] = '\0';
            return 0;
        }

        len = strlen(buf);

        if(len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        if(buf[0] != '\0')
            return 1;

        printf("cannot be empty, try again: ");
    }
}


int selectHabit(void){
    int id;
    int index = -1;
    if (habitCount == 0){
        printf("No habits yet!\n");
        return -1;
    }
    viewHabits();
    printf("Enter habit ID: ");
    id = readInt(1, nextHabitId - 1);
    for (int i = 0; i < habitCount; i++){
        if(habits[i].id == id){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Habit not found!\n");
    }
    return index;
}

// HABIT FUNCTIONS by MR
void addHabit(){
    if (habitCount == 50)
    {
        printf("Habits list is full!\n");
        return;
    }
    char name[50];
    printf("Enter habit name: ");
    readString(name, 50);
    if (name[0] == '\0'){
        printf("Enter a habit!\n");
        return;
    }
    strcpy(habits[habitCount].name, name);
    habits[habitCount].id = nextHabitId;
    habits[habitCount].completedToday = 0;
    habitCount++;
    nextHabitId++;
    saveHabits();
}

void viewHabits(){
    if (habitCount == 0){
        printf("No habits yet!\n");
        return;
    }
    for (int i = 0; i < habitCount; i++){
        printf("%d | %s | %s\n",
               habits[i].id,
               habits[i].name,
               habits[i].completedToday == 0 ? "Not completed" : "Completed");
    }
}

void editHabit(){
    int index = selectHabit();
    if (index == -1)
    {
        return;
    }
    char newName[50];
    printf("Enter the new habit name: ");
    readString(newName, 50);
    if (newName[0] == '\0'){
        printf("Habit name cannot be empty!\n");
        return;
    }
    strcpy(habits[index].name, newName);
    saveHabits();
    printf("Habit updated successfully!\n");
}

void removeHabit(){
    int index = selectHabit();
    if (index == -1)
    return;
    for (int i = index; i < habitCount - 1; i++){
        habits[i] = habits[i + 1];
    }
    habitCount--;
    for (int i = 0; i < habitCount; i++){
        habits[i].id = i + 1;
    }
    nextHabitId = habitCount + 1;
    saveHabits();
    printf("Habit removed successfully!\n");
}

void markHabitComplete(){
    int index = selectHabit();
    if (index == -1)
    {
        return;
    }
    if (habits[index].completedToday == 1)
    {
        printf("This habit is already completed!\n");
        return;
    }
    habits[index].completedToday = 1;
    saveHabits();
    printf("Habit marked as completed!\n");
}

void showProgress(){
    if (habitCount == 0)
    {
        printf("No habits available to track progress!\n");
        return;
    }
    int completed = 0;
    for (int i = 0; i < habitCount; i++){
        if (habits[i].completedToday == 1)
        {
            completed++;
        }
    }
    float percentage = (completed * 100.0) / habitCount;

    printf("\n----- Daily Progress -----\n");
    printf("Completed habits: %d\n", completed);
    printf("Total habits: %d\n", habitCount);
    printf("Progress: %.2f%%\n", percentage);
}

void resetDailyStatus(){
    if (habitCount == 0)
    {
        printf("No habits to reset!\n");
        return;
    }
    for (int i = 0; i < habitCount; i++){
        habits[i].completedToday = 0;
    }
    saveHabits();
    printf("Daily habit status has been reset!\n");
}

// DIARY FUNCTIONS by AM
void addDiaryEntry(void){
    DiaryEntry e;
    time_t now;
    struct tm *t;

    if (diaryCount >= MAX_DIARY) {
        printf("diary full\n");
        return;
    }

    now = time(NULL);
    t = localtime(&now);

    if (t) {
        strftime(e.date, sizeof(e.date), "%Y-%m-%d", t);
    } else {
        strcpy(e.date, "unknown");
    }

    printf("reflection: ");
    readString(e.text, sizeof(e.text));

    e.id = nextDiaryId++;
    diary[diaryCount++] = e;

    saveDiary();
}


void viewDiary(void){
    int i;

    if (diaryCount == 0) {
        printf("no entries yet\n");
        return;
    }

    for (i = 0; i < diaryCount; i++) {
        printf("%d | %s | %s\n",
               diary[i].id,
               diary[i].date,
               diary[i].text);
    }
}


void editDiaryEntry(void){
    int id, idx, i;

    if (diaryCount == 0) {
        printf("no entries yet\n");
        return;
    }

    viewDiary();

    printf("id to edit: ");
    id = readInt(1, nextDiaryId);

    idx = -1;

    for (i = 0; i < diaryCount; i++) {
        if (diary[i].id == id) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("not found\n");
        return;
    }

    printf("new text: ");
    readString(diary[idx].text, sizeof(diary[idx].text));

    saveDiary();
}


void deleteDiaryEntry(void) {
    int id, idx, i;

    if (diaryCount == 0) {
        printf("no entries yet\n");
        return;
    }

    viewDiary();

    printf("id to delete: ");
    id = readInt(1, nextDiaryId);

    idx = -1;

    for (i = 0; i < diaryCount; i++) {
        if (diary[i].id == id) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("not found\n");
        return;
    }

    for (i = idx; i < diaryCount - 1; i++) {
        diary[i] = diary[i + 1];
    }

    diaryCount--;

    saveDiary();
}


// SAVE HABITS FUNCTION

void saveHabits(void) {
    FILE *fp;
    int i;
    size_t j;
    char temp[NAME_LEN];

    fp = fopen("habits.txt", "w");

    if (!fp) {
        printf("cannot open habits.txt\n");
        return;
    }

    for (i = 0; i < habitCount; i++) {
        strcpy(temp, habits[i].name);

        for (j = 0; temp[j] != '\0'; j++) {
            if (temp[j] == '|')
                temp[j] = '/';
        }

        fprintf(fp, "%d|%s|%d\n",
                habits[i].id,
                temp,
                habits[i].completedToday);
    }

    fclose(fp);
}


// LOAD HABITS FUNCTION

void loadHabits(void) {
    FILE *fp;

    char line[LINE_LEN], name[NAME_LEN], *p1, *p2;

    int id, done;
    size_t len;

    habitCount = 0;
    nextHabitId = 1;

    fp = fopen("habits.txt", "r");

    if (!fp)
        return;

    while (fgets(line, sizeof(line), fp) &&
           habitCount < MAX_HABITS) {

        len = strlen(line);

        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        p1 = strchr(line, '|');

        if (!p1)
            continue;

        *p1 = '\0';

        p2 = strchr(p1 + 1, '|');

        if (!p2)
            continue;

        *p2 = '\0';

        if (sscanf(line, "%d", &id) != 1 ||
            sscanf(p2 + 1, "%d", &done) != 1)
            continue;

        if (strlen(p1 + 1) >= sizeof(name) ||
            (done != 0 && done != 1))
            continue;

        strcpy(name, p1 + 1);

        habits[habitCount].id = id;
        strcpy(habits[habitCount].name, name);
        habits[habitCount].completedToday = done;

        if (id >= nextHabitId)
            nextHabitId = id + 1;

        habitCount++;
    }

    fclose(fp);
}


// SAVE DIARY FUNCTION

void saveDiary(void) {
    FILE *fp;
    int i;
    size_t j;
    char temp[TEXT_LEN];

    fp = fopen("diary.txt", "w");

    if (!fp) {
        printf("cannot open diary.txt\n");
        return;
    }

    for (i = 0; i < diaryCount; i++) {
        strcpy(temp, diary[i].text);

        for (j = 0; temp[j] != '\0'; j++) {
            if (temp[j] == '|')
                temp[j] = '/';
        }

        fprintf(fp, "%d|%s|%s\n",
                diary[i].id,
                diary[i].date,
                temp);
    }

    fclose(fp);
}


// LOAD DIARY FUNCTION

void loadDiary(void) {
    FILE *fp;

    char line[LINE_LEN], date[DATE_LEN],
         text[TEXT_LEN], *p1, *p2;

    int id;
    size_t len;

    diaryCount = 0;
    nextDiaryId = 1;

    fp = fopen("diary.txt", "r");

    if (!fp)
        return;

    while (fgets(line, sizeof(line), fp) &&
           diaryCount < MAX_DIARY) {

        len = strlen(line);

        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        p1 = strchr(line, '|');

        if (!p1)
            continue;

        *p1 = '\0';

        p2 = strchr(p1 + 1, '|');

        if (!p2)
            continue;

        *p2 = '\0';

        if (sscanf(line, "%d", &id) != 1)
            continue;

        if (strlen(p1 + 1) >= sizeof(date) ||
            strlen(p2 + 1) >= sizeof(text))
            continue;

        strcpy(date, p1 + 1);
        strcpy(text, p2 + 1);

        diary[diaryCount].id = id;
        strcpy(diary[diaryCount].date, date);
        strcpy(diary[diaryCount].text, text);

        if (id >= nextDiaryId)
            nextDiaryId = id + 1;

        diaryCount++;
    }

    fclose(fp);
}


// HABIT MENU

void habitMenu(void) {
    int choice;

    while (1) {
        printf("\n1. add habit\n2. view habits\n3. edit habit\n4. remove habit\n");
        printf("5. mark complete\n6. show progress\n7. start new day\n8. back\n");

        choice = readInt(1, 8);

        switch (choice) {
            case 1: addHabit(); break;
            case 2: viewHabits(); break;
            case 3: editHabit(); break;
            case 4: removeHabit(); break;
            case 5: markHabitComplete(); break;
            case 6: showProgress(); break;
            case 7: resetDailyStatus(); break;
            case 8: return;
        }
    }
}


// DIARY MENU

void diaryMenu(void) {
    int choice;

    while (1) {
        printf("\n1. add entry\n2. view entries\n3. edit entry\n4. delete entry\n5. back\n");

        choice = readInt(1, 5);

        switch (choice) {
            case 1: addDiaryEntry(); break;
            case 2: viewDiary(); break;
            case 3: editDiaryEntry(); break;
            case 4: deleteDiaryEntry(); break;
            case 5: return;
        }
    }
}


// MAIN MENU
void mainMenu(void){
    int choice;
    while (1) {
        printf("\n1. habits\n2. diary\n3. exit\n");
        choice = readInt(1, 3);
        switch (choice) {
            case 1:
                habitMenu();
                break;

            case 2:
                diaryMenu();
                break;

            case 3:
                saveHabits();
                saveDiary();
                return;
        }
    }
}

int main(void) {
    loadHabits();
    loadDiary();

    mainMenu();

    return 0;
}