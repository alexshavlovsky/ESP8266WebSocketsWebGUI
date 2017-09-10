/*
  FilterAVG.h -- moving average filter

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

#include <DiscretTimer.h>

// constructor example for float value
// FilterAVG <float, int32_t, int32_t, 100> f1(20, 500);
// value type - float
// array type - int32_t
// sum type - int32_t (2 147 483 647 max)
// multiplier - 100 (2 digits)
// array size - 20
// discretization period - 500 ms
// overall averaging period - 20*500=10000 ms
// max allowable value - 2 147 483 647/20/100 = 1073741

// constructor example for integer value
// FilterAVG <int32_t, int32_t, int32_t, 1> f1(50, 100);
// value type - int32_t
// array type - int32_t
// sum type   - int32_t (2 147 483 647 макс.)
// multiplier - 1
// array size - 50
// discretization period - 100 ms
// overall averaging period - 50*100=5000 ms
// max allowable value - 2 147 483 647/50/1 = 42 949 672

template <typename MEASURE_TYPE, typename ARRAY_TYPE, typename SUM_TYPE, int16_t MeasureMultiplier>
class FilterAVG {
    ARRAY_TYPE* filt_array;
    size_t ArrayMaxSize;
    size_t ArrayCurrentSize;
    size_t ArrayWritePos;
    SUM_TYPE ArraySum;
    DiscretTimer* sampler;
public:
    FilterAVG(size_t SetSize, uint32_t SetPeriod):ArrayMaxSize(SetSize) {
        filt_array=new ARRAY_TYPE[ArrayMaxSize];
        sampler=new DiscretTimer(SetPeriod);
        ArraySum=0;
        ArrayCurrentSize=0;
        ArrayWritePos=0;
    }
    ~FilterAVG() {
        delete sampler;
        delete []filt_array;
    }
    void AddMeasure(MEASURE_TYPE Measure) {
        if(!sampler->onSample()) {
            return;
        }
        if(ArrayCurrentSize < ArrayMaxSize) {
            ArraySum=ArraySum+ARRAY_TYPE(Measure*MeasureMultiplier);
            ArrayCurrentSize++;
        } else {
            ArraySum=ArraySum-filt_array[ArrayWritePos]+ARRAY_TYPE(Measure*MeasureMultiplier);
        }
        filt_array[ArrayWritePos]=ARRAY_TYPE(Measure*MeasureMultiplier);
        ArrayWritePos++;
        if(ArrayWritePos==ArrayMaxSize) {
            ArrayWritePos=0;
        }
    }
    MEASURE_TYPE GetAVG() {
        if(ArrayCurrentSize>0) {
            return ArraySum/(ArrayCurrentSize*MeasureMultiplier);
        } else {
            return 0;
        }
    }
    float GetAVGfloat() {
        if(ArrayCurrentSize>0) {
            return (float)ArraySum/(ArrayCurrentSize*MeasureMultiplier);
        } else {
            return 0;
        }
    }
};
