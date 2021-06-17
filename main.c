#define _CRT_SECURE_NO_WARNINGS
#include  <windows.h>
#include <math.h>
#include <stdio.h>
#include "Header.h"

extern double a, b, B;
extern double* x, *x1, *y;
extern int n;
char k[] = "C:\\mass\\test0.txt";

BOOL APIENTRY ExampleDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void PaintGrid(HDC hdc, int left, int right, int top, int bottom);
void PaintGraph(HDC hdc, int left, int right, int top, int bottom);
void KeyPoints(HDC hdc, int left, int right, int top, int bottom);
void getApprox(const char* k);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_ASTERISK);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "MainMenu";
	wc.lpszClassName = "SimpleApplication";
	RegisterClass(&wc);


	HWND hWnd = CreateWindow("SimpleApplication",
		"Graph",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

HPEN hPenKey, hPenGraph;
double xMin = -6, xMax = 13, xStep = 1, yMin = 0, yMax = 7, yStep = 1;

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT r;
	int i, j;
	static bshow = MF_CHECKED;
	static OPENFILENAME ofn;
	static char strFile[MAX_PATH];
	static CHOOSECOLOR cc;
	static COLORREF CustColors[16];
	switch (uMsg)
	{
	case WM_CREATE:
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = strFile;
		ofn.nMaxFile = MAX_PATH;

		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hWnd;
		cc.lpCustColors = CustColors;
		cc.Flags = CC_RGBINIT | CC_FULLOPEN;

		hPenKey = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		hPenGraph = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
		getApprox(k);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &r);
		PaintGrid(hdc, r.left + 100, r.right - 100, r.top + 50, r.bottom - 50);
		if (bshow == MF_CHECKED)
		{
			PaintGraph(hdc, r.left + 100, r.right - 100, r.top + 50, r.bottom - 50);
			KeyPoints(hdc, r.left + 100, r.right - 100, r.top + 50, r.bottom - 50);
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPTIONS:
			DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), "ExampleDialog", hWnd, ExampleDlgProc);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_OPEN:
			if (GetOpenFileName(&ofn))
			{
				getApprox(strFile);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			MessageBox(hWnd, "Линейная аппроксимация", "Справка", MB_OK);
			break;
		case IDM_COLORGRAPH:
			if (ChooseColor(&cc))
			{
				DeleteObject(&hPenGraph);
				hPenGraph = CreatePen(PS_SOLID, 3, cc.rgbResult);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				break;
			}
		case IDM_COLORPOINTS:
			if (ChooseColor(&cc))
			{
				DeleteObject(&hPenKey);
				hPenKey = CreatePen(PS_SOLID, 3, cc.rgbResult);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				break;
			}
		case IDM_SHOW:
			bshow = bshow == MF_CHECKED ? MF_UNCHECKED : MF_CHECKED;
			CheckMenuItem(GetMenu(hWnd), IDM_SHOW, bshow);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		return 0;
	case WM_DESTROY:
		DeleteObject(hPenKey);
		DeleteObject(hPenGraph);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void PaintGrid(HDC hdc, int left, int right, int top, int bottom)
{
	double p, pStep, mark;
	char str[100];
	SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);
	pStep = (bottom - top) / (yMax - yMin) * yStep;
	for (p = top, mark = yMax; p <= bottom + 1; p += pStep, mark -= yStep)
	{
		MoveToEx(hdc, left, p, NULL);
		LineTo(hdc, right, p);
		TextOut(hdc, left - 4, p + 4, str, sprintf(str, "%.1f", mark));
	}
	pStep = (right - left) / (xMax - xMin) * xStep;
	SetTextAlign(hdc, TA_CENTER | TA_TOP);
	for (p = left, mark = xMin; p <= right + 1; p += pStep, mark += xStep)
	{
		MoveToEx(hdc, p, top, NULL);
		LineTo(hdc, p, bottom);
		TextOut(hdc, p, bottom + 2, str, sprintf(str, "%.1f", mark));
	}
}

void PaintGraph(HDC hdc, int left, int right, int top, int bottom)
{
	double x = xMin, xs, px = left, py = bottom - (a * x + b)*(bottom - top) / (yMax - yMin) + yMin*(bottom - top) / (yMax - yMin);
	MoveToEx(hdc, px, py, NULL);
	SelectObject(hdc, hPenGraph);
	for (xs = (xMax - xMin) / (right - left); px <= right; px += 1, x += xs)
	{
		py = bottom - (a * x + b) * (bottom - top) / (yMax - yMin)+ yMin*(bottom - top) / (yMax - yMin);
		if (py >= bottom)
			MoveToEx(hdc, px, bottom, NULL);
		if (py <= top)
			MoveToEx(hdc, px, top,NULL);
		if(py>=top && py<=bottom)
			LineTo(hdc, px, py);
	}
}

void KeyPoints(HDC hdc, int left, int right, int top, int bottom)
{
	double px, py;
	int i;
	MoveToEx(hdc, left, bottom, NULL);
	SelectObject(hdc, hPenKey);
	for (i = 0; i < n; i++)
	{
		if (x[i] <= xMax && x[i] >= xMin && y[i] <= yMax && y[i] >= yMin)
		{
			px = (x[i] - xMin) * (right - left) / (xMax - xMin) + left;
			py = bottom - (y[i] - yMin) * (bottom - top) / (yMax - yMin);
			Ellipse(hdc, px - 4, py - 4, px + 4, py + 4);
		}
	}
}

BOOL APIENTRY ExampleDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char str[256];
	switch (uMsg)
	{
	case WM_INITDIALOG:
		sprintf(str, "%g", xMin);
		SetDlgItemText(hWnd, IDC_EDIT1, str);
		sprintf(str, "%g", xMax);
		SetDlgItemText(hWnd, IDC_EDIT2, str);
		sprintf(str, "%g", xStep);
		SetDlgItemText(hWnd, IDC_EDIT3, str);
		sprintf(str, "%g", yMin);
		SetDlgItemText(hWnd, IDC_EDIT4, str);
		sprintf(str, "%g", yMax);
		SetDlgItemText(hWnd, IDC_EDIT5, str);
		sprintf(str, "%g", yStep);
		SetDlgItemText(hWnd, IDC_EDIT6, str);
		sprintf(str, "%g", a);
		SetDlgItemText(hWnd, IDC_A, str);
		sprintf(str, "%g", b);
		SetDlgItemText(hWnd, IDC_b, str);
		sprintf(str, "%g", B);
		SetDlgItemText(hWnd, IDC_B, str);

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hWnd, IDC_EDIT1, str, 256);
			xMin = atof(str);
			GetDlgItemText(hWnd, IDC_EDIT2, str, 256);
			xMax = atof(str);
			GetDlgItemText(hWnd, IDC_EDIT3, str, 256);
			xStep = atof(str);
			GetDlgItemText(hWnd, IDC_EDIT4, str, 256);
			yMin = atof(str);
			GetDlgItemText(hWnd, IDC_EDIT5, str, 256);
			yMax = atof(str);
			GetDlgItemText(hWnd, IDC_EDIT6, str, 256);
			yStep = atof(str);
			GetDlgItemText(hWnd, IDC_A, str, 256);
			a = atof(str);
			GetDlgItemText(hWnd, IDC_b, str, 256);
			b = atof(str);
			GetDlgItemText(hWnd, IDC_B, str, 256);
			B = atof(str);
		case IDCANCEL:
			EndDialog(hWnd, wParam);
			return TRUE;
		}
	}
	return FALSE;
}
