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
#include <wtypes.h>
#include <algorithm>
#include <vector>
#include <stdio.h>

#include "resource.h"
#include "ConfigHandler.h"
#include "DiacriticTranslator.h"
#include <string>
#include <random>
#include <time.h>
#include "TrainOpts.h"
#include <CommCtrl.h>
#include "Trainer.h"

using namespace std;

/** Global Variables: *****************************************************************/

static HWND     hWndTrainer;
static HWND     hWndMain;
static WNDPROC  lpfnTrainEditLowProc;
static WNDPROC  lpfnTrainOkLowProc;
static CConfigHandler* Config;
static WCHAR     sSaveLayout[KL_NAMELENGTH];
static HFONT     hTrainerFont;
static HFONT     hTrainerFontUL;
vector<wstring>  strlFailQues;
vector<wstring>  strlFailAnsw;
vector<wstring>  strlCorrQues;
vector<wstring>  strlCorrAnsw;
wstring          strCorrectText;

/** Forward declarations of functions included in this code module: *******************/

void Trainer_CloseForm    (HWND hDlg);
void Trainer_Update       (HWND hDlg);
void Trainer_PrintFormated(HWND hDlg, int bx, int by, wstring InString);
void Trainer_Init         (HINSTANCE hInstance, CConfigHandler* nConfig);
bool Trainer_Prepare      (WCHAR* Vocabulary);
void Trainer_Run          (HINSTANCE hInstance, HWND hWnd);
wstring MarkedCompare     (wstring ActReply, wstring ReqReply);
int  LongestCommonSubseq  (wstring s1, wstring s2, wstring &output);
wstring Trim              (wstring Input);
int  FindAnswer           (wstring Quest, wstring Reply);
bool Trainer_ParseLine    (wstring Input);

/** Edit-Box Message-Handler: *********************************************************/

LRESULT CALLBACK TrainEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    /** Variables                                                                     */
    static CDiacriticTranslator Diacritic(hwnd);
    /** Do the message-processing:                                                    */
    switch (message) {
    case WM_CHAR:
    case WM_DEADCHAR:
        if (wParam == VK_RETURN) {
            Trainer_Update(hWndTrainer);
            return true;
        }
        if (Diacritic.Translate(&message, &wParam, &lParam)) return 0;
        break;
    }
    return CallWindowProc(lpfnTrainEditLowProc, hwnd, message, wParam, lParam);
}

/** OK-Button Message-Handler: *********************************************************/

LRESULT CALLBACK lpfnTrainOkProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CHAR:
        if (wParam == VK_RETURN) {
            Trainer_Update(hWndTrainer);
            return true;
        }
        break;
    }
    return CallWindowProc(lpfnTrainOkLowProc, hwnd, message, wParam, lParam);
}

/** Dialog Message-Handler: ***********************************************************/

