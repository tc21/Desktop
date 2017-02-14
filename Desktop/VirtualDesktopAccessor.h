#pragma once
#include "stdafx.h"

// Custom-written header for VirtualDesktopAccessor.h

void EnableKeepMinimized();

void RestoreMinimized();

int GetDesktopCount();

int GetDesktopNumberById(GUID desktopId);

GUID GetWindowDesktopId(HWND window);

int GetWindowDesktopNumber(HWND window);

int IsWindowOnCurrentVirtualDesktop(HWND window);

GUID GetDesktopIdByNumber(int number);

int IsWindowOnDesktopNumber(HWND window, int number);

BOOL MoveWindowToDesktopNumber(HWND window, int number);

int GetCurrentDesktopNumber();

void GoToDesktopNumber(int number);

int IsPinnedWindow(HWND hwnd);

void PinWindow(HWND hwnd);

void UnPinWindow(HWND hwnd);;

int IsPinnedApp(HWND hwnd);

void PinApp(HWND hwnd);

void UnPinApp(HWND hwnd);

void RegisterPostMessageHook(HWND listener, int messageOffset);

void UnregisterPostMessageHook(HWND hwnd);