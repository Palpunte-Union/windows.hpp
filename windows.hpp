#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT BorderLessHitTest(HWND, UINT, WPARAM, LPARAM);
int hitTestRC(int, int, boolean, boolean);


TCHAR szClassName[] = TEXT("Window");

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
    MSG msg;
    BOOL bRet;

    if (!InitApp(hCurInst))
        return false;
    if (!InitInstance(hCurInst, nCmdShow)) 
        return false;

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM InitApp(HINSTANCE hInst) {
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = (HICON)LoadImage(
        NULL, IDI_APPLICATION,
        IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    wc.hCursor = (HCURSOR)LoadImage(
        NULL, IDC_ARROW,
        IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.lpszMenuName = NULL,
    wc.lpszClassName = szClassName;
    wc.hIconSm = (HICON)LoadImage(
        NULL, IDI_APPLICATION,
        IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

    return (RegisterClassEx(&wc));
}

BOOL InitInstance(HINSTANCE hInst, int nCmdShow) {
    HWND hWnd;

    hWnd = CreateWindow(szClassName,
            TEXT("window"),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            NULL,
            hInst,
            NULL
    );
    if(!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_NCCALCSIZE:
            return 0;
        case WM_NCHITTEST:
            RECT rcClient;
            POINT poClient;
            poClient.x = LOWORD ( lp );
            poClient.y = HIWORD ( lp );
            GetClientRect ( hWnd, &rcClient );

            ScreenToClient ( hWnd, &poClient );
            LRESULT lresult = BorderLessHitTest(hWnd, msg, wp, lp);
            if (lresult == 0) {
                return CallWindowProc((WNDPROC)GetWindowLong, hWnd, msg, wp, lp);
            }
            return lresult;
    }
    return (DefWindowProc(hWnd, msg, wp, lp));
}

LRESULT BorderLessHitTest(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    int borderOffset = 10;
    int borderThickness = 4;

    POINT point;
    RECT rcWindow;

    GetCursorPos(&point);
    GetWindowRect(hWnd, &rcWindow);

    int uRow = 1, uCol = 1;
    boolean fOnResizeBorder = false, fOnFrameDrag = false;

    int topOffset = 27 == 0 ? borderThickness : 27;

    if (point.y >= rcWindow.top && point.y < rcWindow.top + topOffset + borderOffset) {
        fOnResizeBorder = (point.y < (rcWindow.top + borderThickness));
        if (!fOnResizeBorder) {
            fOnFrameDrag = (point.y <= rcWindow.top + 27 + borderOffset) && (point.x < (rcWindow.right - (150 + borderOffset + 0))) && (point.x > (rcWindow.left + 0 + borderOffset + 0));
        }
        uRow = 0;
    }
    else if (point.y < rcWindow.bottom && point.y >= rcWindow.bottom - borderThickness)
        uRow = 2;
    if (point.x >= rcWindow.left && point.x < rcWindow.left + borderThickness)
        uCol = 0;
    else if (point.x < rcWindow.right && point.x >= rcWindow.right - borderThickness)
        uCol = 2;

    return hitTestRC(uRow, uCol, fOnResizeBorder, fOnFrameDrag);
}

int hitTestRC(int Row, int Col, boolean a, boolean b) {
    switch (Row) {
        case 0:
            switch (Col) {
                case 0:
                    return HTTOPLEFT;
                case 1:
                    if (a) {
                        return HTTOP;
                    } else if (b) {
                        return HTCAPTION;
                    } else {
                        return HTNOWHERE;
                    }
                case 2:
                    return HTTOPRIGHT;
            }
        case 1:
            switch (Col) {
                case 0:
                    return HTLEFT;
                case 1:
                    return HTNOWHERE;
                case 2:
                    return HTRIGHT;
            }
        case 2:
            switch (Col) {
                case 0:
                    return HTBOTTOMLEFT;
                case 1:
                    return HTBOTTOM;
                case 2:
                    return HTBOTTOMRIGHT;
            }
    }
}