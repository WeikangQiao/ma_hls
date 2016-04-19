//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  filter_unit.hpp
//
//  Implements the Filter Fetcher and Filter Cache.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "DRAM.hpp"
#include <ap_int.h>

// Worst-Case Memory Requirement: 64 ch. in, 256 ch. out, 3x3 filter.
// ~148k elements, 4.7Mb cache for float32.
static const int FILTER_CACHE_SIZE = 64 * 256 * 3 * 3;

// One 3x3 Filter:
//   [ w0 w1 w2
//     w3 w4 w5
//     w6 w7 w8 ]
struct filter3x3 {
  float weights[3 * 3];
};


typedef enum { KERNEL_SIZE_1x1, KERNEL_SIZE_3x3 } KERNEL_SIZE_T;

typedef int IN_CHANNELS;
typedef int OUT_CHANNELS;

// Filter Unit: Filter Fetcher and Filter Cache
class filter_unit {

private:
  // Internal Wires / Connections

  // Internal Cache Memory
  float cache[FILTER_CACHE_SIZE];
  // External RAM Interface
  DRAM<float> *ram;
  // Internal Configuration
  int _base_addr;
  IN_CHANNELS _ch_in;
  OUT_CHANNELS _ch_out;
  KERNEL_SIZE_T _kernelsize;

  // Internal Size Calculations
  int get_kernel_size();
  int get_num_filters();
  int get_elements_per_filter();

public:
  // Public Interface
  // Constructor -> link to RAM
  filter_unit(DRAM<float> &ext_ram) : ram(&ext_ram){};
  // Change Configuration
  void configure(int base_addr, int ch_in, int ch_out, KERNEL_SIZE_T kernel);
  // Fetch Filters from RAM to Cache (with previously set configuration)
  void fetch_filters();
  // Fetch Filter from RAM to Output
  filter3x3 get_filter(int ch_in, int ch_out);
};
