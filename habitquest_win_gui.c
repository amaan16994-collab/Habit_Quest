#include <windows.h>
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

#define ID_TAB_HABITS 101
#define ID_TAB_DIARY 102
#define ID_INPUT 103
#define ID_BTN_ADD 104
#define ID_BTN_DONE 105
#define ID_BTN_EDIT 106
#define ID_BTN_DEL 107
#define ID_BTN_RESET 108
#define ID_LIST 109

typedef struct {
    int id, completedToday;
    char name[NAME_LEN];
} Habit;

typedef struct {
    int id;
    char date[DATE_LEN], text[TEXT_LEN];
} DiaryEntry;

Habit habits[MAX_HABITS];
DiaryEntry diary[MAX_DIARY];
int habitCount = 0, diaryCount = 0, nextHabitId = 1, nextDiaryId = 1;
int currentTab = 0, editIndex = -1;

HWND hTabHabits, hTabDiary, hLabel, hInput, hBtnAdd, hBtnDone, hBtnEdit, hBtnDel, hBtnReset, hList, hProgress;

void saveHabits(void), loadHabits(void), saveDiary(void), loadDiary(void),
     resetDailyStatus(void), refreshList(void), switchTab(int tab),
     handleAddSave(void), handleEditLoad(void), handleDelete(void), handleMarkDone(void);

void saveHabits(void) {
    FILE *fp;
    int i;
    size_t j;
    char temp[NAME_LEN];

    fp = fopen("habits.txt", "w");
    if (!fp) return;

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
    if (!fp) return;

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

void refreshList(void) {
    int i, done;
    float pct;
    char row[LINE_LEN], status[128];

    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    if (currentTab == 0) {
        done = 0;
        for (i = 0; i < habitCount; i++) {
            if (habits[i].completedToday == 1) done++;
            sprintf(row, "#%d  |  %s  |  %s", habits[i].id, habits[i].name,
                    habits[i].completedToday ? "done" : "not done");
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)row);
        }
        pct = habitCount > 0 ? ((float)done / (float)habitCount) * 100.0f : 0.0f;
        sprintf(status, "progress: %d / %d habits done (%.1f%%)", done, habitCount, pct);
        SetWindowText(hProgress, status);
    } else {
        for (i = 0; i < diaryCount; i++) {
            sprintf(row, "#%d  |  %s  |  %s", diary[i].id, diary[i].date, diary[i].text);
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)row);
        }
        sprintf(status, "total diary entries: %d", diaryCount);
        SetWindowText(hProgress, status);
    }
}

void switchTab(int tab) {
    currentTab = tab;
    editIndex = -1;
    SetWindowText(hInput, "");

    if (tab == 0) {
        SetWindowText(hLabel, "habit name:");
        SetWindowText(hBtnAdd, "add habit");
        ShowWindow(hBtnDone, SW_SHOW);
        ShowWindow(hBtnReset, SW_SHOW);
    } else {
        SetWindowText(hLabel, "reflection:");
        SetWindowText(hBtnAdd, "add entry");
        ShowWindow(hBtnDone, SW_HIDE);
        ShowWindow(hBtnReset, SW_HIDE);
    }
    refreshList();
}

void handleAddSave(void) {
    char buf[TEXT_LEN];
    time_t now;
    struct tm *t;

    GetWindowText(hInput, buf, sizeof(buf));
    if (buf[0] == '\0') return;

    if (currentTab == 0) {
        buf[NAME_LEN - 1] = '\0';
        if (editIndex >= 0 && editIndex < habitCount) {
            strcpy(habits[editIndex].name, buf);
            editIndex = -1;
            SetWindowText(hBtnAdd, "add habit");
        } else if (habitCount < MAX_HABITS) {
            habits[habitCount].id = nextHabitId++;
            habits[habitCount].completedToday = 0;
            strcpy(habits[habitCount].name, buf);
            habitCount++;
        }
        saveHabits();
    } else {
        if (editIndex >= 0 && editIndex < diaryCount) {
            strcpy(diary[editIndex].text, buf);
            editIndex = -1;
            SetWindowText(hBtnAdd, "add entry");
        } else if (diaryCount < MAX_DIARY) {
            now = time(NULL);
            t = localtime(&now);
            if (t) {
                strftime(diary[diaryCount].date, sizeof(diary[diaryCount].date), "%Y-%m-%d", t);
            } else {
                strcpy(diary[diaryCount].date, "unknown");
            }
            diary[diaryCount].id = nextDiaryId++;
            strcpy(diary[diaryCount].text, buf);
            diaryCount++;
        }
        saveDiary();
    }

    SetWindowText(hInput, "");
    refreshList();
}

