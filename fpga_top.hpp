//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  fpga_top.hpp
//
//  Top-Level Module for SqueezeNetOnFGPA.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef _FPGA_TOP_H_
#define _FPGA_TOP_H_

#include "netconfig.hpp"

//typedef ap_uint<64> bus_t;
typedef int bus_t;

#define TILING_CO (8)
#define TILING_CI (4)

typedef ap_uint<2> cacheline_t;
typedef ap_uint<22> imgcacheaddr_t;
typedef ap_uint<16> pixelperrow_t;
typedef ap_int<10> coordinate_t;

void fpga_top(volatile int num_layers, volatile bus_t *DRAM, 
  unsigned int byte_layerconfig_offset, unsigned int byte_weights_offset, 
  unsigned int byte_input_offset);
  
#endif
