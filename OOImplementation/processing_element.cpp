//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  processing_element.cpp
//
//  Implements the actual Multiply-Accumulate Unit ("Processing Element").
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "processing_element.hpp"

// Processing Element: Multiply-Accumulate Unit
void processing_element::macc(float img_in[NUM_MULT], float filt_in[NUM_MULT],
                              float &sum_out, float prod_out[NUM_MULT]) {

// Reshape input and output arrays so that all NUM_MULT elements
// are transferred in parallel.
#pragma HLS ARRAY_RESHAPE variable = img_in complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = filt_in complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = prod_out complete dim = 1

  acc = 0;

// Multiplications: Completely unrolled -> NUM_MULT individual multipliers.
#pragma HLS unroll
L_multiply:
  for (int i = 0; i < NUM_MULT; i++) {
    products[i] = img_in[i] * filt_in[i];
  }

// Accumulation: Completely unrolled, individual adders.
#pragma HLS unroll
L_accumulate:
  for (int i = 0; i < NUM_MULT; i++) {
    acc += products[i];
  }

// Assignment of Outputs: Completely unrolled.
#pragma HLS unroll
L_assign:
  for (int i = 0; i < NUM_MULT; i++) {
    prod_out[i] = products[i];
  }
  sum_out = acc;
}
