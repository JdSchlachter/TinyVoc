//
//  This file is part of TinyVoc project
//  Copyright (C)2022 Jens Daniel Schlachter <osw.schlachter@mailbox.org>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

/** Global Includes: ******************************************************************/

#include "stdafx.h"
#include <commdlg.h>
#include <string>
#include "shellapi.h"
#include "resource.h"
#include "ConfigHandler.h"
#include "DiacriticTranslator.h"
#include "TrainOpts.h"
#include "Trainer.h"

/** Compiler Settings: ****************************************************************/

#ifdef _MSC_VER
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"User32.lib"  )
#pragma comment(lib,"gdi32.lib"   )
#pragma comment(lib,"version.lib" )
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)
#else
#pragma GCC diagnostic ignored "-Wconversion-null"
#endif

/** Local Defines: ********************************************************************/

#define C_TEXTBUFSIZE 1000000

/** Global variables: *****************************************************************/

HWND            hWndMain;
HWND            hWndEdit;
WCHAR           szTitle[] = L"TinyVoc Portable";                  // The title bar text
WCHAR           szWindowClass[MAX_PATH];            // the main window class name
WCHAR           szFileName[MAX_PATH];
WNDPROC         lpfnEditBoxLowProc;
OPENFILENAME    FileDialogue;
CConfigHandler  Config;

/** Forward Declarations: *************************************************************/

HWND    CreateEditBox       (HWND hOwner, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc    (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditBoxProc(HWND, UINT, WPARAM, LPARAM);
void    About_Click     (HWND hwnd);
void    FileNew_Click   (HWND hwnd);
void    FileOpen_Click  (HWND hwnd);
void    FileOpen        (HWND hwnd, LPWSTR lpszFileName);
void    FileSaveAs_Click(HWND hwnd);
void    FileSave_Click  (HWND hwnd);
void    FileClose_Click (HWND hwnd);
void    TrainSettings_Click(HWND hwnd, HINSTANCE hInstance);
void    Train_Click     (HWND hwnd, HINSTANCE hInstance);
void    Help_Click      (HWND hwnd);
void    About_Click     (HWND hwnd);
void    CreateFileDlg   (HWND hwnd);
bool    FileOpenDlg     (HWND hwnd, PTSTR pstrFileName);
bool    FileSaveDlg     (HWND hwnd, PTSTR pstrFileName);
void    AddVersionInfo  (WCHAR* pszwOutput, const WCHAR* pszwEntry);

/** The Application's Main-Function: **************************************************/

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ PSTR      lpCmdLine, _In_ int       nCmdShow) {
    /** Variables and compiler-switches:                                              */
    WNDCLASSEXW  wcex;
    MSG          msg;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    /** Load strings as required:                                                     */
    LoadStringW(hInstance, IDC_TinyVoc, szWindowClass, MAX_PATH);
    /** Shorten the title, when this is started not portable:                         */ 
    if (!Config.bIsPortable()) {
        szTitle[5] = 0;
    }
    /** Setup the window-class:                                                       */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TinyVoc));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TinyVoc);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TinyVoc));
    /** Try to register it:                                                           */
    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, TEXT("This program requires at least Windows 2K!"), szTitle, MB_ICONERROR);
        return 0;
    }
    /** Create the window-handler:                                                    */
    hWndMain = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
        NULL, NULL, hInstance, NULL);
    /** Create the trainer-window: */
    Trainer_Init(hInstance, &Config);
    /** Load the accelerator-table from the ressources:                               */
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TinyVoc));
    /** Show the main-window:                                                         */
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);
    /** .. and run the main message-loop:                                             */
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(hWndMain, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

/** Encapsulated creator of the edit-box: *********************************************/

HWND CreateEditBox(HWND hOwner, WPARAM wParam, LPARAM lParam) {
    /** Variables:                                                                    */
    HWND   hWndEdit;
    HFONT  hFont;
    /** Create the edit-box-control:                                                  */
    hWndEdit = CreateWindow(TEXT("edit"), NULL,
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL,
        0, 0, 0, 0, hOwner, (HMENU)IDC_EDIT,
        ((LPCREATESTRUCT)lParam)->hInstance, NULL);
    /** Overwrite its message-procedure and conserve the low-level one:               */
    lpfnEditBoxLowProc = (WNDPROC)SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (LONG_PTR)EditBoxProc);
    /** Set the font of the edit-box:                                                 */
    hFont = CreateFont(Config.iFontSize, 0, 0, 0,
        FW_DONTCARE,                  // nWeight
        FALSE,                        // bItalic
        FALSE,                        // bUnderline
        0,                            // cStrikeOut
        ANSI_CHARSET,                 // nCharSet
        OUT_DEFAULT_PRECIS,           // nOutPrecision
        CLIP_DEFAULT_PRECIS,          // nClipPrecision
        PROOF_QUALITY,                // nQuality
        VARIABLE_PITCH, TEXT("Consolas"));
    SendMessage(hWndEdit,             // Handle of edit control
        WM_SETFONT,                   // Message to change the font
        (WPARAM)hFont,                // handle of the font
        MAKELPARAM(TRUE, 0));
    /** Set it to accept files: */
    DragAcceptFiles(hWndEdit, true);
    /** And be done:                                                                  */
    return hWndEdit;
}

