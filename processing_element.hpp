//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  processing_element.hpp
//
//  Implements the actual Multiply-Accumulate Unit ("Processing Element").
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

static const int NUM_MULT = 9;

// Processing Element: Multiply-Accumulate Unit
class processing_element {

private:
  // Internal Wires / Connections:
  float products[NUM_MULT];
  float acc;

public:
  // Public Interface:
  void macc(float img_in[NUM_MULT], float filt_in[NUM_MULT], float &sum_out,
            float prod_out[NUM_MULT]);
};