INT_PTR CALLBACK TrainerProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    /** Variables: */
    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    RECT         rcWind;
    HICON        hIcon;
    static int   iMinHeight, iMinWidth;
    static ControlLocator LocQuest, LocAns, LocProg, LocOk;
    /** Parse the message: */
    switch (message) {
    case WM_INITDIALOG:
        /** Update the icon with the application's one: */
        hIcon = (HICON)LoadImageW(GetModuleHandleW(NULL),
            MAKEINTRESOURCE(IDI_TinyVoc),
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON),
            0);
        if (hIcon) {
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        }
        /** Get the current size as minimum-size for later: */
        GetWindowRect(hDlg, &rcWind);
        iMinWidth = rcWind.right - rcWind.left;
        iMinHeight = rcWind.bottom - rcWind.top;
        /** Prepare the fonts to be used: */
        hTrainerFont = CreateFont(Config->iFontSize, 0, 0, 0,
            FW_DONTCARE,                  // nWeight
            FALSE,                        // bItalic
            FALSE,                        // bUnderline
            0,                            // cStrikeOut
            ANSI_CHARSET,                 // nCharSet
            OUT_DEFAULT_PRECIS,           // nOutPrecision
            CLIP_DEFAULT_PRECIS,          // nClipPrecision
            PROOF_QUALITY,                // nQuality
            VARIABLE_PITCH, TEXT("Consolas"));
        hTrainerFontUL = CreateFont(Config->iFontSize, 0, 0, 0,
            FW_DONTCARE,                  // nWeight
            FALSE,                        // bItalic
            TRUE ,                        // bUnderline
            0,                            // cStrikeOut
            ANSI_CHARSET,                 // nCharSet
            OUT_DEFAULT_PRECIS,           // nOutPrecision
            CLIP_DEFAULT_PRECIS,          // nClipPrecision
            PROOF_QUALITY,                // nQuality
            VARIABLE_PITCH, TEXT("Consolas"));
        /** Assign it to our controls: */
        SendMessage(GetDlgItem(hDlg, IDC_QUESTION), WM_SETFONT, (WPARAM)hTrainerFont, MAKELPARAM(TRUE, 0));
        SendMessage(GetDlgItem(hDlg, IDC_ANSWER  ), WM_SETFONT, (WPARAM)hTrainerFont, MAKELPARAM(TRUE, 0));
        /** Overwrite its message-procedure and conserve the low-level one:               */
        lpfnTrainEditLowProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hDlg, IDC_ANSWER), GWLP_WNDPROC, (LONG_PTR)TrainEditProc);
        lpfnTrainOkLowProc   = (WNDPROC)SetWindowLongPtr(GetDlgItem(hDlg, IDOK), GWLP_WNDPROC, (LONG_PTR)lpfnTrainOkProc);
        /** Set the color of the progress-bar: */
        SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETBARCOLOR, 0, RGB(60, 180, 70));
        /** Initialize the control-locators for later: */
        LocQuest.GetLocalCoordinates(GetDlgItem(hDlg, IDC_QUESTION));
        LocAns.GetLocalCoordinates(GetDlgItem(hDlg, IDC_ANSWER));
        LocProg.GetLocalCoordinates(GetDlgItem(hDlg, IDC_PROGRESS));
        LocOk.GetLocalCoordinates(GetDlgItem(hDlg, IDOK));
        /** And exit: */
        SetFocus(GetDlgItem(hDlg, IDC_ANSWER));
        return (INT_PTR)FALSE;
    case WM_GETMINMAXINFO:
        /** Set the minimum size based as reply via lpMMI in lParam:                  */
        lpMMI->ptMinTrackSize.x = iMinWidth;
        lpMMI->ptMinTrackSize.y = iMinHeight;
        break;
    case WM_SIZE:
        /** When the window is resized, do so with the controls:                      */
        LocQuest.LeftAlign(lParam);
        LocAns.LeftAlign(lParam);
        LocProg.LeftAlign(lParam);
        LocOk.RightAlign(lParam);
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            Trainer_Update(hDlg);
            return (INT_PTR)TRUE;
        case IDCANCEL:
            Trainer_CloseForm(hDlg);
            return (INT_PTR)TRUE;
        }
        break;
    case WM_PAINT:
        if (strCorrectText != L"") Trainer_PrintFormated(hDlg, 13, 69, strCorrectText);
        break;
    case WM_DESTROY:
        DeleteObject(hTrainerFont);
        DeleteObject(hTrainerFontUL);
    }
    return (INT_PTR)FALSE;
}

/** Dialog-Functions: *****************************************************************/

void Trainer_CloseForm(HWND hDlg) {
    /** Save the current keyboard-layout in the config: */
    GetKeyboardLayoutName(Config->sLocale);
    /** Restore the old keyboard-layout from before: */
    SendMessageW(hDlg, WM_INPUTLANGCHANGEREQUEST, 0,
                 reinterpret_cast<LPARAM>(LoadKeyboardLayout(sSaveLayout, KLF_ACTIVATE)));
    /** Move back to the main window: */
    ShowWindow(hWndMain, SW_SHOW);
    EndDialog(hDlg, true);
}

