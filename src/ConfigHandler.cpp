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
#include <winuser.h>
#include <stdio.h>
#include <Shlobj.h>
#include <string>
#include "ConfigHandler.h"

/** Compiler Settings: ****************************************************************/

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

/** Public Functions: *****************************************************************/

/** Constructor: **********************************************************************
 *    Triggers the check for portability and tries to load the settings               *
 *    from an ini-file either from the current path when running portable,            *
 *    or from the windows user's path:                                                */
 
CConfigHandler::CConfigHandler() {
    /** Variables:                                                                    */
    HINSTANCE hInstance;
    WCHAR szNameBuffer[MAX_PATH];
    LPWSTR lpwszPathPointer;
    /** Fetch the name and file-path, by which this application was started:          */
    hInstance = GetModuleHandle(NULL);
    GetModuleFileName(hInstance, szNameBuffer, MAX_PATH);
    /** Check portability:                                                            */
    vCheckPortable(szNameBuffer);
    /** Setup the file-name:                                                          */
    if (bPortable) {
        /** If it is portable, build the local path:                                  */
        lpwszPathPointer = wcsrchr(szNameBuffer, L'\\');
        lpwszPathPointer[1] = 0;
        wcscat(szNameBuffer, L"Data\\settings\\TinyVoc.ini");
    }else{
        /** If it is not, try to build the path from the user-data-path:              */
        if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, szNameBuffer) != S_OK) {
            vSetDefaultData();
            return;
        }
        wcscat(szNameBuffer, L"\\TinyVoc\\TinyVoc.ini");
    }
    if (!bReadFromFile(szNameBuffer)) vSetDefaultData();
}

/** Destructor: ***********************************************************************
 *    Tries to store the settings in an ini-file either in the current path           *
 *    when running portable, or in the windows user's path:                           */

