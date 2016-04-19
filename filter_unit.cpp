//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  filter_unit.cpp
//
//  Implements the Filter Fetcher and Filter Cache.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "filter_unit.hpp"

// Reconfigure Unit
void filter_unit::configure(int base_addr, int ch_in, int ch_out,
                            KERNEL_SIZE_T kernel) {
  _base_addr = base_addr;
  _ch_in = ch_in;
  _ch_out = ch_out;
  _kernelsize = kernel;
}

int filter_unit::get_kernel_size() {
  if (_kernelsize == KERNEL_SIZE_1x1)
    return 1;
  else if (_kernelsize == KERNEL_SIZE_3x3)
    return 3;
  else
    return 0;
}

int filter_unit::get_num_filters() { return _ch_in * _ch_out; }

int filter_unit::get_elements_per_filter() {
  if (_kernelsize == KERNEL_SIZE_1x1)
    return 1;
  else if (_kernelsize == KERNEL_SIZE_3x3)
    return 9;
  else
    return 0;
}

void filter_unit::fetch_filters() {

  int num_filters = get_num_filters() * get_elements_per_filter();

#pragma HLS pipeline
#pragma HLS unroll factor = 2
L_FETCH:
  for (int i = 0; i < num_filters; i++) {
    int memory_addr = _base_addr + i;
    float weight = ram->read(memory_addr);
    cache[i] = weight;
  }
}

filter3x3 filter_unit::get_filter(int ch_in, int ch_out) {
  int elements_per_ch_out = get_elements_per_filter();
  int elements_per_ch_in = _ch_out * elements_per_ch_out;
  int cache_addr = elements_per_ch_in * ch_in + elements_per_ch_out * ch_out;

  filter3x3 filter;

  for (int i = 0; i < elements_per_ch_out; i++) {
    filter.weights[i] = cache[cache_addr];
    cache_addr = cache_addr + 1;
  }

  return filter;
}