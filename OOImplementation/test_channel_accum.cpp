//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  test_channel_accum.cpp
//
//  TestBench for Channel Accumulator.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <cmath>
#include <stdlib.h>

#include "channel_accum.hpp"

static float MAX_FLOAT_ERR = 1e-6;

// Test Instance and Memories
channel_accum ch_acc;

// Helper Function: Check Reset Functionality.
bool _ca_check_zero() {
  printf("CHECKING FOR ZEROS:\n");
  // check products:
  for (int i = 0; i < MAX_NUM_CHANNELS; i++) {
    // printf("checking element %d: ", i);

    float expected = 0;
    float acquired = ch_acc.read(i);

    if (fabs(expected - acquired) > MAX_FLOAT_ERR) {
      printf("ERROR! expected: %f != acquired: %f in element %d\n", expected,
             acquired, i);
      return false;
    } else {
      // printf("ok.\n");
    }
  }
  printf("ok.\n");
  return true;
}

// Helper Function: Check given Expected Results
bool _ca_check_expected(float *shouldbe, int num_channels) {
  printf("CHECKING FOR EXPECTED RESULTS:\n");
  // check products:
  for (int i = 0; i < num_channels; i++) {
    printf("checking element %d: ", i);

    float expected = shouldbe[i];
    float acquired = ch_acc.read(i);

    if (fabs(expected - acquired) > MAX_FLOAT_ERR) {
      printf("ERROR! expected: %f != acquired: %f in element %d\n", expected,
             acquired, i);
      return false;
    } else {
      printf("ok.\n");
    }
  }
  printf("ok.\n");
  return true;
}

// Main TestBench for Channel Accumulator
// Returns Exit Code 0 if all goes well, Exit Code -1 otherwise.
bool test_channel_accum() {

  // Test 1: Small, fixed Test
  const int num_ch = 3;
  const int num_vals = 3;
  float vals_to_add[num_ch][num_vals] = {
      {1, 2, 3}, // 6
      {4, 5, 6}, // 15
      {7, 8, 9}  // 24
  };
  float goldenresults[num_ch] = {6, 15, 24};

  ch_acc.reset();
  if (!_ca_check_zero())
    return false;
  for (int ch = 0; ch < num_ch; ch++) {
    for (int val = 0; val < num_vals; val++) {
      ch_acc.add(vals_to_add[ch][val], ch);
    }
  }
  if (!_ca_check_expected(goldenresults, num_ch))
    return false;

  return true;
}
