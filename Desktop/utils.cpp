#include "utils.h"
#include "VirtualDesktopAccessor.h"

const char *SETTINGS_FILE = "settings.ini";

// Gets a value from a config file: [Section]\nkey=value\n
// Ignores leading and trailing whitespace
char * ReadSettings(char *section, char *key, char *copy) {
    FILE *settingsFile;
    char content[MAX_PATH];

    fopen_s(&settingsFile, SETTINGS_FILE, "r");
    while (find('[', settingsFile)) {
        if (strcmp(section, get(']', settingsFile, content)) == 0) {
            while (find('\n', settingsFile)) {
                if (strcmp(key, get('=', settingsFile, content)) == 0) {
                    strcpy_s(copy, MAX_PATH, get('\n', settingsFile, content));
                    fclose(settingsFile);
                    return copy;
                }
            }
        }
    }
    fclose(settingsFile);
    return NULL;
}

// Advances a file until the character c is passed
// Returns false if it reaches end of file
// Designed for reading config files
bool find(char c, FILE *f) {
    int next;
    while (true) {
        next = fgetc(f);
        if (next == EOF) {
            return false;
        }
        if (next == c) {
            return true;
        }
    }
}

// Starting from the first character, retrieves all chars until character c
// c not included; max 260 chars; leading and trailing whitespace removed
// Designed for reading config files
char * get(char end, FILE *f, char *copy) {
    int next;
    size_t n = 0;

    copy[0] = '\0';
    while (true) {
        next = fgetc(f);
        if (next == EOF) {
            break;
        }
        next = (char)next;
        if (next == end || n == MAX_PATH - 1) {
            break;
        }
        if (n > 0 || !isspace(next)) {
            copy[n + 1] = '\0';
            copy[n] = next;
            n++;
        }
    }
    n = strlen(copy) - 1;
    while (n >= 0 && isspace(copy[n])) {
        n--;
    }
    copy[n+1] = '\0';
    return copy;
}

// Sets a wallpaper
void SetWallpaper(char *path) {
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, path, SPIF_SENDCHANGE);
}

// Sets wallpaper automatically based on desktop number and orientation
void RegisterDesktopChange() {
    int desktop = GetCurrentDesktopNumber() + 1;
    char desktop_str[2];
    char desktop_path[MAX_PATH];
    char *orientation = GetDesktopOrientation() == HORIZONTAL ? "Horizontal" : "Vertical";
    sprintf_s(desktop_str, 2, "%d", desktop);
    SetWallpaper(ReadSettings(orientation, desktop_str, desktop_path));
}

// Gets desktop orientation
DesktopOrientation GetDesktopOrientation() {
    if (GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN)) {
        return HORIZONTAL;
    } else {
        return VERTICAL;
    }
}

// Sets desktop orientation, if different from current
bool SetDesktopOrientation(DesktopOrientation orientation) {
    if (GetDesktopOrientation() == orientation) {
        return false;
    } else {
        DISPLAY_DEVICE display;
        display.cb = sizeof(DISPLAY_DEVICE);
        if (EnumDisplayDevices(NULL, 0, &display, EDD_GET_DEVICE_INTERFACE_NAME) == 0) {
            MessageBox(NULL, "Failed to Load Display Devices!", "Error!",
                       MB_ICONEXCLAMATION | MB_OK);
            return false;
        }
        
        DEVMODE mode;
        mode.dmSize = sizeof(DEVMODE);
        if (EnumDisplaySettings(display.DeviceName, ENUM_CURRENT_SETTINGS, &mode) == 0) {
            MessageBox(NULL, "Failed to Load Display Devmode!", "Error!",
                       MB_ICONEXCLAMATION | MB_OK);
            return false;
        }

        // Note that this means we will rotate the display by 90 degrees
        // So that either the top or left of the monitor faces up
        DWORD f;
        f = mode.dmPelsWidth;
        mode.dmPelsWidth = mode.dmPelsHeight;
        mode.dmPelsHeight = f;
        mode.dmDisplayOrientation = orientation == HORIZONTAL ? DMDO_DEFAULT : DMDO_90;

        if (ChangeDisplaySettingsEx(display.DeviceName, &mode, NULL, 0, NULL)
            == DISP_CHANGE_SUCCESSFUL) {
            return true;
        } else {
            MessageBox(NULL, "Failed to Change Display Settings!", "Error!",
                       MB_ICONEXCLAMATION | MB_OK);
            return false;
        }
    }
}

// Refresh Rainmeter
void RefreshRainmeter(DesktopOrientation orientation) {
    char *newID = orientation == HORIZONTAL ? "Rainmeter-Horizontal" : "Rainmeter-Vertical";
    char path[MAX_PATH];
    char settings[MAX_PATH];
    char overlay[MAX_PATH];
    char newSettings[MAX_PATH];
    char newOverlay[MAX_PATH];

    ReadSettings("Rainmeter", "path", path);
    ReadSettings("Rainmeter", "settings", settings);
    ReadSettings("Rainmeter", "overlay", overlay);
    ReadSettings(newID, "settings", newSettings);
    ReadSettings(newID, "overlay", newOverlay);

    if (path) {
        int rs = CopyFile(newSettings, settings, FALSE);
        int ro = CopyFile(newOverlay, overlay, FALSE);

        ShellExecute(NULL, NULL, path, "!RefreshApp", NULL, SW_HIDE);
    }
}