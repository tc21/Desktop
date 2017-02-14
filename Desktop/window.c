#include <Windows.h>

const char className[] = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int scaledForMonitor(int size);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    // Declare DPI Awareness
    if (!SetProcessDPIAware()) {
        MessageBox(NULL, "Failed to Set DPI Awareness!", "Error",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Register window class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Windows Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Get preferred window sizes

    // Create the window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        className,
        "Title of window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, scaledForMonitor(480), scaledForMonitor(240),
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL) {
        MessageBox(NULL, "Windows Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    // Show about window in lmb down
    case WM_LBUTTONDOWN:
    {
        char fileName[MAX_PATH];
        HINSTANCE hInstance = GetModuleHandle(NULL);
        GetModuleFileName(hInstance, fileName, MAX_PATH);
        MessageBox(hwnd, fileName, "This program is:", MB_OK | MB_ICONINFORMATION);
    }
        break;
    // When the window is closed
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int scaledForMonitor(int size) {
    HDC screen = GetDC(0);
    float dpiScale = GetDeviceCaps(screen, LOGPIXELSX) / 96.0f;
    ReleaseDC(0, screen);

    return (int)(size * dpiScale);
}