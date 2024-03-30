// FileCleaner.cpp : Defines the entry point for the application.
//
#include<iostream>
#include "framework.h"
#include "FileCleaner.h"
#include <windows.h>
#include <vector>
#include <string> // Include the string header file
#include <shlobj.h> // Include for Shell functions
#include <strsafe.h>
#pragma comment(lib, "strsafe.lib")
#include <Shlwapi.h> // For PathCombine
#pragma comment(lib, "Shlwapi.lib") // Link against the Shlwapi librar

using namespace std;
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hEditBox;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HBITMAP hBackgroundImage;

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FILECLEANER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);



    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FILECLEANER));



    MSG msg;

    // Main message loop:
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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILECLEANER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FILECLEANER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Load the background image
    hBackgroundImage = (HBITMAP)LoadImage(NULL, L"C:\\wallpaper\\abcd.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // Register the window class
    const wchar_t CLASS_NAME[] = L"MyWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    if (hWnd == NULL)
    {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    // Calculate the position to center the combobox
    RECT rect;
    GetClientRect(hWnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    int comboBoxWidth = 200; // Adjust as needed
    int comboBoxHeight = 100; // Adjust as needed
    int x = (windowWidth - comboBoxWidth) / 2;
    int y = (windowHeight - comboBoxHeight) / 2;

    // Create the drop-down list
    HWND hComboBox = CreateWindowEx(
        0,                              // Optional window styles
        L"COMBOBOX",                    // Control class name
        NULL,                           // Text to display (will be set later)
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS, // Styles
        x, y,                         // Position
        comboBoxWidth, comboBoxHeight,                       // Size
        hWnd,                           // Parent window
        (HMENU)IDC_COMBO_FILETYPE,                           // Control identifier
        hInstance,                      // Instance handle
        NULL                            // Pointer not needed
    );

    if (hComboBox == NULL)
    {
        return 0;
    }

    // Add items to the drop-down list
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L".temp");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L".txt");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L".xlsx");

    // Set the default selection
    SendMessage(hComboBox, CB_SETCURSEL, 0, 0);

    // Show the window
    ShowWindow(hWnd, nCmdShow);

    // Create a button
    HWND hButton = CreateWindowW(
        L"BUTTON",                         // Predefined class; Unicode assumed 
        L"Select folder",                       // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x + comboBoxWidth + 10,                                // x position 
        y,                                // y position 
        100,                               // Button width
        30,                                // Button height
        hWnd,                              // Parent window
        (HMENU)IDM_OPEN_FOLDER,                              // No menu
        hInstance,                         // Instance handle
        NULL);                             // Additional application data

    if (!hButton)
    {
        return FALSE;
    }

    UpdateWindow(hWnd);
    return TRUE;
}

vector<wstring> ListFiles(const TCHAR* folderPath, const wchar_t* fileExt)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    TCHAR szFullPattern[MAX_PATH];
    TCHAR szFiles[MAX_PATH * 100]; // Allocate space for multiple file paths

    // Initialize szFiles to an empty string
    szFiles[0] = '\0';

    // Set the search path
    StringCchPrintf(szFullPattern, MAX_PATH, L"%s\\*", folderPath);

    // Start searching for files
    hFind = FindFirstFile(szFullPattern, &findFileData);
    vector<wstring> filesToDelete;
    if (hFind != INVALID_HANDLE_VALUE)
    {
    
        do
        {
            // Skip directories
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // Check if the file name contains a common temporary file pattern
                if (wcsstr(findFileData.cFileName, fileExt) == NULL)
                {
                    continue;
                }

                wchar_t szFilePath[MAX_PATH];
                PathCombine(szFilePath, folderPath, findFileData.cFileName);
                filesToDelete.push_back(szFilePath);

                StringCchCat(szFiles, MAX_PATH * 100, findFileData.cFileName);
                StringCchCat(szFiles, MAX_PATH * 100, L"\n");
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
  
    }

    // Display the concatenated file paths in a single message box
    MessageBox(NULL, szFiles, L"Files in the selected folder:", MB_OK);

    return filesToDelete;
}

DWORD DeleteFiles(const vector<wstring>& filesToDelete)
{
    DWORD totalSizeInKB = 0;

    for (const auto& filePath : filesToDelete)
    {
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        if (GetFileAttributesEx(filePath.c_str(), GetFileExInfoStandard, &fileInfo))
        {
            totalSizeInKB += (fileInfo.nFileSizeLow + fileInfo.nFileSizeHigh * (MAXDWORD + 1)) / 1024;

            if (!DeleteFile(filePath.c_str()))
            {
                wcerr << L"Failed to delete file: " << filePath << L", error code: " << GetLastError() << endl;
            }
            else
            {
                wcout << L"Deleted file: " << filePath << endl;
            }
        }
        else
        {
            wcerr << L"Failed to get file attributes for: " << filePath << L", error code: " << GetLastError() << endl;
        }
    }

    return totalSizeInKB;
}

void ShowFinalDialog(DWORD totalSizeFreedKB)
{
    wstring message = L"Files deleted successfully.\n\n";
    message += L"Total space freed up: " + to_wstring(totalSizeFreedKB) + L" KB";

    MessageBox(NULL, message.c_str(), L"Deletion Summary", MB_OK | MB_ICONINFORMATION);
}


// Function to open folder dialog
void OpenFolderDialog(HWND hWnd, wchar_t* fileExt)
{
    BROWSEINFO browseInfo = { 0 };
    browseInfo.hwndOwner = hWnd;
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo);

    TCHAR szPath[MAX_PATH];
    if (pidl != NULL)
    {
        SHGetPathFromIDList(pidl, szPath);
        MessageBox(hWnd, szPath, L"Selected Folder", MB_OK);

        vector<wstring> filesToDelete = ListFiles(szPath, fileExt);

        int result = MessageBox(hWnd, L"Are you sure you want to delete these files?", L"Confirmation", MB_YESNO | MB_ICONQUESTION);

        if (result == IDYES) {
            // Delete the temp files
            DWORD spaceFreeInKB = DeleteFiles(filesToDelete);
            ShowFinalDialog(spaceFreeInKB);
        }

        CoTaskMemFree(pidl);
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int result = 0; // Define result variable and initialize it to 0
    TCHAR szPath[MAX_PATH]; // Declare szPath variable
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Draw the background image
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hBackgroundImage);

        BITMAP bm;
        GetObject(hBackgroundImage, sizeof(bm), &bm);

        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_OPEN_FOLDER: // Add a new case for opening folder dialog
            if (wmEvent == BN_CLICKED) // Check for button click notification
            {
                // Get the selected item from the combo box
                HWND hComboBox = GetDlgItem(hWnd, IDC_COMBO_FILETYPE); // Assuming IDC_COMBO_FILETYPE is the ID of your combo box
                if (hComboBox != NULL) // Check if combo box handle is valid
                {
                    int index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                    wchar_t buffer[256]; // Adjust size as needed
                    SendMessage(hComboBox, CB_GETLBTEXT, index, (LPARAM)buffer);
                    OpenFolderDialog(hWnd, buffer);
                }
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
     break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



// Message handler for about box.
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
