#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");
TCHAR szChildClassName[] = TEXT("ChildWindow");
TCHAR szLayerClassName[] = TEXT("LayerWindow");
HWND hChild = 0;

LRESULT CALLBACK LayerWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_SIZE:
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

enum CHILD_POSITION {
 NONE,
 LEFT,
 TOP,
 RIGHT,
 BOTTOM,
};

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hLayer;
	static BOOL bWindowMove;
	static BOOL bCaptionDown;
	switch (msg)
	{
	case WM_CREATE:
		hLayer = (HWND)((LPCREATESTRUCT)lParam)->lpCreateParams;
		break;
	case WM_MOVE:
		if (bWindowMove) {
			HWND hParent = GetWindow(hWnd, GW_OWNER);
			RECT rect;
			GetClientRect(hParent, &rect);
			ClientToScreen(hParent, (LPPOINT)&rect.left);
			ClientToScreen(hParent, (LPPOINT)&rect.right);
			POINT point;
			GetCursorPos(&point);
			// オーナーウィンドウの左側
			RECT rectLeft = rect;
			rectLeft.right = rectLeft.left + 200;
			// オーナーウィンドウの右側
			RECT rectRight = rect;
			rectRight.left = rectRight.right - 200;
			// オーナーウィンドウの上側
			RECT rectTop = rect;
			rectTop.bottom = rectTop.top + 200;
			// オーナーウィンドウの下側
			RECT rectBottom = rect;
			rectBottom.top = rectBottom.bottom - 200;
			if (PtInRect(&rectLeft, point)) {
				MoveWindow(hLayer, rectLeft.left, rectLeft.top, rectLeft.right - rectLeft.left, rectLeft.bottom - rectLeft.top, TRUE);
				SetWindowLongPtr(hLayer, GWLP_USERDATA, (LONG_PTR)CHILD_POSITION::LEFT);
				ShowWindow(hLayer, SW_SHOW);
			}
			else if (PtInRect(&rectRight, point)) {
				MoveWindow(hLayer, rectRight.left, rectRight.top, rectRight.right - rectRight.left, rectRight.bottom - rectRight.top, TRUE);
				SetWindowLongPtr(hLayer, GWLP_USERDATA, (LONG_PTR)CHILD_POSITION::RIGHT);
				ShowWindow(hLayer, SW_SHOW);
			}
			else if (PtInRect(&rectTop, point)) {
				MoveWindow(hLayer, rectTop.left, rectTop.top, rectTop.right - rectTop.left, rectTop.bottom - rectTop.top, TRUE);
				SetWindowLongPtr(hLayer, GWLP_USERDATA, (LONG_PTR)CHILD_POSITION::TOP);
				ShowWindow(hLayer, SW_SHOW);
			}
			else if (PtInRect(&rectBottom, point)) {
				MoveWindow(hLayer, rectBottom.left, rectBottom.top, rectBottom.right - rectBottom.left, rectBottom.bottom - rectBottom.top, TRUE);
				SetWindowLongPtr(hLayer, GWLP_USERDATA, (LONG_PTR)CHILD_POSITION::BOTTOM);
				ShowWindow(hLayer, SW_SHOW);
			}
			else {
				SetWindowLongPtr(hLayer, GWLP_USERDATA, (LONG_PTR)CHILD_POSITION::NONE);
				ShowWindow(hLayer, SW_HIDE);
			}
		}
		break;
	case WM_NCHITTEST:
		{
			LRESULT ret = DefWindowProc(hWnd, msg, wParam, lParam);
			switch (GetWindowLongPtr(hChild, GWLP_USERDATA)) {
			case LEFT:
				if (ret == HTLEFT || ret == HTTOP || ret == HTBOTTOM || ret == HTTOPLEFT ||  ret == HTBOTTOMLEFT) {
					ret = HTNOWHERE;
				}
				else if (ret == HTTOPRIGHT || ret == HTBOTTOMRIGHT) {
					ret = HTRIGHT;
				}
				break;
			case TOP:
				if (ret == HTLEFT || ret == HTTOP || ret == HTRIGHT || ret == HTTOPLEFT || ret == HTTOPRIGHT) {
					ret = HTNOWHERE;
				}
				else if (ret == HTBOTTOMLEFT || ret == HTBOTTOMRIGHT) {
					ret = HTBOTTOM;
				}
				break;
			case RIGHT:
				if (ret == HTTOP || ret == HTRIGHT || ret == HTBOTTOM || ret == HTTOPRIGHT || ret == HTBOTTOMRIGHT) {
					ret = HTNOWHERE;
				}
				else if (ret == HTBOTTOMLEFT || ret == HTTOPLEFT) {
					ret = HTLEFT;
				}
				break;
			case BOTTOM:
				if (ret == HTLEFT || ret == HTRIGHT || ret == HTBOTTOM || ret == HTBOTTOMLEFT || ret == HTBOTTOMRIGHT) {
					ret = HTNOWHERE;
				}
				else if (ret == HTTOPLEFT || ret == HTTOPRIGHT) {
					ret = HTTOP;
				}
				break;
			}
			return ret;
		}
		break;
	case WM_NCLBUTTONDOWN:
		if (wParam == HTCAPTION) {
			bCaptionDown = TRUE;
		}
		else {
			bCaptionDown = FALSE;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_APP:
		if (GetAncestor(hWnd, GA_PARENT) == GetWindow(hWnd, GW_OWNER)) { // 子ウィンドウの場合は、親子関係を無くす
			RECT rect;
			GetWindowRect(hWnd, &rect);
			MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
			SetParent(hWnd, NULL);
			ClipCursor(NULL);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, CHILD_POSITION::NONE);
		}
		break;
	case WM_ENTERSIZEMOVE:
		if (bCaptionDown) {
			bWindowMove = TRUE;
			PostMessage(hWnd, WM_APP, 0, 0);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_EXITSIZEMOVE:
		if (IsWindowVisible(hLayer)) {
			RECT rect;
			GetWindowRect(hLayer, &rect);
			HWND hParent = GetWindow(hWnd, GW_OWNER);
			ScreenToClient(hParent, (LPPOINT)&rect.left);
			ScreenToClient(hParent, (LPPOINT)&rect.right);
			SetParent(hWnd, hParent);
			MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, GetWindowLongPtr(hLayer, GWLP_USERDATA));
			ShowWindow(hLayer, SW_HIDE);
		}
		bWindowMove = FALSE;
		bCaptionDown = FALSE;
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_SIZE:
		if (IsWindowVisible(hChild) && GetAncestor(hChild, GA_PARENT) == hWnd) {
			RECT rectParent;
			GetClientRect(hWnd, &rectParent);
			RECT rectChild;
			GetWindowRect(hChild, &rectChild);
			const int nChildWidth = rectChild.right - rectChild.left;
			const int nChildHeight = rectChild.bottom - rectChild.top;
			switch (GetWindowLongPtr(hChild, GWLP_USERDATA)) {
			case LEFT:
				MoveWindow(hChild, 0, 0, nChildWidth, rectParent.bottom, TRUE);
				break;
			case TOP:
				MoveWindow(hChild, 0, 0, rectParent.right, nChildHeight, TRUE);
				break;
			case RIGHT:
				MoveWindow(hChild, rectParent.right - nChildWidth, 0, nChildWidth, rectParent.bottom, TRUE);
				break;
			case BOTTOM:
				MoveWindow(hChild, 0, rectParent.bottom - nChildHeight, rectParent.right, nChildHeight, TRUE);
				break;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 92, 232));
	HWND hLayerWindow = 0;
	{
		WNDCLASS wndclass = { 0,LayerWndProc,0,0,hInstance,0,0,hBrush,0,szLayerClassName};
		RegisterClass(&wndclass);
		hLayerWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST, szLayerClassName, 0, WS_POPUP, 0, 0, 128, 128, 0, 0, hInstance, 0);
	}
	SetLayeredWindowAttributes(hLayerWindow, 0, 64, LWA_ALPHA);

	{
		WNDCLASS wndclass = { CS_HREDRAW | CS_VREDRAW,ChildWndProc,0,0,hInstance,0,LoadCursor(0,IDC_ARROW),(HBRUSH)(COLOR_WINDOW + 1),0,szChildClassName };
		RegisterClass(&wndclass);
		hChild = CreateWindowEx(WS_EX_TOOLWINDOW, szChildClassName, TEXT("Child Window"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWnd, 0, hInstance, (LPVOID)hLayerWindow);
	}
	ShowWindow(hChild, SW_SHOWDEFAULT);
	UpdateWindow(hChild);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(hBrush);
	return (int)msg.wParam;
}
