#include <windows.h>
#include <tchar.h>

#include "Game.h"
#include <string>

// Global variables
static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Tic-Tac-Toe");
static TCHAR endgameMessageBoxTitle[] = _T("Game Ended");
HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

constexpr auto ENDGAME_SLEEP_TIME = 250;

#define GAME Game::GetGame()

// Buttons' ids
constexpr auto ID_NEW_GAME = 1;

constexpr auto DIFF_ID_OFFSET = 200;
constexpr auto ID_DIFF_EASY = 1 + DIFF_ID_OFFSET;
constexpr auto ID_DIFF_NORMAL = 2 + DIFF_ID_OFFSET;
constexpr auto ID_DIFF_HARD = 3 + DIFF_ID_OFFSET;

constexpr auto FIELD_ID_OFFSET = 300;
constexpr auto ID_FIELD_1 = 1 + FIELD_ID_OFFSET;
constexpr auto ID_FIELD_2 = 2 + FIELD_ID_OFFSET;
constexpr auto ID_FIELD_3 = 3 + FIELD_ID_OFFSET;

constexpr auto BUTTON_ID_OFFSET = 100;
auto GetButtonId(const int row, const int col) {
    return BUTTON_ID_OFFSET + row * GAME.ColN() + col;
}

// Button handles vector
std::vector<HWND> cellButtonHandles;

namespace BTN {
    constexpr unsigned short SIZE = 85;
    constexpr unsigned short SPACING = 5;
}

namespace MAINWND {
    constexpr unsigned short TOP = 63;
    constexpr unsigned short LEFT = 20;
    constexpr unsigned short MARGIN = 10;

    inline unsigned short HEIGHT() {
        return (BTN::SIZE * GAME.RowN() + BTN::SPACING * (GAME.RowN() - 1) +
            MARGIN * 2 + TOP);
    }
    inline unsigned short WIDTH() {
        return (BTN::SIZE * GAME.ColN() + BTN::SPACING * (GAME.ColN() - 1) +
            MARGIN * 2 + LEFT);
    }

    inline unsigned short POSX() {
        return (GetSystemMetrics(SM_CXSCREEN) - WIDTH()) / 2;
    }
    inline unsigned short POSY() {
        return (GetSystemMetrics(SM_CYSCREEN) - HEIGHT()) / 2;
    }
}

inline void WndMenusDiffUpdateChecked(HWND hWnd) {
    HMENU hMenu = GetMenu(hWnd);
    HMENU hDiffSubMenu = GetSubMenu(hMenu, 1);

    CheckMenuItem(hDiffSubMenu, ID_DIFF_EASY, MF_UNCHECKED);
    CheckMenuItem(hDiffSubMenu, ID_DIFF_NORMAL, MF_UNCHECKED);
    CheckMenuItem(hDiffSubMenu, ID_DIFF_HARD, MF_UNCHECKED);

    CheckMenuItem(hDiffSubMenu, GAME.GetAIDiff() + 1 + DIFF_ID_OFFSET, MF_CHECKED);
}

inline void WndMenusFieldUpdateChecked(HWND hWnd) {
    HMENU hMenu = GetMenu(hWnd);
    HMENU hFieldSubMenu = GetSubMenu(hMenu, 2);

    int optionId = 0;
    if (GAME.RowN() == 3 && GAME.ColN() == 3) {
        optionId = 1;
    }
    else if (GAME.ColN() == 4 && GAME.RowN() == 4) {
        optionId = 2;
    }
    else if (GAME.ColN() == 5 && GAME.RowN() == 5) {
        optionId = 3;
    }

    CheckMenuItem(hFieldSubMenu, ID_FIELD_1, MF_UNCHECKED);
    CheckMenuItem(hFieldSubMenu, ID_FIELD_2, MF_UNCHECKED);
    CheckMenuItem(hFieldSubMenu, ID_FIELD_3, MF_UNCHECKED);

    CheckMenuItem(hFieldSubMenu, optionId + FIELD_ID_OFFSET, MF_CHECKED);
}

void WndAddMenus(HWND hWnd) {
    HMENU rootMenu = CreateMenu();
    AppendMenu(rootMenu, MF_STRING, ID_NEW_GAME, _T("New game"));

    HMENU hDiffSubMenu = CreatePopupMenu();
    AppendMenu(hDiffSubMenu, MF_STRING, ID_DIFF_EASY, L"Easy");
    AppendMenu(hDiffSubMenu, MF_STRING, ID_DIFF_NORMAL, L"Normal");
    AppendMenu(hDiffSubMenu, MF_STRING, ID_DIFF_HARD, L"Hard");
    AppendMenu(rootMenu, MF_POPUP, (UINT_PTR)hDiffSubMenu, L"Difficulty");

    HMENU hFieldSubMenu = CreatePopupMenu();
    AppendMenu(hFieldSubMenu, MF_STRING, ID_FIELD_1, L"3 x 3");
    AppendMenu(hFieldSubMenu, MF_STRING, ID_FIELD_2, L"4 x 4");
    AppendMenu(hFieldSubMenu, MF_STRING, ID_FIELD_3, L"5 x 5");
    AppendMenu(rootMenu, MF_POPUP, (UINT_PTR)hFieldSubMenu, L"Field");

    SetMenu(hWnd, rootMenu);
    WndMenusDiffUpdateChecked(hWnd);
    WndMenusFieldUpdateChecked(hWnd);
}

void WndUpdateCells(HWND hWnd) {
    for (int i = 0; i < GAME.RowN(); i++) {
        for (int j = 0; j < GAME.ColN(); j++) {
            const int buttonId = GetButtonId(i, j);
            std::wstring buttonText(1, GAME[i][j]);
            SetDlgItemTextW(hWnd, buttonId, buttonText.c_str());
        }
    }
}