CConfigHandler::~CConfigHandler() {
    HINSTANCE hInstance;
    WCHAR szNameBuffer[MAX_PATH];
    LPWSTR lpwszPathPointer;
    /** Setup the file-name:                                                          */
    if (bPortable) {
        //D:\PortableApps\PuTTYPortable\Data\settings
        /** If it is portable, build the local path:                                  */
        hInstance = GetModuleHandle(NULL);
        GetModuleFileName(hInstance, szNameBuffer, MAX_PATH);
        lpwszPathPointer = wcsrchr(szNameBuffer, L'\\');
        lpwszPathPointer[1] = 0;
        /** Try to enter first subdirectory: */
        wcscat(szNameBuffer, L"Data");
        if (!CreateDirectory(szNameBuffer, NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
            /** It was not successful, and not because it already existed:            */
            MessageBox(NULL, TEXT("Failure in attempt to create\nlocal data-directory!"), TEXT("TinyVoc"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        /** Try to enter second subdirectory: */
        wcscat(szNameBuffer, L"\\settings");
        if (!CreateDirectory(szNameBuffer, NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
            /** It was not successful, and not because it already existed:            */
            MessageBox(NULL, TEXT("Failure in attempt to create\nlocal settings-directory!"), TEXT("TinyVoc"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        /** Finally try to save the file: */
        wcscat(szNameBuffer, L"\\TinyVoc.ini");
        /** If it is portable, it should be right there:                              */
        if (! bWriteToFile(szNameBuffer)) {
            MessageBox(NULL, TEXT("Failure in attempt to store\nconfiguration in portable ini-file!"), TEXT("TinyVoc"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
    }else{
        /** If it is local, so fetch the user-directory:                              */
        if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, szNameBuffer) != S_OK) return;
        /** Create the directory in case it does not exist yet:                       */
        wcscat(szNameBuffer, L"\\TinyVoc");
        if (!CreateDirectory(szNameBuffer, NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
            /** It was not successful, and not because it already existed:            */
            MessageBox(NULL, TEXT("Failure in attempt to create\nsettings folder in user-directory!"), TEXT("TinyVoc"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        /** Add the actual file-name and try to save:                                 */
        wcscat(szNameBuffer, L"\\TinyVoc.ini");
        if (! bWriteToFile(szNameBuffer)) {
            MessageBox(NULL, TEXT("Failure in attempt to store\nconfiguration in user ini-file!"), TEXT("TinyVoc"), MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

/** Get-Function of the portability-flag: *********************************************
 *    This allows other classes to check, if the application runs                     *
 *    as portable:                                                                    */

bool CConfigHandler::bIsPortable(void) {
    return (bPortable);
}

/** Private Functions: ****************************************************************/

/** Check-function for portability: ***************************************************
 *    Sets the according flag to portable, when                                       *
 *     - an ini-file was found in the current directory or                            *
 *     - the drive, from which the application was started is a removable one.        */

void CConfigHandler::vCheckPortable(LPWSTR szNameBuffer) {
    /** Variables:                                                                    */
    WCHAR szEnvPath[MAX_PATH];
    /** Compare to the environment-variables for programs: */
    GetEnvironmentVariable(L"ProgramFiles", szEnvPath, MAX_PATH);
    if (wcsncmp(szNameBuffer, szEnvPath, wcslen(szEnvPath)) == 0) {
        bPortable = false;
        return;
    }
    GetEnvironmentVariable(L"ProgramFiles(x86)", szEnvPath, MAX_PATH);
    if (wcsncmp(szNameBuffer, szEnvPath, wcslen(szEnvPath)) == 0) {
        bPortable = false;
        return;
    }
    bPortable = true;
}

/** File-Writer: **********************************************************************
 *    Writes the configuration into a plain text-file:                                */

bool CConfigHandler::bWriteToFile(const WCHAR* pszwFName) {
    /** Variables:                                                                    */
    FILE *fp;
    /** Try to open the file:                                                         */ 
    fp = _wfopen(pszwFName, L"w, ccs=UTF-8");
    if (fp == NULL) return false;
    /** Write the items:                                                              */
    fwprintf(fp, L"[TinyVoc]\n");
    fwprintf(fp, L"FontSize=%d\n" , iFontSize );
    fwprintf(fp, L"Selection=%d\n", iSelection);
    fwprintf(fp, L"SubCount=%d\n" , iSubCount );
    fwprintf(fp, L"IgnCase=%d\n"  , iIgnCase  );
    #ifdef _MSC_VER
        fwprintf(fp, L"Locale=%s", sLocale);
    #else
        fwprintf(fp, L"Locale=%S", sLocale);
    #endif
    /** And close:                                                                    */
    fclose(fp); 
    return true;
}

/** Entry-Parser: *********************************************************************
 *    Tries to read and parse a value with a given token from the ini-file:           */

INT32 CConfigHandler::iParseFileEntry(FILE *fp, const WCHAR* pszwToken, DWORD dwLim, INT32 ulDefault) {
    WCHAR buf[C_CONF_LINE_SIZE];
    DWORD dwRdVal;
    WCHAR *epr;
    /** Try to fetch a line from the file:                                            */
    if (fgetws(buf, C_CONF_LINE_SIZE, fp) == NULL) return ulDefault;
    /** Check, if it contains the expected token:                                     */
    if (wcsncmp(buf, pszwToken, wcslen(pszwToken)) != 0) return ulDefault;
    /** Try to parse the numeric argument behind it:                                  */
    dwRdVal = wcstol(&buf[wcslen(pszwToken)], &epr, 10);
    /** When the result is not in bounds, it was not successful:                      */
    if ((dwRdVal < 1) || (dwRdVal > dwLim)) return ulDefault;
    /** If it was, the value can be written out:                                      */
    return (INT32)dwRdVal;
}

/** File-Reader: **********************************************************************
 *    Open the source-file and uses the parser above to fetch the configuration-      *
 *    values one by one:                                                              */

bool CConfigHandler::bReadFromFile(const WCHAR* pszwFName) {
    FILE *fp;
    WCHAR buf[C_CONF_LINE_SIZE];
    /** Try to open the file:                                                         */
    fp = _wfopen(pszwFName, L"r, ccs=UTF-8");
    if (fp == NULL) return false;
    /** Fetch the first line, and check if it is the expected header:                 */
    if (fgetws(buf, C_CONF_LINE_SIZE, fp) == NULL) return false;
    if (wcsncmp(buf, L"[TinyVoc]", 7) != 0) return false;
    /** Use the parser to fetch and check the configuration-items:                    */
    iFontSize  = iParseFileEntry(fp, L"FontSize=" , CNF_MAX_FONTSIZE , CNF_DEF_FONTSIZE );
    iSelection = iParseFileEntry(fp, L"Selection=", CNF_MAX_SELECTION, CNF_DEF_SELECTION);
    iSubCount  = iParseFileEntry(fp, L"SubCount=" , CNF_MAX_SUBCOUNT , CNF_DEF_SUBCOUNT );
    iIgnCase   = iParseFileEntry(fp, L"IgnCase="  , CNF_MAX_IGNCASE  , CNF_DEF_IGNCASE  );
    /** try to read the locale:                                                       */
    if (fgetws(buf, C_CONF_LINE_SIZE, fp) == NULL) return false;
    if (wcsncmp(buf, L"Locale=", 7) != 0) return false;
    wcscpy(sLocale, &buf[7]);
    /** And close the file:                                                           */
    fclose(fp);
    return true;
}

/** Default-Configurator: *************************************************************
 *    This sets the configuration values to the default values, when anything         *
 *    failed while trying to read them from somewhere:                                */

void CConfigHandler::vSetDefaultData(void) {
    iFontSize  = CNF_DEF_FONTSIZE;
    iSelection = CNF_DEF_SELECTION;
    iSubCount  = CNF_DEF_SUBCOUNT;
    iIgnCase   = CNF_DEF_IGNCASE;
    GetKeyboardLayoutName(sLocale);
}
