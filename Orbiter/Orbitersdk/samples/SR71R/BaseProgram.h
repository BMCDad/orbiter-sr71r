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
#include "AscentProgram.h"

/*
+--------------------+
|MAIN          PG 1:3|
|<ASCENT             |
|                    |
|<ORBIT OPS          |
|                    |
|<RE ENTRY           |
|                    |
|<ATMOSPHERE         |
|                    |
|                    |
+--------------------+

Page1: (select program)
FN1 - 'ASCENT'      - FC::LoadProgram('ASCENT')
FN2 - 'ORBIT OPS'   - FC::LoadProgram('ORBIT')
FN3 - 'RE ENTRY'    - FC::LoadProgram('REENTRY')
FN4 - 'ATMOSPHERE'  - FC::LoadProgram('ATMO')

Page2: (version info)
*/

namespace FC
{
    class BaseProgram : public Program
    {
    public:
        BaseProgram(FC::IComputer* comp) :
            Program(comp, "MAIN"),
            activeProgram_(this)
        {
            RegisterPage([this]() {SetupMainPage(); }, nullptr);
            RegisterPage([this]() {SetupHelpPage(); }, nullptr);
        }

        void Init() override
        {
            Program::Init();
            activeProgram_ = this;
        }

		

        void Step(double simt, double simdt, double mjd) override
        {
            // Give time to each program, even if not active.
//            ascentProgram_.Step(simt, simdt, mjd);
        }

        bool HandleKey(FC::GCKey key) override
        {
            bool result = false;

            if (this != activeProgram_)
            {
                result = activeProgram_->HandleKey(key);
            }

			if (result) return result;

			return Program::HandleKey(key);
        }

		double GetScratch() { return scratchValue_; }
		void SetScratch(double value) { scratchValue_ = value; }

    private:
        Program*        activeProgram_;

//        AscentProgram   ascentProgram_;

		double			scratchValue_;

        void LoadAscent()
        {
//            activeProgram_ = &ascentProgram_;
//            activeProgram_->Init();
        }

        void SetupMainPage()
        {
            SetFunctionKeyFunc(1, [this]() {LoadAscent(); }, "ASCENT");
//            TextOut(6, 1, "WIP - Coming soon");
        }

        void SetupHelpPage()
        {
            TextOut(3, 1, "Flight");
            TextOut(4, 1, "Computer");
            TextOut(5, 1, "Vers: 0.01");
        }
    };
}