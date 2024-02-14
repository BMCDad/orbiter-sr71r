//	MFDBase - bco Orbiter Library
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

#include "Component.h"
#include "vessel.h"
#include "Control.h"
#include <map>

namespace bc_orbiter
{
	class MFDBase : 
		public Component,
		public VesselComponent,
		public PowerConsumer
	{
	public:
		MFDBase(PowerProvider& pwr, vessel* vessel, int mfdId, double amps) :
			Component(vessel),
			power_(pwr),
			mfdId_(mfdId),
			vessel_(*vessel)
		{
			power_.AttachConsumer(this);
		}

		// power_consumer
		double AmpDraw() const override { 
			return 
				(IsPowered() && (oapiGetMFDMode(mfdId_) != MFD_NONE)) 
				? 4.0 : 0.0; 
		}

		void OnChange(double v) {
			if (!IsPowered()) { oapiOpenMFD(MFD_NONE, mfdId_); }
		}

		bool OnVCMouseEvent(int id, int event) { return OnMouseEvent(id, event); }
		bool OnPanelMouseEvent(int id, int event) { return OnMouseEvent(id, event); }

//		virtual void ChangePowerLevel(double newLevel) override;

		/**
		Notification when the MFD mode changes.
		*/
		void OnMfdMode(int mfdId, int mode);

		/**
		Returns the label for 'button'.
		*/
		const char* GetButtonLabel(int button);

		/**
		Handles the mouse event.
		*/
		bool OnMouseEvent(int id, int event);

//		virtual double CurrentDraw() override;

	protected:
		PowerProvider& power_;
		vessel& vessel_;

		bool IsPowered() const { 
			return (power_.VoltsAvailable() > 24.0);
		} // power_.volts_available() > 24.0; }

		void Update();
		void Redraw();
		void AssignKey(int areaId, int mfdKey);
		void AssignPwrKey(int areaId) { idPower_ = areaId; }
		int GetPwrKey() const { return idPower_; }
		void AssignSelect(int areaId) { idSelect_ = areaId; }
		int GetSelectKey() const { return idSelect_; }
		void AssignMenu(int areaId) { idMenu_ = areaId; }
		int GetMenuKey() const { return idMenu_; }
	private:
		
		int						mfdId_{ 0 };
		std::map<int, int>		mfdButtonIds_;

		int						idPower_{ 0 };
		int						idSelect_{ 0 };
		int						idMenu_{ 0 };
	};

	//inline void MFDBase::ChangePowerLevel(double newLevel)
	//{
	//	PoweredComponent::ChangePowerLevel(newLevel);
	//	Update();
	//}

	inline void MFDBase::OnMfdMode(int mfdId, int mode)
	{
		if (mfdId == mfdId_)
		{
			Redraw();
		}
	}

	inline const char* MFDBase::GetButtonLabel(int button)
	{
		const char* label = NULL;

		auto btn = mfdButtonIds_.find(button);
		if (btn != mfdButtonIds_.end())
		{
			label = oapiMFDButtonLabel(mfdId_, btn->second);
		}

		return label;
	}

	inline bool MFDBase::OnMouseEvent(int id, int event)
	{
		bool result = false;

		if (!IsPowered()) return false;

		auto key = mfdButtonIds_.find(id);
		if (key != mfdButtonIds_.end())
		{
			oapiProcessMFDButton(mfdId_, key->second, event);
			result = true;
		}
		else
		{
			if (id == idPower_)
			{
				oapiToggleMFD_on(mfdId_);
				result = true;
			}
			else if (id == idSelect_)
			{
				oapiSendMFDKey(mfdId_, OAPI_KEY_F1);
				result = true;
			}
			else if (id == idMenu_)
			{
				oapiSendMFDKey(mfdId_, OAPI_KEY_GRAVE);
				result = true;
			}
		}

		return result;
	}

	inline void MFDBase::Update()
	{
		auto mode = oapiGetMFDMode(mfdId_);

		if (mode != MFD_NONE)
		{
			oapiRefreshMFDButtons(mfdId_);
		}
	}

	inline void MFDBase::Redraw()
	{
		for (auto &p : mfdButtonIds_)
		{
            vessel_.TriggerRedrawArea(0, 0, p.first);
		}

        vessel_.TriggerRedrawArea(0, 0, idPower_);
        vessel_.TriggerRedrawArea(0, 0, idSelect_);
        vessel_.TriggerRedrawArea(0, 0, idMenu_);
	}

	inline void MFDBase::AssignKey(int areaId, int mfdKey)
	{
		mfdButtonIds_[areaId] = mfdKey;
	}
}