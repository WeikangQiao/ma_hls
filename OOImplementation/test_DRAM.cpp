//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  test_dram.cpp
//
//  TestBench for DRAM Simulator, type Float.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <cmath>
#include <stdlib.h>
#include <cfloat>
#include <vector>

#include "DRAM.hpp"

static float MAX_FLOAT_ERR = 1e-6;

// Test Instance: DRAM with floats.
DRAM<float> testRAM;

// Main TestBench for DRAM Simulator.
// Checks Write, Readback.
// Returns true if all goes well, false otherwise.
bool test_DRAM() {

  // Test 1: Write predefined Data, Read Back and Check
  int addr[] = {0, 1, 2, 24, 33, 1024, DRAM_MAX_FILTER_SIZE - 1};
  float vals[] = {0, 1.000, FLT_MAX, FLT_MIN};

  int num_addrs = (sizeof(addr) / sizeof(*addr));
  int num_vals = (sizeof(vals) / sizeof(*vals));

  for (int v = 0; v < num_vals; v++) {
    for (int a = 0; a < num_addrs; a++) {
      testRAM.write(addr[a], vals[v]);
    }
    for (int a = 0; a < num_addrs; a++) {
      float acquired = testRAM.read(addr[a]);
      float expected = vals[v];
      if (fabs(acquired - expected) > MAX_FLOAT_ERR) {
        printf("ERROR: expected: %f, acquired: %f\n", expected, acquired);
        return false;
      }
    }
  }
  printf("all ok.\n");
  return true;
}
