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
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

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

   // Create a button
   HWND hButton = CreateWindowW(
       L"BUTTON",                         // Predefined class; Unicode assumed 
       L"Select folder",                       // Button text 
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
       10,                                // x position 
       10,                                // y position 
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

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
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
    MessageBox(NULL, szFiles, L"Files in Folder", MB_OK);

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
void OpenFolderDialog(HWND hWnd)
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

        // pick this from user in future
        const wchar_t* fileExtToDelete = L".xlsx";

        vector<wstring> filesToDelete = ListFiles(szPath, fileExtToDelete);

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
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
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
            OpenFolderDialog(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
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