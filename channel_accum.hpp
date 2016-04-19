//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  channel_accum.hpp
//
//  Implements the Output Channel Summation.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

static const int MAX_NUM_CHANNELS = 256 / 4;

// Processing Element: Multiply-Accumulate Unit
class channel_accum {

private:
  // Internal Memory
  float accumulator[MAX_NUM_CHANNELS];

  // Public Interface:
public:
  // Reset Accumulation Registers (Call for each new Input Pixel)
  void reset();
  // Add Partial Output Channel Sums to Accumulation Register
  void add(float value, unsigned int channel);
  // Read Accumulated Output Channel
  float read(unsigned int channel);
};
