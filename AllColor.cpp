// AllColor.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "AllColor.h"

#define MAX_LOADSTRING 100
#define ID_TIMER 1 

static int iIncr;
static PALETTEENTRY pe;

HPALETTE CreateRoutine(HWND hwnd)
{
	HDC hdc;
	HPALETTE hPalette;
	LOGPALETTE lp;
	// Determine the color resolution and set iIncr
	hdc = GetDC(hwnd);
	iIncr = 1 << (8 - GetDeviceCaps(hdc, COLORRES) / 3);
	ReleaseDC(hwnd, hdc);
	// Create the logical palette
	lp.palVersion = 0x0300;
	lp.palNumEntries = 1;
	lp.palPalEntry[0].peRed = 0;
	lp.palPalEntry[0].peGreen = 0;
	lp.palPalEntry[0].peBlue = 0;
	lp.palPalEntry[0].peFlags = PC_RESERVED;

	hPalette = CreatePalette(&lp);
	// Save global for less typing
	pe = lp.palPalEntry[0];
	SetTimer(hwnd, ID_TIMER, 10, NULL);
	return hPalette;
}
void DisplayRGB(HDC hdc, PALETTEENTRY * ppe)
{
	TCHAR szBuffer[16];
	wsprintf(szBuffer, TEXT(" %02X-%02X-%02X "),
		ppe->peRed, ppe->peGreen,
		ppe->peBlue);
	TextOut(hdc, 0, 0, szBuffer, lstrlen(szBuffer));
}
void PaintRoutine(HDC hdc, int cxClient, int cyClient)
{
	HBRUSH hBrush;
	RECT rect;
	// Draw Palette Index 0 on entire window
	hBrush = CreateSolidBrush(PALETTEINDEX(0));
	SetRect(&rect, 0, 0, cxClient, cyClient);
	FillRect(hdc, &rect, hBrush);
	DeleteObject(SelectObject(hdc, GetStockObject(WHITE_BRUSH)));
	// Display the RGB value
	DisplayRGB(hdc, &pe);
	return;
}
void TimerRoutine(HDC hdc, HPALETTE hPalette)
{
	static BOOL bRedUp = TRUE, bGreenUp = TRUE, bBlueUp = TRUE;
	// Define new color value
	pe.peBlue += (bBlueUp ? iIncr : -iIncr);
	if (pe.peBlue == (BYTE)(bBlueUp ? 0 : 256 - iIncr))
	{
		pe.peBlue = (bBlueUp ? 256 - iIncr : 0);
		bBlueUp ^= TRUE;
		pe.peGreen += (bGreenUp ? iIncr : -iIncr);
		if (pe.peGreen == (BYTE)(bGreenUp ? 0 : 256 - iIncr))
		{
			pe.peGreen = (bGreenUp ? 256 - iIncr : 0);
			bGreenUp ^= TRUE;
			pe.peRed += (bRedUp ? iIncr : -iIncr);
			if (pe.peRed == (BYTE)(bRedUp ? 0 :
				256 - iIncr))
			{
				pe.peRed = (bRedUp ? 256 - iIncr : 0);
				bRedUp ^= TRUE;
			}
		}
	}
	// Animate the palette
	BOOL asdfd = AnimatePalette(hPalette, 0, 1, &pe);
	DWORD asdf = GetLastError();
	DisplayRGB(hdc, &pe);
	return;
}
void DestroyRoutine(HWND hwnd, HPALETTE hPalette)
{
	KillTimer(hwnd, ID_TIMER);
	DeleteObject(hPalette);
	return;
}


// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ALLCOLOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ALLCOLOR));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

BOOL CheckDisplay(HWND hwnd)
{
	HDC hdc;
	int iPalSize;
	hdc = GetDC(hwnd);
	iPalSize = GetDeviceCaps(hdc, SIZEPALETTE);
	ReleaseDC(hwnd, hdc);
	if (iPalSize != 256)
	{
		//MessageBox(hwnd, TEXT("This program requires that the video ") TEXT("display mode have a 256-color palette."), szWindowClass, MB_ICONERROR);
		//return FALSE;
	}
	return TRUE;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ALLCOLOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ALLCOLOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HPALETTE hPalette;
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		if (!CheckDisplay(hwnd))
			return -1;
		hPalette = CreateRoutine(hwnd);
		return 0;

	case WM_DISPLAYCHANGE:
		if (!CheckDisplay(hwnd))
			DestroyWindow(hwnd);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		PaintRoutine(hdc, cxClient, cyClient);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_TIMER:
		hdc = GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		TimerRoutine(hdc, hPalette);
		ReleaseDC(hwnd, hdc);
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_QUERYNEWPALETTE:
		if (!hPalette)
			return FALSE;
		hdc = GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		InvalidateRect(hwnd, NULL, TRUE);
		ReleaseDC(hwnd, hdc);
		return TRUE;
	case WM_PALETTECHANGED:
		if (!hPalette || (HWND)wParam == hwnd)
			break;
		hdc = GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		UpdateColors(hdc);
		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		DestroyRoutine(hwnd, hPalette);

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
