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

#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "IComputer.h"

namespace FC
{

    /**
    The base program all flight computer programs inherit from.
    */
    class Program
    {
    public:
        Program(FC::IComputer* comp) :
            computer_(comp)
        {}

		virtual void Init() {};
		virtual bool HandleKey(FC::GCKey key) { return false; }
		virtual void Step(double simt, double simdt, double mjd) {}
		virtual void OnActivate() {}


        // Make this available to the parent program.
        bool IsRunning() const { return isRunning_; }
    protected:


        void SetIsRunning(bool state) { isRunning_ = state; }

		IComputer* Computer() { return computer_; }

        IAvionics* Avionics() { return computer_->Avionics(); }

		double GetScratchValue() { return computer_->GetScratchPad(); }
    private:
        // Pointer to the base computer.  This is private, but a child class
        // could grab this during construction if needed.  However, child
        // programs should make all calls through the protected method calls
        // that in essence create the API to the computer.  Then all changes
        // to the computer can happen in one place.
        
		FC::IComputer*      computer_;

        bool                isRunning_;
		double				prevUpdate_;
    };



	/*
	[7][8][9][+/ -][PRV][NXT]
	[4][5][6][.][][HUD]
	[1][2][3][0][ENT][CLR]

		1111111111
		01234567890123456789
		+ -------------------- +
		| MAIN          PG 1:3 | 0
[F1]	| <ASCENT			   | [F6]		1
		|                      |			1
[F2]	| <ORBIT OPS		   | [F7]		3
		|                      |			4
[F3]	| <RE ENTRY			   | [F8]		5
		|                      |			6
[F4]	| <ATMOSPHERE		   | [F9]		7
		|					   |			8
[F5]	|                      | [F10]		9
		| []				   |			10
		+ -------------------- +

		*/

	class MainProgram : public Program
	{
	public:
		MainProgram(FC::IComputer* comp) : Program(comp)
		{}

		// Program overrides
		void OnActivate() override
		{
			Computer()->ClearScreen();
			Computer()->DisplayLine(0, "Main            1/1");
			Computer()->DisplayLine(1, "<ASCENT");
			Computer()->DisplayLine(3, "<ORBIT OPS");
			Computer()->DisplayLine(5, "<RE ENTRY");
			Computer()->DisplayLine(7, "<AUTO PILOT");
		}


		bool HandleKey(FC::GCKey key) 
		{
			bool handled = false;

			switch (key)
			{
			case GCKey::F1:
				Computer()->ActivateProg(FCProg::Ascent);
				break;

			case GCKey::F2:
				Computer()->ActivateProg(FCProg::Orbit);
				break;

			case GCKey::F3:
				Computer()->ActivateProg(FCProg::ReEntry);
				break;

			case GCKey::F4:
				Computer()->ActivateProg(FCProg::Atmosphere);
				break;

			}
			return false; 
		}
	};


	class AtmoProgram : public Program
	{
	public:
		AtmoProgram(FC::IComputer* comp) : Program(comp)
		{
			isAltitude_ = false;
			isHeading_	= false;
			isKEAS_		= false;
			isMACH_		= false;
			isAttitude_	= false;
		}

		// Program overrides
		void OnActivate() override
		{
			RefreshScreen();
		}


		bool HandleKey(FC::GCKey key)
		{
			bool handled = false;

			switch (key)
			{
			case GCKey::F1:
				// Run heading
				isHeading_ = !isHeading_;
				handled = true;
				break;

			case GCKey::F2:
				// Run Altitude
				isAltitude_ = !isAltitude_;
				handled = true;
				break;

			case GCKey::F3:
				// Run KEAS
				isKEAS_ = !isKEAS_;
				if (isKEAS_) isMACH_ = false;
				handled = true;
				break;

			case GCKey::F4:
				// Run MACH
				isMACH_ = !isMACH_;
				if (isMACH_) isKEAS_ = false;
				handled = true;
				break;

			case GCKey::F5:
				// Run Attitude
				isAttitude_ = !isAttitude_;
				handled = true;
				break;

			case GCKey::F10:
				Computer()->ActivateProg(FCProg::Main);
				break;

			}

			if (handled) RefreshScreen();

			return handled;
		}

	private:

		void RefreshScreen()
		{
			Computer()->ClearScreen();
			Computer()->DisplayLine(0, "Auto Pilot      1/1");
			Computer()->DisplayLine(1, "<HDG    .");
			Computer()->DisplayLine(3, "<ALT    .");
			Computer()->DisplayLine(5, "<KEAS   .");
			Computer()->DisplayLine(7, "<MACH   .");
			Computer()->DisplayLine(9, "<ATT    .      RUN>");

			if (isHeading_)		Computer()->DisplayText(1, 8, "X");
			if (isAltitude_)	Computer()->DisplayText(3, 8, "X");
			if (isKEAS_)		Computer()->DisplayText(5, 8, "X");
			if (isMACH_)		Computer()->DisplayText(7, 8, "X");
			if (isAttitude_)	Computer()->DisplayText(9, 8, "X");
		}

		bool isHeading_;
		bool isAltitude_;
		bool isKEAS_;
		bool isMACH_;
		bool isAttitude_;
	};

}