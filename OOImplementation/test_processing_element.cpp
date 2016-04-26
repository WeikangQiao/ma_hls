//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  test_processing_element.cpp
//
//  TestBench for Processing Element.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <cmath>
#include <stdlib.h>  

#include "processing_element.hpp"

static float MAX_FLOAT_ERR = 1e-6;

// Test Instance and Memories
processing_element _PE;
float _PE_input[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
float _PE_filt[9] = {0, 0, 0, 1, 0, 0, 0, 0, 0};
float _PE_out = 0;
float _PE_products[9];

// Helper Function: Check individual Multipliers by alternative Computation
bool check_PE_products() {
  printf("CHECKING PRODUCT:\n");
  // check _PE_products:
  for (int i = 0; i < 9; i++) {
    //printf("checking element %d: ", i);

    float expected = _PE_input[i] * _PE_filt[i];
    float acquired = _PE_products[i];

    if (fabs(expected - acquired) > MAX_FLOAT_ERR) {
      printf("ERROR! expected: %f != acquired: %f\n", expected, acquired);
      return false;
    } else {
      printf("ok. ");
    }
  }
  printf("\n");
  return true;
}

// Helper Function: Check final Accumulation Sum by alternative Computation.
// Need to call check__PE_products() first!
bool check_PE_sum() {
  printf("CHECKING SUM:\n");
  // check sum:
  float mysum = 0;
  for (int i = 0; i < 9; i++) {
    mysum += _PE_products[i];
  }
  if (fabs(mysum - _PE_out) > MAX_FLOAT_ERR) {
    printf("ERROR! expected: %f != acquired: %f\n", mysum, _PE_out);
    return false;
  } else {
    printf("%f, ok.\n", mysum);
    return true;
  }
}

// Main TestBench for Processing Elements. 
// Checks Multipliers and Accumulators.
// Returns true if all goes well, false otherwise.
bool test_processing_element() {

  // Test 1: Predefined _PE_filter, Predefined Image
  _PE.macc(_PE_input, _PE_filt, _PE_out, _PE_products);
  if (!check_PE_products())
    return false;
  if (!check_PE_sum())
    return false;
  
  // Test 2: Random _PE_filter, Random Image
  for (int i = 0; i < 9; i++) {
    _PE_input[i] = rand()/(float)RAND_MAX;
    _PE_filt[i] = rand()/(float)RAND_MAX;
    //printf("elem %d: %f * %f", i, _PE_input[i], _PE_filt[i]);
  }
  _PE.macc(_PE_input, _PE_filt, _PE_out, _PE_products);
  if (!check_PE_products())
    return false;
  if (!check_PE_sum())
    return false;
  
  return true;
}
