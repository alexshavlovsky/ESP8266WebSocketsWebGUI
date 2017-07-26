/*
  DiscretTimer.h -- simple discretization timer

  Copyright (c) 2017 Alex Shavlovsky. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef DISCRETTIMER_H_
#define DISCRETTIMER_H_

class DiscretTimer {
    uint32_t MinSamplePeriod; // min sampling interval
    uint32_t TimeNow; // corrent time stamp
    uint32_t TimeLast; // previous time stamp
public:
    DiscretTimer(uint32_t SetMinSamplePeriod):MinSamplePeriod(SetMinSamplePeriod){
        TimeLast=millis();
    }
    void setPeriod(uint32_t SetMinSamplePeriod){
        MinSamplePeriod=SetMinSamplePeriod;
    }
    bool onSample() {
        TimeNow=millis();
        if(TimeNow - TimeLast >= MinSamplePeriod) {
            TimeLast = TimeNow;
            return true;
        }
        return false;
    }
};

#endif //DISCRETTIMER_H_
