#include <iostream>
#include <windows.h>

#define ID_BUTTON_SHUTDOWN 1001
#define ID_CHECKBOX_STARTUP 1002
#define ID_MENU_ABOUT 1003
#define ID_MENU_WEBSITE 1004
#define ID_APPICON 1005

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hThisInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpszArgument,
                   int nFunsterStil)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "WindowsApp";
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    wincl.hIcon = LoadIcon(hThisInstance, MAKEINTRESOURCE(ID_APPICON));
    wincl.hIconSm = LoadIcon(hThisInstance, MAKEINTRESOURCE(ID_APPICON));
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    if (!RegisterClassEx(&wincl))
        return 0;

    hwnd = CreateWindowEx(
        0,
        "WindowsApp",
        "Shutdown Timer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        200,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL);

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nFunsterStil);

    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hButtonShutdown, hEditDelay, hCheckBoxStartup;

    switch (message)
    {
    case WM_CREATE:
    {
        /* Buat tombol "Mulai Shutdown" */
        hButtonShutdown = CreateWindow(
            "BUTTON",
            "Mulai Shutdown",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20,
            40,
            120,
            30,
            hwnd,
            (HMENU)ID_BUTTON_SHUTDOWN,
            NULL,
            NULL);

        /* Buat input teks untuk waktu penundaan */
        hEditDelay = CreateWindow(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            160,
            75,
            80,
            20,
            hwnd,
            NULL,
            NULL,
            NULL);

        /* Buat centang "Mulai pada Startup" */
        hCheckBoxStartup = CreateWindow(
            "BUTTON",
            "Mulai pada Startup",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            20,
            120,
            140,
            20,
            hwnd,
            (HMENU)ID_CHECKBOX_STARTUP,
            NULL,
            NULL);

        /* Buat menu */
        HMENU hMenu = CreateMenu();
        HMENU hSubMenu = CreatePopupMenu();
        AppendMenu(hSubMenu, MF_STRING, ID_MENU_ABOUT, "About");
        AppendMenu(hSubMenu, MF_STRING, ID_MENU_WEBSITE, "Website");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "Menu");

        SetMenu(hwnd, hMenu);
        break;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_BUTTON_SHUTDOWN)
        {
            char buffer[256];
            GetWindowText(hEditDelay, buffer, sizeof(buffer));
            int delay = atoi(buffer);

            int milliseconds = delay * 1000;

            MessageBox(hwnd, "Shutdown akan dimulai.", "Informasi", MB_OK | MB_ICONINFORMATION);
            Sleep(milliseconds);
            system("shutdown /s /t 0");
        }
        else if (LOWORD(wParam) == ID_CHECKBOX_STARTUP)
        {
            BOOL isChecked = SendMessage(hCheckBoxStartup, BM_GETCHECK, 0, 0);
            if (isChecked == BST_CHECKED)
            {
                HKEY hKey;
                RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
                char path[MAX_PATH];
                GetModuleFileName(NULL, path, MAX_PATH);
                RegSetValueEx(hKey, "ShutdownTimer", 0, REG_SZ, (BYTE *)path, strlen(path) + 1);
                RegCloseKey(hKey);
            }
            else
            {
                HKEY hKey;
                RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
                RegDeleteValue(hKey, "ShutdownTimer");
                RegCloseKey(hKey);
            }
        }
        else if (LOWORD(wParam) == ID_MENU_ABOUT)
        {
            MessageBox(hwnd, "Shutdown Timer\n\nVersi 1.0\n\nDibuat oleh Fajar Julyana", "Tentang", MB_OK | MB_ICONINFORMATION);
        }
        else if (LOWORD(wParam) == ID_MENU_WEBSITE)
        {
            ShellExecute(NULL, "open", "https://fajarjulyana.github.io", NULL, NULL, SW_SHOWNORMAL);
        }

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
