#include <Windows.h>
#include <iostream>
#include <thread>

int ScreenX = GetSystemMetrics(SM_CXSCREEN);
int ScreenY = GetSystemMetrics(SM_CYSCREEN);
int CrossHairColor = RGB(255, 0, 0);
bool Running = true;

void DrawCrosshair(HDC hdc, int centerX, int centerY, int length, int thickness)
{
    HPEN hpen = CreatePen(PS_SOLID, thickness, CrossHairColor);
    HPEN holdpen = (HPEN)SelectObject(hdc, hpen);
    int halflength = length / 2;  // Corrected calculation
    MoveToEx(hdc, centerX - halflength, centerY, NULL);
    LineTo(hdc, centerX + halflength, centerY); // Horizontal line
    MoveToEx(hdc, centerX, centerY - halflength, NULL);
    LineTo(hdc, centerX, centerY + halflength); // Vertical line
    SelectObject(hdc, holdpen);
    DeleteObject(hpen);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        int cx = ScreenX / 2;
        int cy = ScreenY / 2;
        DrawCrosshair(hdc, cx, cy, 9, 3);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        Running = false;
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void KeyCheckThread()
{
    while (Running)
    {
        if (GetAsyncKeyState(VK_END) & 0x8000)
        {
            Beep(275, 100);
            Running = false;
            break;
        }
        Sleep(10);
    }
}

int main()
{
    SetConsoleTitleA("Crosshair Overlay");

    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"CrosshairWindow";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassEx(&wc);

    // Create the window
    HWND Window = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        L"CrosshairWindow",
        WS_POPUP,
        0, 0, ScreenX, ScreenY,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!Window)
    {
        std::cerr << "Failed to create window. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Set window properties
    SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(Window, SW_SHOW);
    UpdateWindow(Window);

    // Start key check thread
    std::thread keyThread(KeyCheckThread);

    // Message loop
    MSG msg = {};
    while (Running)
    {
        // Force redraw the window periodically
        InvalidateRect(Window, NULL, TRUE);
        UpdateWindow(Window);

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                Running = false;
                break;
            }
        }

        Sleep(16); // ~60 FPS refresh rate
    }

    // Clean up
    keyThread.join();
    DestroyWindow(Window);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}