void WndTitleUpdateDiffPostfix(HWND hWnd) {
    TCHAR diffStr[10];

    switch (GAME.GetAIDiff())
    {
    case easy:
        _tcscpy_s(diffStr, _T("Easy"));
        break;
    case normal:
        _tcscpy_s(diffStr, _T("Normal"));
        break;
    case hard:
        _tcscpy_s(diffStr, _T("Hard"));
        break;
    }

    TCHAR newTitle[256];
    _stprintf_s(newTitle, _T("Tic-Tac-Toe - %s"), diffStr);
    SetWindowText(hWnd, newTitle);
}

void RedrawCellButtons(HWND hWnd) {
    for (auto buttonHandle : cellButtonHandles) {
        DestroyWindow(buttonHandle);
    }
    cellButtonHandles.clear();

    const short FontSize = 40;
    HFONT hCellFont = CreateFont(FontSize, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

    for (int i = 0; i < GAME.RowN(); i++) {
        for (int j = 0; j < GAME.ColN(); j++) {
            std::wstring buttonText(1, GAME[i][j]);
            int buttonId = GetButtonId(i, j);
            HWND buttonHandle = CreateWindowW(
                L"BUTTON",  // Predefined class; Unicode assumed 
                buttonText.c_str(), // Button text 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles 
                MAINWND::MARGIN + j * (BTN::SIZE + BTN::SPACING),         // x position 
                MAINWND::MARGIN + i * (BTN::SIZE + BTN::SPACING),         // y position 
                BTN::SIZE,  // Button width
                BTN::SIZE,  // Button height
                hWnd,       // Parent window
                (HMENU)buttonId,
                (HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE),
                NULL // Pointer not needed
            );
            SendMessage(buttonHandle, WM_SETFONT, (WPARAM)hCellFont, TRUE);
            
            cellButtonHandles.push_back(buttonHandle);
        }
    }

    SetWindowPos(hWnd, NULL, MAINWND::POSX(), MAINWND::POSY(), MAINWND::WIDTH(), MAINWND::HEIGHT(), SWP_SHOWWINDOW);
}

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

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int wndPosX = (screenWidth - MAINWND::WIDTH()) / 2;
    const int wndPosY = (screenHeight - MAINWND::HEIGHT()) / 2;

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        wndPosX, wndPosY,
        MAINWND::WIDTH(), MAINWND::HEIGHT(),
        NULL,
        NULL,
        hInstance,
        NULL
    );
    WndTitleUpdateDiffPostfix(hWnd);

    if (!hWnd) {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Tic-Tac-Toe"), NULL);
        return 1;
    }

    GAME.SetFieldSize(opt33);
    RedrawCellButtons(hWnd);

    WndAddMenus(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void HandleCellClick(HWND hWnd, const int _row, const int _col) {
    if (GAME[_row][_col] != ' ') return;
    if (GAME.GetGameState() != ongoing) return;
 
    // User move
    const Cell cell(_row, _col, GAME.CELL_X);

    int mbResult = 0;

    GAME.Set(cell);
    WndUpdateCells(hWnd);
    if (GAME.GetGameState(cell) == playerWin) {
        Sleep(ENDGAME_SLEEP_TIME);
        mbResult = MessageBox(hWnd, L"You won!\nStart a new game?", endgameMessageBoxTitle, MB_YESNO);
    }
    else {
        GameState gameState = GAME.MakeAIMove();
        WndUpdateCells(hWnd);
        if (gameState == aiWin) {
            Sleep(ENDGAME_SLEEP_TIME);
            mbResult = MessageBox(hWnd, L"You lost!\nStart a new game?", endgameMessageBoxTitle, MB_YESNO);
        }
        else if (gameState == tie) {
            Sleep(ENDGAME_SLEEP_TIME);
            mbResult = MessageBox(hWnd, L"Tie!\nStart a new game?", endgameMessageBoxTitle, MB_YESNO);
        }
    }

    if (mbResult == IDYES) {
        GAME.ResetField();
        WndUpdateCells(hWnd);
    }
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
    {
        const auto lw = LOWORD(wParam);
        if (DIFF_ID_OFFSET < lw && lw <= DIFF_ID_OFFSET + 3) {
            GAME.SetAIDiff(AIDiff(lw % DIFF_ID_OFFSET - 1));
            WndMenusDiffUpdateChecked(hWnd);
            GAME.ResetField();
            WndUpdateCells(hWnd);
            WndTitleUpdateDiffPostfix(hWnd);
        }
        else if (FIELD_ID_OFFSET < lw && lw <= FIELD_ID_OFFSET + 3) {
            GAME.SetFieldSize(FieldOption(lw - FIELD_ID_OFFSET - 1));
            RedrawCellButtons(hWnd);
            WndUpdateCells(hWnd);
            WndMenusFieldUpdateChecked(hWnd);
        }
        else if (lw == ID_NEW_GAME) {
            GAME.ResetField();
            WndUpdateCells(hWnd);
            //MessageBox(hWnd, L"Field has been reset!", L"New game", MB_OK);
        }
        else if (HIWORD(wParam) == BN_CLICKED) {
            int buttonId = LOWORD(wParam);
            
            const int id = buttonId - BUTTON_ID_OFFSET;
            if (id < 0 || id >= GAME.RowN() * GAME.ColN()) {
                break; // Incorrect button Id
            }

            const int row = id / GAME.ColN();  
            const int col = id % GAME.ColN();
            HandleCellClick(hWnd, row, col);
        }
        break;
    }
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