void Trainer_Update(HWND hDlg) {
    /** Variables: */
    WCHAR       Quest[255], Reply[255];
    int         r, n;
    static int  PairCount = 0;
    /** Handle according to the current situation: */
    if (strCorrectText != L"") {
        /** This is a correction-round, which needs to be finished: */
        strCorrectText = L"";
        SendMessage(GetDlgItem(hDlg, IDC_ANSWER), EM_SETREADONLY, false, 0);
        InvalidateRect(hDlg, NULL, TRUE);
    } else {
        /** Fetch the strings from the current question: */
        GetWindowText(GetDlgItem(hDlg, IDC_QUESTION), Quest, 255);
        GetWindowText(GetDlgItem(hDlg, IDC_ANSWER), Reply, 255);
        /** Check, whether the combination does exist: */
        n = FindAnswer(wstring(Quest), wstring(Reply));
        if (n > -1) {
            /** The combination was correct:                                */
            strlCorrQues.erase(strlCorrQues.begin() + n);
            strlCorrAnsw.erase(strlCorrAnsw.begin() + n);
        } else {
            /** It was not, so trigger a correction-round: */
            strCorrectText = MarkedCompare(wstring(Reply), strlCorrAnsw[0]);
            strlFailAnsw.push_back(strlCorrAnsw[0]);
            strlFailQues.push_back(strlCorrQues[0]);
            strlCorrQues.erase(strlCorrQues.begin());
            strlCorrAnsw.erase(strlCorrAnsw.begin());
            SetFocus(GetDlgItem(hDlg, IDOK));
            SendMessage(GetDlgItem(hDlg, IDC_ANSWER), EM_SETREADONLY, true, 0);
            InvalidateRect(hDlg, NULL, TRUE);
            return;
        }
    }
    if (strlCorrQues.size() == 0) {
        /** A run was finished: */
        if (strlFailQues.size() == 0) {
            /** And there were no failures: */
            SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETPOS, PairCount, 0);
            MessageBox(hDlg, L"Congraturlation.\nYou did it!", L"TinyVoc Trainer", MB_OK | MB_ICONINFORMATION);
            SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
            return;
        } else {
            /** There were filures, so prepare the next round: */
            while (strlFailQues.size() > 0) {
                r = (rand() % strlFailQues.size());
                strlCorrQues.push_back(strlFailQues[r]);
                strlCorrAnsw.push_back(strlFailAnsw[r]);
                strlFailQues.erase(strlFailQues.begin() + r);
                strlFailAnsw.erase(strlFailAnsw.begin() + r);
            }
            PairCount = (int) strlCorrQues.size();
            SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM (0, PairCount));
        }
    }
    /** When we get here, update the progress-bar: */
    n = PairCount - (int) strlCorrQues.size();
    n = (int) SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETPOS, n, 0);
    /* ... and give the user the next question: */
    SetWindowText(GetDlgItem(hDlg, IDC_QUESTION), strlCorrQues[0].c_str());
    SetWindowText(GetDlgItem(hDlg, IDC_ANSWER), L"");
    SetFocus(GetDlgItem(hDlg, IDC_ANSWER));
}

void Trainer_PrintFormated(HWND hDlg, int bx, int by, wstring InString) {
    /** Variables: */
    PAINTSTRUCT ps;
    HDC         hdc;
    SIZE        size;
    HFONT       hTrainerOldFont;
    wstring     OutString;
    bool        bUnderline = false;
    int         iCurrX = bx;
    int         pos;
    /** Initialize the canvas: */
    hdc = BeginPaint(hDlg, &ps);
    SetTextColor(hdc, RGB(255, 151, 47));
    hTrainerOldFont = (HFONT)SelectObject(hdc, hTrainerFont);
    /** Run through the markers: */
    pos = (int) InString.find(L"▒");
    while (pos != string::npos) {
        /** Output the current part: */
        if (pos > 0) {
            OutString = InString.substr(0, pos);
            TextOut(hdc, iCurrX, by, OutString.c_str(), (int) OutString.length());
            GetTextExtentPoint32(hdc, OutString.c_str(), (int) OutString.length(), &size);
            iCurrX += size.cx;
        }
        /** Cut-Out the handled part: */
        if (pos == InString.length()) {
            InString = L"";
        } else {
            InString = InString.substr(pos + 1);
        }
        /** Switch the underlining: */
        if (bUnderline){
            bUnderline = false;
            (HFONT)SelectObject(hdc, hTrainerFont);
        } else {
            bUnderline = true;
            (HFONT)SelectObject(hdc, hTrainerFontUL);
        }
        /** Try to find the next marker: */
        pos = (int) InString.find(L"▒");
    }
    /** If there is something left, write it out: */
    if (InString.length() > 0) {
        OutString = InString.substr(0, pos);
        TextOut(hdc, iCurrX, by, OutString.c_str(), (int) OutString.length());
    }    
    /** Release the canvas: */
    SelectObject(hdc, hTrainerOldFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    EndPaint(hDlg, &ps);
}

/** Interface-Functions: **************************************************************/

void Trainer_Init(HINSTANCE hInstance, CConfigHandler* nConfig) {
    srand((unsigned int)time(NULL));
    Config = nConfig;
    hWndTrainer = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TRAINER), NULL, TrainerProc);
}

