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

/** Used Defines: *********************************************************************/

#pragma once

#define CNF_MAX_FONTSIZE  30
#define CNF_MAX_SELECTION 2
#define CNF_MAX_SUBCOUNT  9999
#define CNF_MAX_IGNCASE   1

#define CNF_DEF_FONTSIZE  20
#define CNF_DEF_SELECTION 0
#define CNF_DEF_SUBCOUNT  20
#define CNF_DEF_IGNCASE   0

#define C_CONF_LINE_SIZE 80

/** Class Definition: *****************************************************************/

class CConfigHandler {
public:
    // Properties:
    INT32        iFontSize, iSelection, iSubCount, iIgnCase;
    WCHAR        sLocale[80];
    // Methods:
    CConfigHandler();
    ~CConfigHandler();
    bool   bIsPortable(void);
private:
    bool  bPortable;
    void  vCheckPortable(LPWSTR szNameBuffer);
    bool  bWriteToFile(const WCHAR* pszwFName);
    INT32 iParseFileEntry(FILE *fp, const WCHAR* pszwToken, DWORD dwLim, INT32 ulDefault);
    bool  bReadFromFile(const WCHAR* pszwFName);
    void  vSetDefaultData(void);
};
