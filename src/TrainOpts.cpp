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
#include <stdio.h>

#include "ConfigHandler.h"
#include "TrainOpts.h"
#include "resource.h"

/** Global Variables: *****************************************************************/

volatile bool   bReturnStatus;
static CConfigHandler* Config;

/** Forward Declarations: *************************************************************/

void TrainOpts_Init     (HWND hDlg);
void TrainOpts_OkClick  (HWND hDlg);
bool TrainOpts_ShowModal(HINSTANCE hInstance, HWND hWnd, CConfigHandler* nConfig);

/** Message-handler for the window: ***************************************************/

INT_PTR CALLBACK TrainOptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    /** Variables and compiler-switches:                                              */
    UNREFERENCED_PARAMETER(lParam);
    /** Handle the incoming message:                                                  */
    switch (message) {
    case WM_INITDIALOG:
        TrainOpts_Init(hDlg);
        return (INT_PTR)FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            bReturnStatus = true;
            TrainOpts_OkClick(hDlg);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDCANCEL:
            bReturnStatus = false;
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDC_ALL:
            EnableWindow(GetDlgItem(hDlg, IDC_SUBCOUNT), 0);
            break;
        case IDC_SELECTION:
            EnableWindow(GetDlgItem(hDlg, IDC_SUBCOUNT), 0);
            break;
        case IDC_SUBSET:
            EnableWindow(GetDlgItem(hDlg, IDC_SUBCOUNT), 1);
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

/** Event-Handlers: *******************************************************************/

void TrainOpts_Init(HWND hDlg) {
    /** Variables: */
    WCHAR buf[10];
    HICON hIcon;
    /** Load the icon and assign it to tis window:                                    */
    hIcon = (HICON)LoadImageW(GetModuleHandleW(NULL),
        MAKEINTRESOURCE(IDI_TinyVoc),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON), 0);
    if (hIcon) {
        SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }
    /** Set the ignore-case flag:                                                     */
    if (Config->iIgnCase) {
        SendMessage(GetDlgItem(hDlg, IDC_IGNORECASE), BM_SETCHECK, 1, 0);
    } else {
        SendMessage(GetDlgItem(hDlg, IDC_IGNORECASE), BM_SETCHECK, 0, 0);
    }
    /** Set the subset-counter value:                                                 */
    _itow_s(Config->iSubCount, buf, 10, 10);
    SetWindowText(GetDlgItem(hDlg, IDC_SUBCOUNT), buf);
    /** Set the range-selection:                                                      */
    switch (Config->iSelection) {
    case OPT_SELECT_ALL:
        SendMessage(GetDlgItem(hDlg, IDC_ALL), BM_SETCHECK, 1, 0);
        EnableWindow(GetDlgItem(hDlg, IDC_SUBCOUNT), 0);
        SetFocus(GetDlgItem(hDlg, IDC_ALL));
        break;
    case OPT_SELECT_SEL:
        SendMessage(GetDlgItem(hDlg, IDC_SELECTION), BM_SETCHECK, 1, 0);
        EnableWindow(GetDlgItem(hDlg, IDC_SUBCOUNT), 0);
        SetFocus(GetDlgItem(hDlg, IDC_SELECTION));
        break;
    case OPT_SELECT_SUB:
        SendMessage(GetDlgItem(hDlg, IDC_SUBSET), BM_SETCHECK, 1, 0);
        SendMessage(GetDlgItem(hDlg, IDC_SUBCOUNT), EM_SETSEL, 0, 10);
        EnableWindow(GetDlgItem(hDlg, IDC_SUBCOUNT), 1);
        SetFocus(GetDlgItem(hDlg, IDC_SUBCOUNT));
        break;
    }
}

void TrainOpts_OkClick(HWND hDlg) {
    /** Variables: */
    WCHAR buf[10];
    /** Fetch the subset-counter value: */
    GetWindowText(GetDlgItem(hDlg, IDC_SUBCOUNT), buf, 10);
    Config->iSubCount = _wtoi(buf);
    /** Fetch the ignore-case flag: */
    if (SendMessage(GetDlgItem(hDlg, IDC_IGNORECASE), BM_GETCHECK, 0, 0)) {
        Config->iIgnCase = 1;
    } else {
        Config->iIgnCase = 0;
    }
    /** Fetch the selection: */
    if (SendMessage(GetDlgItem(hDlg, IDC_SUBSET), BM_GETCHECK, 0, 0)) {
        Config->iSelection = OPT_SELECT_SUB;
    } else if (SendMessage(GetDlgItem(hDlg, IDC_SELECTION), BM_GETCHECK, 0, 0)) {
        Config->iSelection = OPT_SELECT_SEL;
    } else {
        Config->iSelection = OPT_SELECT_ALL;
    }
}

bool TrainOpts_ShowModal(HINSTANCE hInstance, HWND hWnd, CConfigHandler* nConfig) {
    Config = nConfig;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_TRAINSET), hWnd, TrainOptProc);
    return bReturnStatus;
}
