//	Avionics - SR-71r Orbiter Addon
//	Copyright(C) 2016  Blake Christensen
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "FCProgram.h"

/*
	           1111111111
	 01234567890123456789
	+--------------------+
	|ASCENT        PG 1:1|			0
[F1]|<ALT     [ 232.30k] |[F6]		1
	|                    |			1
[F2]|<INC     [  +78.40] |[F7]		3
	|                    |			4
[F3]|     ALT       INC  |[F8]		5
	|C 030.09k    88.03  |			6
[F4]|D 163.10k   -10.30  |[F9]		7
	|                    |			8
[F5]|                HUD>|[F10]	9
	|[          ]        |			10
	+--------------------+
*/

namespace FC
{
    /**
    Ascent program controls the ascent to orbit procedures.
    */
    class AscentProgram : public Program
    {
    public:
  //      AscentProgram()
  //      {
		//}


		//// IProgram
		//void Reset() override 
		//{
		//	targetAltitude_ = 0.0;
		//	targetInclination_ = 0.0;
		//	prevTime_ = 0; 0;
		//}

		//void LoadPage(IDisplay* display) override
		//{
		//	display->SetFuncLabel(GCKey::F1, "ALT");
		//	display->SetFuncLabel(GCKey::F2, "INC");
		//	display->SetFuncLabel(GCKey::F10, "HUD");

		//	display->DisplayText(0, 0, "ASCENT        PG 1:1");
		//	display->DisplayText(1, 9, "[      k]");
		//	display->DisplayText(3, 9, "[      k]");
		//	display->DisplayText(5, 5, "ALT       INC");
		//	display->DisplayText(6, 0, "C       k");
		//	display->DisplayText(7, 0, "D       k");
		//}

		//void HandleFunc(GCKey key, IComputer* computer) override
		//{
		//	switch (key)
		//	{
		//	case GCKey::F1:
		//		AcceptAltInput(computer);
		//		break;

		//	case GCKey::F2:
		//		AcceptIncInput(computer);
		//		break;

		//	case GCKey::F10:
		//		// HUD
		//		break;
		//	}
		//}

		//void UpdatePage(IDisplay* display) override
		//{

		//}

		//virtual void Step(double simt, double simdt, double mjd, IAvionics* avionics) = 0;


  //  private:
  //      double targetAltitude_ = 0.0;
  //      double targetInclination_ = 0.0;
  //      double prevTime_ = 0.0;

		//void AcceptAltInput(IComputer* computer)
		//{
		//	targetAltitude_ = computer->GetScratchPad();
		//}

		//void AcceptIncInput(IComputer* computer)
		//{
		//	targetInclination_ = computer->GetScratchPad();
		//}
    };
}