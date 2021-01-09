#include <stdio.h>

#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

#include "Main.h"

//GLOBAL VARIABLES
BOOL gGameIsRunning; //g at beginning for global
HWND gGameWindow;
GAMEBITMAP gDrawingSurface;

int __stdcall WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, INT CmdShow)
{

    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(PrevInstance);
    UNREFERENCED_PARAMETER(CmdLine);
    UNREFERENCED_PARAMETER(CmdShow);

    if (GameIsAlreadyRunning() == TRUE)
    {
        MessageBoxA(NULL, "Another instance of this program is already running!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    if (CreateMainGameWindow() != ERROR_SUCCESS)
    {
        goto Exit;
    }

    gDrawingSurface.BitMapInfo.bmiHeader.biSize = sizeof(gDrawingSurface.BitMapInfo.bmiHeader);
    gDrawingSurface.BitMapInfo.bmiHeader.biWidth = GAME_RES_WIDTH;
    gDrawingSurface.BitMapInfo.bmiHeader.biHeight = GAME_RES_HEIGHT;
    gDrawingSurface.BitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    gDrawingSurface.BitMapInfo.bmiHeader.biCompression = BI_RGB;
    gDrawingSurface.BitMapInfo.bmiHeader.biPlanes = 1;
    gDrawingSurface.Memory = VirtualAlloc(NULL, GAME_DRAWING_AREA_MEM_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (gDrawingSurface.Memory == NULL)
    {
        MessageBoxA(NULL, "Failed to allocate memory for drawing surface!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        
        goto Exit;
    }

    MSG Message = { 0 };

    gGameIsRunning = TRUE;

    while (gGameIsRunning)
    {
        while (PeekMessageA(&Message, gGameWindow, 0, 0, PM_REMOVE))
        {
            DispatchMessageA(&Message);
        }

        ProcessPlayerInput();

        RenderFrameGraphics();

        Sleep(1);
    }

Exit:

    return(0);
}


LRESULT CALLBACK MainWindowProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM WParam, _In_ LPARAM LParam)
//hwnd: handle to window, uMsg: message identifier, wParam: first message param, lParam: second msg param
{

    LRESULT Result = 0;

    switch (Message)
    {
        case WM_CLOSE:
        {
            gGameIsRunning = FALSE;
            PostQuitMessage(0);
            break;
        }


        default:
        {
            Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
        }
    }
   
    return(Result);
}


DWORD CreateMainGameWindow(void) {

    DWORD Result = ERROR_SUCCESS;

    WNDCLASSEXA WindowClass = { 0 };

    WindowClass.cbSize = sizeof(WNDCLASSEXA);

    WindowClass.style = 0;

    WindowClass.lpfnWndProc = MainWindowProc;

    WindowClass.cbClsExtra = 0;

    WindowClass.cbWndExtra = 0;

    WindowClass.hInstance = GetModuleHandleA(NULL);

    WindowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);

    WindowClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    WindowClass.lpszMenuName = NULL;

    WindowClass.lpszClassName = GAME_NAME "WINDOWCLASS";

    if (RegisterClassExA(&WindowClass) == 0)
    {
        Result = GetLastError();

        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        
        goto Exit;
    }

    gGameWindow = CreateWindowExA(WS_EX_CLIENTEDGE, WindowClass.lpszClassName, "Window Title", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, GetModuleHandleA(NULL), NULL);

    if (gGameWindow == NULL)
    {
        Result = GetLastError();

        MessageBox(NULL, "Window Creation Failed", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

Exit:

    return(Result);
}

BOOL GameIsAlreadyRunning(void)
{
    HANDLE Mutex = NULL;
    Mutex = CreateMutexA(NULL, FALSE, GAME_NAME "_GameMutex"); //Mutex = a lock that we set before using a shared resource and release after using it. When the lock is set, no other thread can access the locked region of code.

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return(TRUE);
    } 
    else
    {
        return(FALSE);
    }
}

void ProcessPlayerInput(void)
{
    SHORT EscapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);

    if (EscapeKeyIsDown)
    {
        SendMessageA(gGameWindow, WM_CLOSE, 0, 0);
    }
}

void RenderFrameGraphics()
{

}