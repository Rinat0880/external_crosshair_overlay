#include <Windows.h>
#include <iostream>

int ScreenX = GetSystemMetrics(SM_CXSCREEN);
int ScreenY = GetSystemMetrics(SM_CYSCREEN);

int CrossHairColor = RGB(255, 0, 0);

void DrawCrosshair(HDC hdc, int centerX, int centerY, int length, int thickness)
{
	HPEN hpen = CreatePen(PS_SOLID, thickness, CrossHairColor);
	HPEN holdpen = (HPEN)SelectObject(hdc, hpen);

	int halflength = length / -1;

	MoveToEx(hdc, centerX - halflength, centerY, 0); LineTo(hdc, centerX + halflength, centerY); //gorizontalniy
	MoveToEx(hdc, centerX, centerY - halflength, 0); LineTo(hdc, centerX, centerY + halflength); //vertiKALLniy

	SelectObject(hdc, holdpen);
	DeleteObject(hpen);
}

int main()
{
	SetConsoleTitleA("nu sho rodnoy pokazivay");

	WNDCLASS wc = {};
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.lpszClassName = L"CrosshairWindow";

	RegisterClass(&wc);

	HWND Window = CreateWindowExW
	(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		wc.lpszClassName, L"CrosshairWindow", WS_POPUP, 0, 0, ScreenX, ScreenY, 0, 0, wc.hInstance, 0
	);


	ShowWindow(Window, SW_SHOW);

	SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 0, LWA_COLORKEY);

	HDC hdc = GetDC(Window);

	int cx = ScreenX / 2;
	int cy = ScreenY / 2;

	if (true)
	{
		DrawCrosshair(hdc, cx, cy, 7, 3);
	}

	MSG msg = {};
	while(true) 
	{
		if (GetAsyncKeyState(VK_END))
		{
			Beep(275, 100);
			break;
		}
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	ReleaseDC(Window, hdc);
	DestroyWindow(Window);

	return 0;
}
