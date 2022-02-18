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

/** Forward-Declarations: *************************************************************/

void Trainer_Init(HINSTANCE hInstance, CConfigHandler* nConfig);
bool Trainer_Prepare(WCHAR* Vocabulary);
void Trainer_Run(HINSTANCE hInstance, HWND hWnd);

/** Class Definition: *****************************************************************/

class ControlLocator {
	public:
	void GetLocalCoordinates(HWND hWnd);
	void LeftAlign(LPARAM lParam);
	void RightAlign(LPARAM lParam);
	private:
    HWND hWnd;
	int left, top, height, width, dist_r, dist_l;
};

