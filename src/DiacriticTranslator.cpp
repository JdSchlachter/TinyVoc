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
#include "DiacriticTranslator.h"

CDiacriticTranslator::CDiacriticTranslator(HWND hOwner) {
    hParent     = hOwner;
    bHiddenChar = false;
    Diacritic   = 0;
}

CDiacriticTranslator::~CDiacriticTranslator() {

}

bool CDiacriticTranslator::Translate(UINT* message, WPARAM* wParam, LPARAM* lParam) {
    /** Note the current status to be put out at the end: */
    bool bHandled = false;
    /** Ignore any message, that is not a character-message: */
    if ((*message != WM_DEADCHAR) && (*message != WM_CHAR)) return false;
    /** Give out the symbols, when they are hit twice: */
    if ((Diacritic == (WCHAR)(*wParam)) && (!bHiddenChar)) {
        *message  = WM_CHAR;
        Diacritic = 0;
        return false;
    }
    /** Check the diacritics one by one: */
    if (*wParam == L'´') {              // acute
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'`') {        // grave
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'^') {        // circumflex
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'~') {        // tilde
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'"') {        // diaeresis
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'\'') {       // dot
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'/') {        // slash
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (*wParam == L'_') {        // over resp. underscore
        Diacritic = (WCHAR)(*wParam);
        bHandled = true;
    }else if (Diacritic != 0) {
        /** This is not a diacritic, but there was one before: */
        TranslateChar(Diacritic, (WCHAR*)wParam);
        *message = WM_CHAR;
        Diacritic = 0;
    }
    /** Store the fact, that this was a hidden character: */
    bHiddenChar = (*message == WM_DEADCHAR);
    return bHandled;
}

void CDiacriticTranslator::TranslateChar(WCHAR Dia, WCHAR * Base) {
    /* Not supported:
        ÆÞðþĂăĄąďĔĕĘęĚěĞğ
        ĬĭĮįİıĲĳĶķĸĻļĽľŅņŉ
        ŊŋŇňŎŏŐőŒœŔŕŖŗ
        ŢţťŬŭŰűŲų */
    switch (Dia) {
    case L'`':
        if (*Base == L'A') *Base = L'À';
        if (*Base == L'C') *Base = L'Ç';
        if (*Base == L'E') *Base = L'È';
        if (*Base == L'I') *Base = L'Ì';
        if (*Base == L'O') *Base = L'Ò';
        if (*Base == L'U') *Base = L'Ù';
        if (*Base == L'a') *Base = L'à';
        if (*Base == L'c') *Base = L'ç';
        if (*Base == L'e') *Base = L'è';
        if (*Base == L'i') *Base = L'ì';
        if (*Base == L'o') *Base = L'ò';
        if (*Base == L'u') *Base = L'ù';
        if (*Base == L'S') *Base = L'Ş';
        if (*Base == L's') *Base = L'ş';
        if (*Base == L'G') *Base = L'Ģ';
        if (*Base == L'g') *Base = L'ģ';       
        break;
    case L'´':
        if (*Base == L'A') *Base = L'Á';
        if (*Base == L'E') *Base = L'É';
        if (*Base == L'I') *Base = L'Í';
        if (*Base == L'O') *Base = L'Ó';
        if (*Base == L'U') *Base = L'Ú';
        if (*Base == L'Y') *Base = L'Ý';
        if (*Base == L'a') *Base = L'á';
        if (*Base == L'e') *Base = L'é';
        if (*Base == L'i') *Base = L'í';
        if (*Base == L'o') *Base = L'ó';
        if (*Base == L'u') *Base = L'ú';
        if (*Base == L'y') *Base = L'ý';
        if (*Base == L'C') *Base = L'Ć';
        if (*Base == L'c') *Base = L'ć';
        if (*Base == L'L') *Base = L'Ĺ';
        if (*Base == L'l') *Base = L'ĺ';
        if (*Base == L'N') *Base = L'Ń';
        if (*Base == L'n') *Base = L'ń';
        if (*Base == L'S') *Base = L'Ś';
        if (*Base == L's') *Base = L'ś';
        if (*Base == L'Z') *Base = L'Ź';
        if (*Base == L'z') *Base = L'ź';
        if (*Base == L'!') *Base = L'¡';
        if (*Base == L'?') *Base = L'¿';
        break;
    case L'^':
        if (*Base == L'A') *Base = L'Â';
        if (*Base == L'E') *Base = L'Ê';
        if (*Base == L'I') *Base = L'Î';
        if (*Base == L'O') *Base = L'Ô';
        if (*Base == L'U') *Base = L'Û';
        if (*Base == L'a') *Base = L'â';
        if (*Base == L'e') *Base = L'ê';
        if (*Base == L'i') *Base = L'î';
        if (*Base == L'o') *Base = L'ô';
        if (*Base == L'u') *Base = L'û';
        if (*Base == L'C') *Base = L'Ĉ';
        if (*Base == L'c') *Base = L'ĉ';
        if (*Base == L'G') *Base = L'Ĝ';
        if (*Base == L'g') *Base = L'ĝ';
        if (*Base == L'H') *Base = L'Ĥ';
        if (*Base == L'h') *Base = L'ĥ';
        if (*Base == L'J') *Base = L'Ĵ';
        if (*Base == L'j') *Base = L'ĵ';
        if (*Base == L'S') *Base = L'Ŝ';
        if (*Base == L's') *Base = L'ŝ';
        if (*Base == L'W') *Base = L'Ŵ';
        if (*Base == L'w') *Base = L'ŵ';
        if (*Base == L'Y') *Base = L'Ŷ';
        if (*Base == L'y') *Base = L'ŷ';
        break;
    case L'~':
        if (*Base == L'A') *Base = L'Ã';
        if (*Base == L'N') *Base = L'Ñ';
        if (*Base == L'O') *Base = L'Õ';
        if (*Base == L'a') *Base = L'ã';
        if (*Base == L'n') *Base = L'ñ';
        if (*Base == L'o') *Base = L'õ';
        if (*Base == L'I') *Base = L'Ĩ';
        if (*Base == L'i') *Base = L'ĩ';
        if (*Base == L'U') *Base = L'Ũ';
        if (*Base == L'u') *Base = L'ũ';
        break;
    case L'"':
        if (*Base == L'A') *Base = L'Ä';
        if (*Base == L'E') *Base = L'Ë';
        if (*Base == L'I') *Base = L'Ï';
        if (*Base == L'O') *Base = L'Ö';
        if (*Base == L'Ü') *Base = L'Ü';
        if (*Base == L'a') *Base = L'ä';
        if (*Base == L'e') *Base = L'ë';
        if (*Base == L'i') *Base = L'ï';
        if (*Base == L'o') *Base = L'ö';
        if (*Base == L'u') *Base = L'ü';
        if (*Base == L'y') *Base = L'ÿ';
        if (*Base == L'Y') *Base = L'Ÿ';
        break;
    case L'/':
        if (*Base == L'O') *Base = L'Ø';
        if (*Base == L'o') *Base = L'ø';
        if (*Base == L'L') *Base = L'Ł';
        if (*Base == L'l') *Base = L'ł';
        if (*Base == L's') *Base = L'ß';
        break;
    case L'_':
        if (*Base == L'A') *Base = L'Ā';
        if (*Base == L'a') *Base = L'ā';
        if (*Base == L'D') *Base = L'Ð';
        if (*Base == L'd') *Base = L'đ';
        if (*Base == L'E') *Base = L'Ē';
        if (*Base == L'e') *Base = L'ē';
        if (*Base == L'H') *Base = L'Ħ';
        if (*Base == L'h') *Base = L'ħ';
        if (*Base == L'I') *Base = L'Ī';
        if (*Base == L'i') *Base = L'ī';
        if (*Base == L'O') *Base = L'Ō';
        if (*Base == L'o') *Base = L'ō';
        if (*Base == L'T') *Base = L'Ŧ';
        if (*Base == L't') *Base = L'ŧ';
        if (*Base == L'U') *Base = L'Ū';
        if (*Base == L'u') *Base = L'ū';
        break;
    case L'\'':
        if (*Base == L'A') *Base = L'Å';
        if (*Base == L'a') *Base = L'å';
        if (*Base == L'C') *Base = L'Ċ';
        if (*Base == L'c') *Base = L'ċ';
        if (*Base == L'D') *Base = L'Ď';
        if (*Base == L'd') *Base = L'ď';
        if (*Base == L'E') *Base = L'Ė';
        if (*Base == L'e') *Base = L'ė';
        if (*Base == L'G') *Base = L'Ġ';
        if (*Base == L'g') *Base = L'ġ';
        if (*Base == L'L') *Base = L'Ŀ';
        if (*Base == L'l') *Base = L'ŀ';
        if (*Base == L'U') *Base = L'Ů';
        if (*Base == L'u') *Base = L'ů';
        if (*Base == L'Z') *Base = L'Ż';
        if (*Base == L'z') *Base = L'ż';
        if (*Base == L'R') *Base = L'Ř';
        if (*Base == L'r') *Base = L'ř';
        if (*Base == L'S') *Base = L'Š';
        if (*Base == L's') *Base = L'š';
        if (*Base == L'T') *Base = L'Ť';
        if (*Base == L'Z') *Base = L'Ž';
        if (*Base == L'z') *Base = L'ž';
        break;
    }
}
