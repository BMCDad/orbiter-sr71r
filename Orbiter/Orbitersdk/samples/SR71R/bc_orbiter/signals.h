//	signal - bco Orbiter Library
//	Copyright(C) 2023  Blake Christensen
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

#include <vector>

namespace bc_orbiter {

	/**
	* slot
	* Receives a notification from a signal when the signal changes.  Used
	* along with the signal class to facilitate communication between components.
	*/
	template<typename T>
	class slot {
	public:
		slot(const std::function<void(T)> func)
			:
			func_(func)
		{}

		virtual ~slot() {}

		virtual void notify(T value) {
			if (value != value_) {
				value_ = value;
				if (nullptr != func_) func_(value_);
			}
		}

		virtual T value() const { return value_; }
	private:
		T value_{};
		const std::function<void(T)> func_{ nullptr };
	};

	/**
	* signal
	* Along with slot, provides a means to pass events between components.
	*/
	template<typename T>
	class signal {
	public:
		signal() = default;
		virtual ~signal() = default;

		void attach(slot<T>& sl) {
			slots_.emplace_back(&sl);
		}

		void fire(const T& val) {
			if (dirty_ || (val != prevValue_)) {
				prevValue_ = val;
				dirty_ = true;
				for (const auto& s : slots_) {
					s->notify(val);
				}
			}
		}

		T current() const {
			return prevValue_;
		}

	private:
		std::vector<slot<T>*> slots_;
		T prevValue_{};
		bool dirty_{ true };
	};

	/**
	* connector
	* A helper class used to connect a signal to a slot.
	* TODO:  This class can probably be made a static function.
	*/
	template<typename TSignal, typename TSlot>
	class connector {
	public:
		connector(TSignal& sig, TSlot& slot)
			:
			signal_(sig),
			slot_(slot)
		{
			signal_.attach(slot_);
		}

	private:
		TSignal& signal_;
		TSlot& slot_;
	};

}
