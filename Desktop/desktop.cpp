//TODOs:
// 1. Unload settings file, so it can be edited and reloaded without exiting the program
// 2. Icon
// 3. Template settings file
// 4. I forgot


#include "stdafx.h"
#include "utils.h"
#include "VirtualDesktopAccessor.h"
#include "resource.h"

extern const char *SETTINGS_FILE;
const char className[]       = "desktopClass";
const char windowName[]      = "Desktop";
const UINT WM_TRAY           = WM_USER + 1;
const UINT WM_DESKTOP_CHANGE = WM_USER + 2;
HINSTANCE  mainInstance      = NULL;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// We create a window that never shows to handle messages
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS windowClass;
    HWND mainWindow;
    MSG message;

    if (!SetProcessDPIAware()) {
        MessageBox(NULL, "Failed to Set DPI Awareness!", "Error",
                   MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    ZeroMemory(&windowClass, sizeof(WNDCLASS));
    windowClass.lpszClassName = className;
    windowClass.hInstance     = hInstance;
    windowClass.lpfnWndProc   = WndProc;

    if (!RegisterClass(&windowClass)) {
        MessageBox(NULL, "Failed to Register Window Class!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    mainWindow = CreateWindow(className, "", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    if (mainWindow == NULL) {
        MessageBox(NULL, "Failed to Create Window!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    mainInstance = hInstance;

    ShowWindow(mainWindow, SW_HIDE);
    RegisterDesktopChange();
    // Register a hook to detect desktop changes
    RegisterPostMessageHook(mainWindow, WM_DESKTOP_CHANGE);
    // Hotkeys
    RegisterHotKey(mainWindow, HORIZONTAL, MOD_ALT | MOD_CONTROL, VK_DOWN);
    RegisterHotKey(mainWindow, VERTICAL, MOD_ALT | MOD_CONTROL, VK_RIGHT);


    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return message.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // First handle tray icon messages, which can't be handled with a switch
    if (msg == WM_TRAY) {
        switch (lParam) {
        case WM_LBUTTONUP:
        {
        }
            break;
        case WM_RBUTTONUP:
        {
            HMENU menu = LoadMenu(mainInstance, MAKEINTRESOURCE(IDR_MENU));
            POINT cursor;
            GetCursorPos(&cursor);
            if (menu == NULL) {
                MessageBox(NULL, "Failed to Create Popup Menu!", "Error!",
                           MB_ICONEXCLAMATION | MB_OK);
                return -1;

            }
            HMENU submenu = GetSubMenu(menu, 0);
            if (submenu == NULL) {
                MessageBox(NULL, "Failed to Create Popup Submenu!", "Error!",
                           MB_ICONEXCLAMATION | MB_OK);
                return -1;

            }
            SetForegroundWindow(hwnd);
            TrackPopupMenu(submenu, TPM_RIGHTBUTTON,
                           cursor.x, cursor.y, 0, hwnd, NULL);
            DestroyMenu(menu);
        }
            break;
        }
        return 0;
    }
    // Change desktop background when virtual desktop changes
    if (msg == WM_DESKTOP_CHANGE) {
        RegisterDesktopChange();
        return 0;
    }
    switch (msg) {
    // Creates notification area icon
    case WM_CREATE:
    {
        NOTIFYICONDATA iconData;
        ZeroMemory(&iconData, sizeof(NOTIFYICONDATA));
        iconData.cbSize           = sizeof(NOTIFYICONDATA);
        iconData.hWnd             = hwnd;
        iconData.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        iconData.uCallbackMessage = WM_TRAY;
        iconData.uID              = (UINT)IDI_APPLICATION;  // Used when the icon is deleted
        iconData.hIcon = (HICON)LoadImage(
            mainInstance,
            MAKEINTRESOURCE(IDI_APPLICATION),
            IMAGE_ICON,
            0, 0,
            LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE
        );
        lstrcpyn(iconData.szTip, windowName, sizeof(iconData.szTip));
        if (!Shell_NotifyIcon(NIM_ADD, &iconData)) {
            MessageBox(NULL, "Failed to Create Notification Area Icon!", "Error!",
                       MB_ICONEXCLAMATION | MB_OK);
            return -1;
        }
    }  
    break;
    // Handles clicks on notification area icon menu
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_CONFIG:
            //Note: perhaps if it doesn't exist we could create a template
            ShellExecute(NULL, NULL, SETTINGS_FILE, NULL, NULL, SW_HIDE);
            break;
        case ID_RELOAD:
            RegisterDesktopChange();
            break;
        case ID_EXIT:
            DestroyWindow(hwnd);
        }
        break;
    // Hotkeys for rotation
    case WM_HOTKEY:
    {

        DesktopOrientation orientation = (DesktopOrientation)wParam;
        if (SetDesktopOrientation(orientation)) {
            RegisterDesktopChange();
            RefreshRainmeter(orientation);
        }
    }
        break;
    // When the window is closed
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
    {
        NOTIFYICONDATA iconData;
        ZeroMemory(&iconData, sizeof(NOTIFYICONDATA));
        iconData.cbSize = sizeof(NOTIFYICONDATA);
        iconData.hWnd   = hwnd;
        iconData.uID    = (UINT)IDI_APPLICATION;
        Shell_NotifyIcon(NIM_DELETE, &iconData);
    }
        PostQuitMessage(0);
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}