/** Main-Window Message-Handler: ******************************************************/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    /** Variables:                                                                    */
    static HINSTANCE hInstance;                                // current instance
    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    /** Parse the window-message:                                                     */
    switch (message) {
    case WM_CREATE:
        /** Store the instance-ID:                                                    */
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        /** Create Edit-Box:                                                          */
        hWndEdit = CreateEditBox(hWnd, wParam, lParam);
        CreateFileDlg(hWnd);
        break;
    case WM_GETMINMAXINFO:
        /** Get the rectangles of window and client to calc the border-width:         *
        GetClientRect(hWnd, &rcClient);
        GetWindowRect(hWnd, &rcWind);
        /** And set the minimum size based as reply via lpMMI in lParam:              */
        lpMMI->ptMinTrackSize.x = 400;
        lpMMI->ptMinTrackSize.y = 200;
        break;
    case WM_SETFOCUS:
        /** When the window received the focus, pass it on to the edit-control:       */
        SetFocus(hWndEdit);
        return 0;
    case WM_SIZE:
        /** When the window is resized, do so with the edit-control:                  */
        MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        SendMessage(hWndEdit, EM_SCROLLCARET, 0, 0);
        return 0;
    case WM_COMMAND:
        /** Parse the menu selections:                                                */
        switch LOWORD(wParam) {
        case IDM_FILE_NEW:
            FileNew_Click(hWnd);
            break;
        case IDM_FILE_OPEN:
            FileOpen_Click(hWnd);
            break;
        case IDM_FILE_SAVE:
            FileSave_Click(hWnd);
            break;
        case IDM_FILE_SAVEAS:
            FileSaveAs_Click(hWnd);
            break;
        case IDM_TRAIN:
            TrainSettings_Click(hWnd, hInstance);
            break;
        case IDM_TRAINWITHLAST:
            Train_Click(hWnd, hInstance);
            break;
        case IDM_HELP:
            Help_Click(hWnd);
            break;
        case IDM_ABOUT:
            About_Click(hWnd);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_DESTROY:
        FileClose_Click(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/** Edit-Box Message-Handler: *********************************************************/

LRESULT CALLBACK EditBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    /** Variables                                                                     */
    static CDiacriticTranslator Diacritic(hWndEdit);
    WCHAR lpszLocalName[MAX_PATH];    
    /** Do the message-processing:                                                    */
    switch (message) {
    case WM_CREATE:
        //bTextChanged = false;
        break;
    case WM_CHAR:
    case WM_DEADCHAR:
        if (Diacritic.Translate(&message, &wParam, &lParam)) return 0;
        break;
    case WM_DROPFILES:
        /** Someone dragged a file on the edit-window, thus try to load it:           */
        DragQueryFile((HDROP)wParam, 0, lpszLocalName, sizeof(lpszLocalName)/2);
        wcscpy(szFileName, lpszLocalName);
        FileOpen(hwnd, lpszLocalName);
        DragFinish((HDROP)wParam);
        break;
    }
    return CallWindowProc(lpfnEditBoxLowProc, hwnd, message, wParam, lParam);
}

/** Event-Handlers: *******************************************************************/

void FileNew_Click(HWND hwnd) {
    SetWindowText(hWndEdit, L"");
    szFileName[0] = 0;
}

void FileOpen_Click(HWND hwnd) {
    WCHAR lpszLocalName[MAX_PATH];
    lpszLocalName[0] = 0;
    /** Try the file-open dialogue:                                                   */
    if (FileOpenDlg(hwnd, lpszLocalName)) {
        wcscpy(szFileName, lpszLocalName);
        FileOpen(hwnd, lpszLocalName);
    }
}

void FileOpen(HWND hwnd, LPWSTR lpszFileName) {
    /** Variables:                                                                    */
    WCHAR* buf = new(WCHAR[C_TEXTBUFSIZE]);
    std::wstring    sText;
    DWORD           dwIndex;
    FILE* fp;
    /** When successful, read the file line by line:                                  */
    fp = _wfopen(lpszFileName, L"r, ccs=UTF-8");
    /** And read the text line by line:                                               */
    sText = L"";
    while (fgetws(buf, MAX_PATH, fp) != NULL) {
        sText = sText + std::wstring(buf);
        /** Make sure to replace any pure\n by \r\n for windows:                      */
        if (sText.back() == L'\n') {
            sText.back() = L'\r';
            sText += L"\n";
        }
    }
    /** Write out the text:                                                           */
    SetWindowText(hWndEdit, sText.c_str());
    /** Move and trigger a scroll:                                                    */
    dwIndex = (DWORD)wcslen(sText.c_str());
    SendMessage(hWndEdit, EM_SETSEL, dwIndex, dwIndex);
    SendMessage(hWndEdit, EM_SCROLLCARET, 0, 0);
}

void FileSaveAs_Click(HWND hwnd) {
    /** Try the file-save dialogue:                                                   */
    if (FileSaveDlg(hwnd, szFileName)) {
        /** Succesful, thus the name is set and saving can be done:                   */
        FileSave_Click(hwnd);
    }
}

void FileSave_Click(HWND hwnd) {
    /** Variables:                                                                    */
    WCHAR* buf = new(WCHAR[C_TEXTBUFSIZE]);
    std::size_t  i;
    std::wstring sText;
    FILE* fp;
    /** Check, whether the file-name is defined:                                      */
    if (szFileName[0] == 0) {
        FileSaveAs_Click(hwnd);
        return;
    }
    /** Try to open the file:                                                         */
    fp = _wfopen(szFileName, L"w, ccs=UTF-8");
    if (fp == NULL) {
        wcscpy(buf, L"Unable to save to ");
        wcscat(buf, szFileName);
        wcscat(buf, L"!");
        MessageBox(hwnd, buf, szTitle, MB_OK | MB_ICONSTOP);
        return;
    }
    /** Prepare the string:                                                           */
    GetWindowText(hWndEdit, buf, C_TEXTBUFSIZE);
    sText = std::wstring(buf);
    /** Prepare the text bei removing \r:                                             */
    i = sText.find(L"\r");
    while (i != std::string::npos) {
        sText.erase(i, 1);
        i = sText.find(L"\r");
    }
#ifdef _MSC_VER
    fwprintf(fp, L"%s", sText.c_str());
#else
    fwprintf(fp, L"%S", sText.c_str());
#endif
    /** And close:                                                                    */
    fclose(fp);
}

void FileClose_Click(HWND hwnd) {
    /** Send a quit message to the application:                                       */
    PostQuitMessage(0);
}

void TrainSettings_Click(HWND hwnd, HINSTANCE hInstance) {
    /** Show the training-options dialogue:                                           */
    if (TrainOpts_ShowModal(hInstance, hWndMain, &Config)) {
        /** If it was successful, start the trainer:                                  */
        Train_Click(hwnd, hInstance);
    }
}

void Train_Click(HWND hwnd, HINSTANCE hInstance) {
    /** Variables:                                                                    */
    WCHAR* buf = new(WCHAR[C_TEXTBUFSIZE]);
    int iStart=0, iEnd=0;
    /** Fetch the string:                                                             */
    GetWindowText(hWndEdit, buf, C_TEXTBUFSIZE);
    /** If required, chop it:                                                         */
    if (Config.iSelection == OPT_SELECT_SEL) {
        SendMessage(hWndEdit, EM_GETSEL, (WPARAM)&iStart, (LPARAM)&iEnd);
        if (iStart == iEnd) {
            MessageBox(hwnd, L"No selection was done.\nSelect a subset or change the training-settings.", szTitle, MB_OK | MB_ICONASTERISK);
            return;
        }
        wcsncpy(buf, &buf[iStart], iEnd - iStart);
        buf[iEnd - iStart] = 0;
    }
    /** Try to prepare the trainer: */
    if ((wcslen(buf) < 1) || (!Trainer_Prepare(buf))) {
        MessageBox(hwnd, L"Unable to parse vocabulary!", szTitle, MB_OK | MB_ICONASTERISK);
        return;
    }
    /** And run:                                                                      */
    Trainer_Run(hInstance, hWndMain);
    ShowWindow(hWndMain, SW_HIDE);
}

void Help_Click(HWND hwnd) {
    /** Variables:                                                                    */
    HINSTANCE hInstance;
    WCHAR     szNameBuffer[MAX_PATH];
    LPWSTR    lpwszPathPointer;
    DWORD     dwAttrib;
    /** Fetch the name and file-path, by which this application was started:          */
    hInstance = GetModuleHandle(NULL);
    GetModuleFileName(hInstance, szNameBuffer, MAX_PATH);
    /** Attach th relative path to the help-file: */
    lpwszPathPointer = wcsrchr(szNameBuffer, L'\\');
    lpwszPathPointer[1] = 0;
    wcscat(szNameBuffer, L"Other\\Help\\TinyVoc.html");
    /** Check, whether it exists: */
    dwAttrib = GetFileAttributes(szNameBuffer);
    if ((dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ) {
        ShellExecute(NULL, L"open", szNameBuffer, NULL, NULL, SW_SHOW);
    }else{
       wcscpy(szNameBuffer, L"The help-file was expected in\n.\\Other\\Help\\TinyVoc.html\nbut could not be found.");
       MessageBox(hwnd, szNameBuffer, szTitle, MB_OK | MB_ICONINFORMATION);
    }    
}

void About_Click(HWND hwnd) {
    /** Variables:                                                                    */
    WCHAR* buf = new(WCHAR[C_TEXTBUFSIZE]);
    /** Build the output-string:                                                      */
    buf[0] = 0;
    AddVersionInfo(buf, L"InternalName");
    wcscat(buf, L" ");
    AddVersionInfo(buf, L"FileVersion");
    wcscat(buf, L"\r\n");
    AddVersionInfo(buf, L"LegalCopyright");
    wcscat(buf, L"\r\n\r\nThis program comes with ABSOLUTELY NO WARRANTY.\r\nIt is free software; you can redistribute it and /or modify it\r\nunder the terms of the GNU General Public License version 3,\r\nor (at your option) any later version.");
    /** And show it:                                                                  */
    MessageBox(hwnd, buf, szTitle, MB_OK | MB_ICONINFORMATION);
}

/** Support-function for the open- and save-dialogues: ********************************/

void CreateFileDlg(HWND hwnd) {
    static WCHAR szFilter[] = TEXT("Vocabulary Files (*.voc)\0*.voc\0") \
        TEXT("Text Files (*.txt)\0*.txt\0") \
        TEXT("All Files (*.*)\0*.*\0\0");
    szFileName[0] = 0;
    FileDialogue.lStructSize = sizeof(OPENFILENAME);
    FileDialogue.hwndOwner = hwnd;
    FileDialogue.hInstance = NULL;
    FileDialogue.lpstrFilter = szFilter;
    FileDialogue.lpstrCustomFilter = NULL;
    FileDialogue.nMaxCustFilter = 0;
    FileDialogue.nFilterIndex = 0;
    FileDialogue.lpstrFile = NULL; // Set in Open and Close functions
    FileDialogue.nMaxFile = MAX_PATH;
    FileDialogue.lpstrFileTitle = NULL; // Set in Open and Close functions
    FileDialogue.nMaxFileTitle = MAX_PATH;
    FileDialogue.lpstrInitialDir = NULL;
    FileDialogue.lpstrTitle = NULL;
    FileDialogue.Flags = 0; // Set in Open and Close functions
    FileDialogue.nFileOffset = 0;
    FileDialogue.nFileExtension = 0;
    FileDialogue.lpstrDefExt = TEXT("voc");
    FileDialogue.lCustData = 0L;
    FileDialogue.lpfnHook = NULL;
    FileDialogue.lpTemplateName = NULL;
}

bool FileOpenDlg(HWND hwnd, PTSTR pstrFileName) {
    FileDialogue.hwndOwner = hwnd;
    FileDialogue.lpstrFile = pstrFileName;
    FileDialogue.lpstrFileTitle = 0;
    FileDialogue.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
    return GetOpenFileName(&FileDialogue);
}

bool FileSaveDlg(HWND hwnd, PTSTR pstrFileName) {
    FileDialogue.hwndOwner = hwnd;
    FileDialogue.lpstrFile = pstrFileName;
    FileDialogue.lpstrFileTitle = 0;
    FileDialogue.Flags = OFN_OVERWRITEPROMPT;
    return GetSaveFileName(&FileDialogue);
}

/** Support-function to fetch info from the version-resource: *************************/

void AddVersionInfo(WCHAR* pszwOutput, const WCHAR* pszwEntry) {
    /** Variables:                                                                    */
    DWORD   vLen, langD;
    BOOL    retVal;
    LPVOID  retbuf = NULL;
    static  WCHAR fileEntry[256];
    /** Fetch-Code:                                                                   */
    HRSRC hVersion = FindResource(NULL, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hVersion != NULL) {
        HGLOBAL hGlobal = LoadResource(NULL, hVersion);
        if (hGlobal != NULL) {
            LPVOID versionInfo = LockResource(hGlobal);
            if (versionInfo != NULL) {
                swprintf(fileEntry, L"\\VarFileInfo\\Translation");
                retVal = VerQueryValue(versionInfo, fileEntry, &retbuf, (UINT*)&vLen);
                if (retVal && vLen == 4) {
                    memcpy(&langD, retbuf, 4);
#ifdef _MSC_VER
                    swprintf(fileEntry, L"\\StringFileInfo\\%02X%02X%02X%02X\\%s",
                        (langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24,
                        (langD & 0xff0000) >> 16, pszwEntry);
#else
                    swprintf(fileEntry, L"\\StringFileInfo\\%02X%02X%02X%02X\\%S",
                        (langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24,
                        (langD & 0xff0000) >> 16, pszwEntry);
#endif
                } else {
                    swprintf(fileEntry, L"\\StringFileInfo\\%04X04B0\\%s",
                        GetUserDefaultLangID(), pszwEntry);
                }
                if (VerQueryValue(versionInfo, fileEntry, &retbuf, (UINT*)&vLen)) {
                    wcscat(pszwOutput, (WCHAR*)retbuf);
                }
            }
        }
        UnlockResource(hGlobal);
        FreeResource(hGlobal);
    }
}
