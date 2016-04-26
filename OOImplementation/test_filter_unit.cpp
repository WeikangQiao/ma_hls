//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  test_filter_unit.cpp
//
//  TestBench for Filter Unit (Filter Fetcher + Filter Cache).
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <string>
#include <cmath>
#include <stdlib.h>

#include "filter_unit.hpp"

static float MAX_FLOAT_ERR = 1e-6;
static int fu_kernel_size = 3;
static int fu_weights_per_filter = fu_kernel_size*fu_kernel_size;

// Test Instance and Memories
DRAM<float> fu_filter_RAM;
filter_unit testFU(fu_filter_RAM);

std::string filter_to_str(filter3x3 &filter) {
  
  std::string str = "[ ";
  for (int i = 0; i < fu_weights_per_filter; i++) {
    char tempstr[20];
    sprintf(tempstr, "%9.4f", filter.weights[i]);
    str.append(tempstr);

    // end of matrix
    if (i == fu_weights_per_filter - 1) {
      str.append(" ]\n");
    } else {
      if (i % fu_kernel_size == fu_kernel_size-1) {
        // end of line
        str.append("\n  ");
      } else {
        str.append(", ");
      }
    }
  }
  return str;
}

// Check given Filter3x3 Structure (element-wise), return true for match
bool FU_check_filter(filter3x3 &acquired, filter3x3 &expected) {

  for (int i = 0; i < fu_weights_per_filter; i++) {
    // printf("%s\n", filter_to_str(acquired).c_str());
    if (fabs(acquired.weights[i] - expected.weights[i]) > MAX_FLOAT_ERR) {
      printf("ERROR: Filters don't match.\n Acquired: \n%s\nExpected: \n%s\n",
             filter_to_str(acquired).c_str(), filter_to_str(expected).c_str());
      return false;
    }
  }
  printf("ok. ");
  return true;
}

// Main TestBench for Filter Unit.
// Checks Loading Filters from DRAM to Cache.
// Checks Getting Filters from Cache.
// Returns true if all goes well, false otherwise.
bool test_filter_unit() {

  // Test 1: Load Filters from DRAM to Cache.

  // Generate 100 Filters (locally + in DRAM)
  const int ch_in = 10;
  const int ch_out = 10;
  const KERNEL_SIZE_T kernelsize = KERNEL_SIZE_3x3;

  int mem_base_addr = 128;

  filter3x3 golden_filters[ch_in][ch_out];

  // each filter is of shape:
  // [ ch_in  ch_out  0
  //   0      0       0
  //   0      0       0 ]
  //
  // with memory layout:
  //
  //   in0out0
  //   in0out1
  //   ...
  //   in0outN
  //   in1out0
  //   in1out1
  //   ...
  //   in1outN
  //   in2out...
  //   in3out...
  //   ...
  //   inMout...
  //   inMoutN
  //
  // where each inXoutY = w0w1w2w3w4...w8
  //
  //   [ w0 w1 w2
  //     w3 w4 w5
  //     w6 w7 w8 ]
  //
  for (int in = 0; in < ch_in; in++) {
    for (int out = 0; out < ch_out; out++) {
      for (int j = 0; j < fu_weights_per_filter; j++) {
        float value = 0;
        if (j == 0)
          value = in;
        else if (j == 1)
          value = out;
        // Write Value to Golden Results
        golden_filters[in][out].weights[j] = value;
        // Write Value to RAM
        int mem_address = mem_base_addr + in * ch_out * fu_weights_per_filter +
                          out * fu_weights_per_filter + j;
        fu_filter_RAM.write(mem_address, value);
      }
    }
  }

  // Fetch Filters into FU Cache
  testFU.configure(mem_base_addr, ch_in, ch_out, kernelsize);
  testFU.fetch_filters();

  bool success = true;
  
  // Get filters from Cache, check contents
  for (int in = 0; in < ch_in; in++) {
    for (int out = 0; out < ch_out; out++) {
      filter3x3 acquired_filter = testFU.get_filter(in, out);
      success &= FU_check_filter(acquired_filter, golden_filters[in][out]);
      if (!success) break;
    }
    if (!success) break;
  }

  return success;
}
