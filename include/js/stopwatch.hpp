/*
CppUtility library
Copyright (C) 2016  Jan Schmidt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <chrono>

namespace js {

/**
 * Stop watch class. std::ratio<1> for seconds
 */
template <class Ratio = std::ratio<1>,
          class Clock = std::chrono::high_resolution_clock, class Rep = double>
class StopWatch {
  private:
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint _start;

  public:
    StopWatch() { reset(); }
    template <class Ratio2, class Clock2, class Rep2>
    StopWatch(const StopWatch<Ratio2, Clock2, Rep2>& watch)
        : _start(watch._start) {}
    ~StopWatch() = default;

    void reset() { _start = Clock::now(); }

    Rep split() {
        TimePoint split = Clock::now();
        return std::chrono::duration<Rep, Ratio>(split - _start).count();
    }

    Rep stop() { return split(); }
};
}