void handleEditLoad(void) {
    int sel;

    sel = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR) return;

    editIndex = sel;
    if (currentTab == 0 && sel < habitCount) {
        SetWindowText(hInput, habits[sel].name);
        SetWindowText(hBtnAdd, "save edit");
    } else if (currentTab == 1 && sel < diaryCount) {
        SetWindowText(hInput, diary[sel].text);
        SetWindowText(hBtnAdd, "save edit");
    }
}

void handleDelete(void) {
    int sel, i;

    sel = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR) return;

    if (currentTab == 0 && sel < habitCount) {
        // shift habits back after removing item
        for (i = sel; i < habitCount - 1; i++) {
            habits[i] = habits[i + 1];
        }
        habitCount--;
        saveHabits();
    } else if (currentTab == 1 && sel < diaryCount) {
        for (i = sel; i < diaryCount - 1; i++) {
            diary[i] = diary[i + 1];
        }
        diaryCount--;
        saveDiary();
    }

    editIndex = -1;
    SetWindowText(hInput, "");
    SetWindowText(hBtnAdd, currentTab == 0 ? "add habit" : "add entry");
    refreshList();
}

void handleMarkDone(void) {
    int sel;

    if (currentTab != 0) return;
    sel = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR || sel >= habitCount) return;

    habits[sel].completedToday = 1;
    saveHabits();
    refreshList();
}

void resetDailyStatus(void) {
    int i;

    for (i = 0; i < habitCount; i++) {
        habits[i].completedToday = 0;
    }
    saveHabits();
    refreshList();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    HFONT font;

    switch (msg) {
        case WM_CREATE:
            font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

            hTabHabits = CreateWindow("BUTTON", "habits", WS_VISIBLE | WS_CHILD,
                                      20, 15, 110, 30, hwnd, (HMENU)ID_TAB_HABITS, NULL, NULL);
            hTabDiary = CreateWindow("BUTTON", "diary", WS_VISIBLE | WS_CHILD,
                                     140, 15, 110, 30, hwnd, (HMENU)ID_TAB_DIARY, NULL, NULL);
            hBtnReset = CreateWindow("BUTTON", "start new day", WS_VISIBLE | WS_CHILD,
                                     620, 15, 130, 30, hwnd, (HMENU)ID_BTN_RESET, NULL, NULL);

            hLabel = CreateWindow("STATIC", "habit name:", WS_VISIBLE | WS_CHILD,
                                  20, 65, 85, 22, hwnd, NULL, NULL, NULL);
            hInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                  110, 62, 330, 26, hwnd, (HMENU)ID_INPUT, NULL, NULL);

            hBtnAdd = CreateWindow("BUTTON", "add habit", WS_VISIBLE | WS_CHILD,
                                   450, 62, 95, 26, hwnd, (HMENU)ID_BTN_ADD, NULL, NULL);
            hBtnDone = CreateWindow("BUTTON", "mark done", WS_VISIBLE | WS_CHILD,
                                    555, 62, 95, 26, hwnd, (HMENU)ID_BTN_DONE, NULL, NULL);
            hBtnEdit = CreateWindow("BUTTON", "edit", WS_VISIBLE | WS_CHILD,
                                    660, 62, 42, 26, hwnd, (HMENU)ID_BTN_EDIT, NULL, NULL);
            hBtnDel = CreateWindow("BUTTON", "del", WS_VISIBLE | WS_CHILD,
                                   708, 62, 42, 26, hwnd, (HMENU)ID_BTN_DEL, NULL, NULL);

            hProgress = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD,
                                     20, 100, 730, 22, hwnd, NULL, NULL, NULL);

            hList = CreateWindow("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                                 20, 130, 730, 310, hwnd, (HMENU)ID_LIST, NULL, NULL);

            SendMessage(hTabHabits, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hTabDiary, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hBtnReset, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hLabel, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hInput, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hBtnAdd, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hBtnDone, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hBtnEdit, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hBtnDel, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hProgress, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(hList, WM_SETFONT, (WPARAM)font, TRUE);

            refreshList();
            break;

        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case ID_TAB_HABITS: switchTab(0); break;
                case ID_TAB_DIARY: switchTab(1); break;
                case ID_BTN_ADD: handleAddSave(); break;
                case ID_BTN_DONE: handleMarkDone(); break;
                case ID_BTN_EDIT: handleEditLoad(); break;
                case ID_BTN_DEL: handleDelete(); break;
                case ID_BTN_RESET: resetDailyStatus(); break;
            }
            break;

        case WM_DESTROY:
            saveHabits();
            saveDiary();
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int show) {
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    (void)hPrev;
    (void)args;

    loadHabits();
    loadDiary();

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "HabitQuestWin";

    if (!RegisterClass(&wc)) return 1;

    hwnd = CreateWindow("HabitQuestWin", "habitquest v1",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        CW_USEDEFAULT, CW_USEDEFAULT, 785, 495,
                        NULL, NULL, hInst, NULL);
    if (!hwnd) return 1;

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}