bool Trainer_Prepare(WCHAR* Vocabulary) {
    int r;
    /** Clear the vocabulary-lists: */
    strlFailQues.clear();
    strlFailAnsw.clear();
    strlCorrQues.clear();
    strlCorrAnsw.clear();
    strCorrectText = L" ";
    /** Push line by line through the parser: */
    WCHAR *p, *temp = NULL;
    p = wcstok_s(Vocabulary, L"\r\n", &temp);
    do {
        if (!Trainer_ParseLine(wstring(p))) return false;
    } while ((p = wcstok_s(NULL, L"\r\n", &temp)) != NULL);
    /** Check, whether something was found: */
    if (strlFailQues.size() == 0) return false;
    /** Reduce the amount in the created lists, if required: */
    if (Config->iSelection == OPT_SELECT_SUB) {
        while ((int) strlFailQues.size() > Config->iSubCount) {
            r = (rand() % strlFailQues.size());
            strlFailQues.erase(strlFailQues.begin() + r);
            strlFailAnsw.erase(strlFailAnsw.begin() + r);
        }
    }
    return true;
}

void Trainer_Run(HINSTANCE hInstance, HWND hWnd) {
    /** Save and update the current keyboard layout: */
    GetKeyboardLayoutName(sSaveLayout);
    SendMessageW(hWnd, WM_INPUTLANGCHANGEREQUEST, 0,
                 reinterpret_cast<LPARAM>(LoadKeyboardLayout(Config->sLocale, KLF_ACTIVATE)));
    /** Save the main window-handle and show the trainer: */
    hWndMain = hWnd;
    ShowWindow(hWndTrainer, true);
    /** Trigger a first update to initialize everything: */
    Trainer_Update(hWndTrainer);
}

/** Support-Functions: ****************************************************************/

wstring MarkedCompare(wstring ActReply, wstring ReqReply) {
    /** Variables: */
    wstring SubSeq    = L"";
    wstring Outstring = L"";
    bool    CorrChar  = true;
    int     i = 0, j  = 0;
    /** Check the general situation: */
    if (ActReply.length() == 0) {
        /** First special case, in which the user did not enter anything at all: */
        Outstring = L"▒" + ReqReply;
    } else if ((ActReply.length() > ReqReply.length()) && (ActReply.substr(0, ReqReply.length())==ReqReply)) {
        /** The second case, in which the user had too many letters at the end.
         ** This cannot be shown within the correct answer: */
        i = (int) ReqReply.length() - 1;
        Outstring = ReqReply.substr(0, i) + L"▒" + ReqReply.substr(i);
    } else {
        /** Some mistake, which can be shown in the correct answer.
         ** Thus, check for the longest common subsequence: */
        LongestCommonSubseq(ActReply, ReqReply, SubSeq);
        /** And walk through the expected characters to mark the incorrect ones: */
        for (i=0; i<ReqReply.length(); i++) {
            if (ReqReply[i] == SubSeq[j]) {
                j++;
                if (!CorrChar) {
                    Outstring += L"▒";
                    CorrChar   = true;
                }
            }else{
                if (CorrChar) {
                    Outstring += L"▒";
                    CorrChar   = false;
                }
            }
            Outstring += ReqReply[i];
        }
    }
    /** And exit:*/
    return (Outstring);
}

#define MAX(a, b) (a > b ? a : b)

