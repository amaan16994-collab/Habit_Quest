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
int habitCount = 0, diaryCount = 0, nextHabitId = 1, nextDiaryId = 1;

int readInt(int min, int max), readString(char *buf, int size);

void addHabit(void), viewHabits(void), editHabit(void), removeHabit(void),
     markHabitComplete(void), showProgress(void), resetDailyStatus(void),
     addDiaryEntry(void), viewDiary(void), editDiaryEntry(void),
     deleteDiaryEntry(void), saveHabits(void), loadHabits(void),
     saveDiary(void), loadDiary(void), mainMenu(void), habitMenu(void),
     diaryMenu(void);

int readInt(int min, int max) {
    int val, ok;
    char buf[LINE_LEN], extra;

    while (1) {
        printf("enter number (%d-%d): ", min, max);
        if (!fgets(buf, sizeof(buf), stdin)) return min;
        ok = sscanf(buf, " %d %c", &val, &extra);
        if (ok == 1 && val >= min && val <= max) return val;
        printf("bad input, try again\n");
    }
}

int readString(char *buf, int size) {
    size_t len;

    while (1) {
        if (!fgets(buf, size, stdin)) {
            buf[0] = '\0';
            return 0;
        }
        len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (buf[0] != '\0') return 1;
        printf("cannot be empty, try again: ");
    }
}

void addHabit(void) {
    Habit h;

    if (habitCount >= MAX_HABITS) {
        printf("habit list full\n");
        return;
    }
    printf("habit name: ");
    readString(h.name, sizeof(h.name));
    h.id = nextHabitId++;
    h.completedToday = 0;
    habits[habitCount++] = h;
    saveHabits();
}

void viewHabits(void) {
    int i;

    if (habitCount == 0) {
        printf("no habits yet\n");
        return;
    }
    for (i = 0; i < habitCount; i++) {
        printf("%d | %s | %s\n", habits[i].id, habits[i].name,
               habits[i].completedToday ? "done" : "not done");
    }
}

void editHabit(void) {
    int id, idx, i;

    if (habitCount == 0) {
        printf("no habits yet\n");
        return;
    }
    viewHabits();
    printf("id to edit: ");
    id = readInt(1, nextHabitId);
    idx = -1;
    for (i = 0; i < habitCount; i++) {
        if (habits[i].id == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("not found\n");
        return;
    }
    printf("new name: ");
    readString(habits[idx].name, sizeof(habits[idx].name));
    saveHabits();
}

void removeHabit(void) {
    int id, idx, sure, i;

    if (habitCount == 0) {
        printf("no habits yet\n");
        return;
    }
    viewHabits();
    printf("id to remove: ");
    id = readInt(1, nextHabitId);
    idx = -1;
    for (i = 0; i < habitCount; i++) {
        if (habits[i].id == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("not found\n");
        return;
    }
    printf("are you sure? (1=yes, 0=no): ");
    sure = readInt(0, 1);
    if (!sure) return;

    // shift items left to fill the gap
    for (i = idx; i < habitCount - 1; i++) {
        habits[i] = habits[i + 1];
    }
    habitCount--;
    saveHabits();
}

void markHabitComplete(void) {
    int id, idx, i;

    if (habitCount == 0) {
        printf("no habits yet\n");
        return;
    }
    viewHabits();
    printf("id to mark done: ");
    id = readInt(1, nextHabitId);
    idx = -1;
    for (i = 0; i < habitCount; i++) {
        if (habits[i].id == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("not found\n");
        return;
    }
    if (habits[idx].completedToday) {
        printf("already done today\n");
        return;
    }
    habits[idx].completedToday = 1;
    saveHabits();
}

void showProgress(void) {
    int done, i;
    float pct;

    if (habitCount == 0) {
        printf("no habits yet\n");
        return;
    }
    done = 0;
    for (i = 0; i < habitCount; i++) {
        if (habits[i].completedToday == 1) done++;
    }
    pct = ((float)done / (float)habitCount) * 100.0f;
    printf("%d/%d done (%.1f%%)\n", done, habitCount, pct);
}

void resetDailyStatus(void) {
    int i;

    for (i = 0; i < habitCount; i++) {
        habits[i].completedToday = 0;
    }
    saveHabits();
    printf("new day started\n");
}

void addDiaryEntry(void) {
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

void viewDiary(void) {
    int i;

    if (diaryCount == 0) {
        printf("no entries yet\n");
        return;
    }
    for (i = 0; i < diaryCount; i++) {
        printf("%d | %s | %s\n", diary[i].id, diary[i].date, diary[i].text);
    }
}

void editDiaryEntry(void) {
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
        // swap pipe so file format stays clean
        for (j = 0; temp[j] != '\0'; j++) {
            if (temp[j] == '|') temp[j] = '/';
        }
        fprintf(fp, "%d|%s|%d\n", habits[i].id, temp, habits[i].completedToday);
    }
    fclose(fp);
}

void loadHabits(void) {
    FILE *fp;
    char line[LINE_LEN], name[NAME_LEN], *p1, *p2;
    int id, done;
    size_t len;

    habitCount = 0;
    nextHabitId = 1;
    fp = fopen("habits.txt", "r");
    if (!fp) return;

    while (fgets(line, sizeof(line), fp) && habitCount < MAX_HABITS) {
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        p1 = strchr(line, '|');
        if (!p1) continue;
        *p1 = '\0';
        p2 = strchr(p1 + 1, '|');
        if (!p2) continue;
        *p2 = '\0';

        if (sscanf(line, "%d", &id) != 1 || sscanf(p2 + 1, "%d", &done) != 1) continue;
        if (strlen(p1 + 1) >= sizeof(name) || (done != 0 && done != 1)) continue;

        strcpy(name, p1 + 1);
        habits[habitCount].id = id;
        strcpy(habits[habitCount].name, name);
        habits[habitCount].completedToday = done;
        if (id >= nextHabitId) nextHabitId = id + 1;
        habitCount++;
    }
    fclose(fp);
}

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
            if (temp[j] == '|') temp[j] = '/';
        }
        fprintf(fp, "%d|%s|%s\n", diary[i].id, diary[i].date, temp);
    }
    fclose(fp);
}

void loadDiary(void) {
    FILE *fp;
    char line[LINE_LEN], date[DATE_LEN], text[TEXT_LEN], *p1, *p2;
    int id;
    size_t len;

    diaryCount = 0;
    nextDiaryId = 1;
    fp = fopen("diary.txt", "r");
    if (!fp) return;

    while (fgets(line, sizeof(line), fp) && diaryCount < MAX_DIARY) {
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        p1 = strchr(line, '|');
        if (!p1) continue;
        *p1 = '\0';
        p2 = strchr(p1 + 1, '|');
        if (!p2) continue;
        *p2 = '\0';

        if (sscanf(line, "%d", &id) != 1) continue;
        if (strlen(p1 + 1) >= sizeof(date) || strlen(p2 + 1) >= sizeof(text)) continue;

        strcpy(date, p1 + 1);
        strcpy(text, p2 + 1);
        diary[diaryCount].id = id;
        strcpy(diary[diaryCount].date, date);
        strcpy(diary[diaryCount].text, text);
        if (id >= nextDiaryId) nextDiaryId = id + 1;
        diaryCount++;
    }
    fclose(fp);
}

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

void mainMenu(void) {
    int choice;

    while (1) {
        printf("\n1. habits\n2. diary\n3. exit\n");
        choice = readInt(1, 3);
        switch (choice) {
            case 1: habitMenu(); break;
            case 2: diaryMenu(); break;
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