#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#define MAX_WINDOWS 10

typedef struct {
    HWND hwnd;
    TCHAR title[256];
} NotepadWindow;

NotepadWindow g_windows[MAX_WINDOWS];
int g_count = 0;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    TCHAR className[256];
    GetClassName(hwnd, className, sizeof(className) / sizeof(TCHAR));

    if (_tcscmp(className, _T("Notepad")) == 0 && g_count < MAX_WINDOWS) {
        GetWindowText(hwnd, g_windows[g_count].title, 256);
        g_windows[g_count].hwnd = hwnd;
        g_count++;
    }
    return TRUE;
}

void SelectAllText(HWND hwndEdit) {
    SendMessage(hwndEdit, EM_SETSEL, 0, -1);
    Sleep(100);
}

void SimulateCopyPaste(HWND hwndEdit, HWND hwndTopLevel) {
    SelectAllText(hwndEdit);
    SendMessage(hwndEdit, WM_COPY, 0, 0);
    SendMessage(hwndEdit, WM_PASTE, 0, 0);
    SendMessage(hwndEdit, WM_PASTE, 0, 0);
}

HWND FindChildWindow(HWND parent, LPCTSTR className) {
    HWND child = NULL;
    while ((child = FindWindowEx(parent, child, NULL, NULL)) != NULL) {
        TCHAR cls[256];
        GetClassName(child, cls, 256);

        if (_tcscmp(cls, className) == 0) {
            return child;
        }

        HWND deeper = FindChildWindow(child, className);
        if (deeper) return deeper;
    }
    return NULL;
}

int main() {
    EnumWindows(EnumWindowsProc, 0);

    if (g_count == 0) {
        printf("�޸��� â�� ã�� �� �����ϴ�.\n");
        return 1;
    }

    printf("���� �ִ� �޸��� ���:\n");
    for (int i = 0; i < g_count; ++i) {
        printf("%d: %s (HWND: 0x%p)\n", i, g_windows[i].title, g_windows[i].hwnd);
    }

    printf("\n��� �޸��� ��ȣ�� �Է��ϼ���: ");
    int index;
    scanf("%d", &index);

    if (index < 0 || index >= g_count) {
        printf("�߸��� ��ȣ�Դϴ�.\n");
        return 1;
    }

    HWND hwndEdit = FindChildWindow(g_windows[index].hwnd, _T("RichEditD2DPT"));
    if (hwndEdit == NULL) {
        printf("������ �޸��忡�� ���� ��Ʈ���� ã�� �� �����ϴ�.\n");
        return 1;
    }

    printf("���� ��Ʈ�� �ڵ�: 0x%p\n", hwndEdit);

    SimulateCopyPaste(hwndEdit, g_windows[index].hwnd);

    printf("���� �� �ٿ��ֱ� �۾��� �Ϸ�Ǿ����ϴ�.\n");
    return 0;
}