int LongestCommonSubseq  (wstring s1, wstring s2, wstring &output) {
    int i, j, k, t;
    int s1Len = s1.size();
    int s2Len = s2.size();
    int *z  = (int* )calloc((s1Len + 1) * (s2Len + 1), sizeof(int));
    int **c = (int**)calloc((s1Len + 1), sizeof(int*));

    for (i = 0; i <= s1Len; ++i) {
        c[i] = &z[i * (s2Len + 1)];
    }

    for (i = 1; i <= s1Len; ++i) {
        for (j = 1; j <= s2Len; ++j) {
            if (s1[i - 1] == s2[j - 1])
                c[i][j] = c[i - 1][j - 1] + 1;
            else
                c[i][j] = MAX(c[i - 1][j], c[i][j - 1]);
        }
    }

    t = c[s1Len][s2Len];
    output = wstring(t, ' ');

    for (i = s1Len, j = s2Len, k = t - 1; k >= 0;) {
        if (s1[i - 1] == s2[j - 1])
            output[k] = s1[i - 1], i--, j--, k--;
        else if (c[i][j - 1] > c[i - 1][j])
            --j;
        else
            --i;
    }

    free(c);
    free(z);
    return t;
}

wstring Trim(wstring Input) {
    /** Trim left side: */
    while ((Input.length() > 0) && (Input.front() == L' ')) {
        Input = Input.substr(1);
    }
    /** Trim right-side: */
    while ((Input.length() > 0) && (Input.back() == L' ')) {
        Input.pop_back();
    }
    return Input;
}

int FindAnswer(wstring Quest, wstring Reply) {
    int i = 0;
    bool found = false;
    if (Config->iIgnCase) {
        std::transform(Reply.begin(), Reply.end(), Reply.begin(), ::tolower);
        std::transform(Quest.begin(), Quest.end(), Quest.begin(), ::tolower);
    }
    while ((! found) && (i < (int) strlCorrQues.size())) {
        found = ((strlCorrQues[i] == Quest) && (strlCorrAnsw[i] == Reply));
        if (found) return (i);
        i++;
    }
    return (-1);
}

bool Trainer_ParseLine(wstring Input) {
    size_t pos;
    wstring newQuestion, newAnswer;
    /** Replace any tab-stop by a space: */
    pos = Input.find(L"\t");
    while (pos != string::npos) {
        Input = Input.replace(pos, 1, L" ");
        pos = Input.find(L"\t");
    }
    /** First trim-round: */
    Input = Trim(Input);
    /** When nothing was left, this was an empty line to be ignored: */
    if (Input.length() == 0) return true;
    /** Find the first equal-sign as cutting-point: */
    pos = Input.find(L"=");
    if ((pos == string::npos) || (pos == 0)) return false;
    /** Cut out the two parts: */
    newQuestion = Trim(Input.substr(0, pos - 1));
    newAnswer = Trim(Input.substr(pos + 1));
    /** Check, that there are two valid strings without an additional equal-sign: */
    if ((newQuestion.length() == 0) || (newAnswer.length() == 0) || (newAnswer.find(L"=") != string::npos)) return false;
    /** And when something valid was found, attach them to the failed-list,
        which will be used for initialization: */
    strlFailQues.push_back(newQuestion);
    strlFailAnsw.push_back(newAnswer);
    return true;
}

/** Support functions on the control-locator-class: ***********************************/

void ControlLocator::GetLocalCoordinates(HWND hWnd) {
    RECT Rect, ClientRect;
    /** Fetch client-area: */
    GetClientRect(GetParent(hWnd), &ClientRect);
    /** Get local position of control: */
    GetWindowRect(hWnd, &Rect);
    MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&Rect, 2);
    /** Calculate and store properties: */
    this->hWnd = hWnd;
    this->left = Rect.left;
    this->top = Rect.top;
    this->height = (Rect.bottom - Rect.top);
    this->width = (Rect.right - Rect.left);
    this->dist_r = (ClientRect.right - ClientRect.left) - (Rect.right - Rect.left);
    this->dist_l = (ClientRect.right - ClientRect.left) - Rect.left;
}

void ControlLocator::LeftAlign(LPARAM lParam) {
    MoveWindow(this->hWnd, this->left, this->top, LOWORD(lParam) - this->dist_r, this->height, TRUE);
}

void ControlLocator::RightAlign(LPARAM lParam) {
    MoveWindow(this->hWnd, LOWORD(lParam) - this->dist_l, this->top, this->width, this->height, TRUE);
}
