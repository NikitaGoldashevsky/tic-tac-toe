#include <windows.h>
#include <tchar.h>

// Global variables
static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");
HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//#define OnMenuClicked 1

namespace BTN {
    constexpr unsigned short SIZE = 80;
    constexpr unsigned short SPACING = 5;
}

namespace MAINWND {
    constexpr unsigned short TOP = 20;
    constexpr unsigned short MARGIN = 10;
    constexpr unsigned short HEIGHT = BTN::SIZE * 3 + BTN::SPACING * 2 + MARGIN * 2 + 20;
    constexpr unsigned short WIDTH = BTN::SIZE * 3 + BTN::SPACING * 2 + MARGIN * 2 + 43;
}

//void WndAddMenus(HWND hWnd) {
//    HMENU RootMenu = CreateMenu();
//    AppendMenu(RootMenu, MF_STRING, OnMenuClicked, _T("My menu"));
//    SetMenu(hWnd, RootMenu);
//}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        MAINWND::HEIGHT, MAINWND::WIDTH, 
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd) {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Tic-Tac-Toe"), NULL);
        return 1;
    }

    // Creating buttons
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            CreateWindowW(
                L"BUTTON",  // Predefined class; Unicode assumed 
                L"",      // Button text 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles 
                MAINWND::MARGIN + i * (BTN::SIZE + BTN::SPACING),         // x position 
                MAINWND::MARGIN + j * (BTN::SIZE + BTN::SPACING),         // y position 
                BTN::SIZE,  // Button width
                BTN::SIZE,  // Button height
                hWnd,       // Parent window
                NULL,       // No menu
                (HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE),
                NULL);      // Pointer not needed
        }
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        /*if (LOWORD(wParam) == OnMenuClicked) {
            MessageBox(hWnd, L"Menu has been clicked!", L"Menu works", MB_OK);
        }*/
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}