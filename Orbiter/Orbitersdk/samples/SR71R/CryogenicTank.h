//	CryogenicTank - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
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

#include "IConsumable.h"
#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Component.h"
#include "bc_orbiter\signals.h"

#include <string>

namespace bco = bc_orbiter;

/**
Base class for a cryogenic tank.
*/
class CryogenicTank : public bco::PoweredComponent,
	public IConsumable
{
public:
	CryogenicTank(bco::BaseVessel* vessel, double capacity, double fillRate, double lossPerHour, char* configKey);
    
	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	virtual void Step(double simt, double simdt, double mjd);

	// IConsumable:
	virtual double GetLevel() const override;
	virtual double Draw(double amount) override;

	bool	IsFilling() const { return isFilling_; }
	void	IsFilling(bool b);
	bool	IsAvailable() const { return isAvailable_; }
	void	IsAvailable(bool b) { isAvailable_ = b; }

	bco::signal<double>&	LevelSignal()			{ return sigLevel_; }		 // 0 - 1 : does adjustment based on capacity
	bco::signal<bool>&		IsFillingSignal()		{ return sigIsFilling_; }
	bco::signal<bool>&		IsAvailableSignal()		{ return sigIsAvailable_; }

	bco::slot<bool>&		ToggleFillSlot()		{ return slotToggleFill_; }
	bco::slot<bool>&		ExternAvailableSlot()	{ return slotExternalAvail_; }

protected:
	void					FillTank(double amount);
	void					ToggleFilling();

private:

	bco::signal<double>		sigLevel_;
	bco::signal<bool>		sigIsFilling_;
	bco::signal<bool>		sigIsAvailable_;

	bco::slot<bool>			slotToggleFill_;
	bco::slot<bool>			slotExternalAvail_;

	bool					isFilling_;
	bool					isAvailable_;
	double					prevTime_;

	std::string				configKey_;

	double					capacity_;
	double					level_;
    double                  lossRate_;
	double					fillRate_;
};