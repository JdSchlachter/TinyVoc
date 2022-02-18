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

/** Class Definition: *****************************************************************/

class  CDiacriticTranslator {
public:
    CDiacriticTranslator(HWND hOwner);
    ~CDiacriticTranslator();
    bool Translate(UINT* message, WPARAM *wParam, LPARAM *lParam);
private:
	void TranslateChar(WCHAR Dia, WCHAR* Base);
	HWND  hParent;
	bool  bHiddenChar;
	WCHAR Diacritic;
};

