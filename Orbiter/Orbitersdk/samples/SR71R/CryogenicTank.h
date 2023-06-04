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

#include <string>

namespace bco = bc_orbiter;

/**
Base class for a cryogenic tank.
*/
class CryogenicTank : public bco::PoweredComponent,
	public IConsumable
{
public:
	CryogenicTank(bco::BaseVessel* vessel, double capacity, double lossPerHour, char* configKey);
    
	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	virtual void Step(double simt, double simdt, double mjd);

	// IConsumable:
	virtual double GetLevel() const override;
	virtual double RotateMesh(double amount) override;

	bool	IsFilling() const { return isFilling_; }
	void	IsFilling(bool b) { isFilling_ = b; }
	bool	IsAvailable() const { return isAvailable_; }
	void	IsAvailable(bool b) { isAvailable_ = b; }

protected:
	void					FillTank(double amount);
	void					ToggleFilling();

private:
	bool					isFilling_;
	bool					isAvailable_;

	std::string				configKey_;

	double					capacity_;
	double					level_;
    double                  lossRate_;
};