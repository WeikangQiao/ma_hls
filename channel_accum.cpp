//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  channel_accum.cpp
//
//  Implements the Output Channel Summation.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "channel_accum.hpp"

// Reset Accumulation Registers (Call for each new Input Pixel)
void channel_accum::reset() {
  for (int i = 0; i < MAX_NUM_CHANNELS; i++) {
    accumulator[i] = 0;
  }
}
// Add Partial Output Channel Sums to Accumulation Register
void channel_accum::add(float value, unsigned int channel) {
  accumulator[channel] = accumulator[channel] + value;
}
// Read Accumulated Output Channel
float channel_accum::read(unsigned int channel) { return accumulator[channel]; }