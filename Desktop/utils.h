#include "stdafx.h"

typedef enum {
    HORIZONTAL, VERTICAL
} DesktopOrientation;

char * ReadSettings(char *section, char *key, char *copy);
bool find(char c, FILE *f);
char * get(char end, FILE *f, char *copy);
void SetWallpaper(char *path);
void RegisterDesktopChange();
DesktopOrientation GetDesktopOrientation();
bool SetDesktopOrientation(DesktopOrientation orientation);
void RefreshRainmeter(DesktopOrientation orientation);