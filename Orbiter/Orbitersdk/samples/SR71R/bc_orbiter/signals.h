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

#include <functional>
#include <vector>

namespace bc_orbiter {

/**
* Slot
* Receives a notification from a signal when the signal changes.  Used
* along with the signal class to facilitate communication between components.
*/
template<typename T>
class Slot {
 public:
  Slot(const std::function<void(T)> func = [](T v) {}) : func_(func) {}

  virtual ~Slot() {}

  void Notify(T Value) {
    if (dirty_ || (Value != value_)) {
      dirty_ = false;
      value_ = Value;
      if (nullptr != func_) func_(value_);
    }
  }

  T Value() const { return value_; }
  void Set() { dirty_ = true; }

 private:
  bool	dirty_{ true };
  T value_ { };

  const std::function<void(T)> func_{ nullptr };
};

/**
* signal
* Along with slot, provides a means to pass events between components.
*/
template<typename T>
class Signal {
 public:
  Signal() = default;
  virtual ~Signal() = default;

  void Attach(Slot<T>& sl) {
    slots_.emplace_back(&sl);
  }

  void Fire(const T& val) {
    value_ = val;
    for (const auto& s : slots_) {
      s->Notify(val);
    }
  }

  T Current() const {
    return value_;
  }

  void Update(T val) {	// Update value without firing event (config)
    value_ = val;
  }

  friend std::istream& operator>>(std::istream& input, Signal<T>& obj) {
    if (input) {
      T Value;
      input >> Value;
      obj.Fire(Value);
    }

    return input;
  }

  friend std::ostream& operator<<(std::ostream& output, Signal<T>& obj) {
    output << obj.value_;
    return output;
  }

 private:
  std::vector<Slot<T>*> slots_;
  T value_{};
};

/**
* signal
* Along with slot, provides a means to pass events between components.
*/
class Signaller {
 public:
  Signaller() = default;
  virtual ~Signaller() = default;

  void Attach(const std::function<void()> sl) {
    funcs_.emplace_back(sl);
  }

  void Fire() {
    for (const auto& s : funcs_) {
      s();
    }
  }

private:
  std::vector<std::function<void()>> funcs_;
};

template<typename TSignal, typename TSlot>
void Connect(TSignal& sig, TSlot& Slot) {
  sig.Attach(Slot);
}
}
