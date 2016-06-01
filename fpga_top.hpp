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

#include <cassert>
//#include <cmath>
#include "ap_int.h"

#ifdef __SYNTHESIS__
#include <ap_utils.h>
#include <hls_math.h>
#endif

#include "network.hpp"
#include "netconfig.hpp"

// ==========================
// = Architecture Constants =
// ==========================
const int TILING_CO = (1000);
const int TILING_CI = (1000);

const int NUM_IMG_CACHE_LINES = (3);
// Actually, just need 3 Image Cache Lines
// Using 4 might simplify hardware -> check!

// ====================
// = Type Definitions =
// ====================
typedef ap_uint<2> cacheline_t;  // cache height = 4 lines
typedef ap_uint<NBITS(MAX_INPUT_PER_LAYER)> imgdramoffset_t;
typedef ap_uint<NBITS(MAX_IMAGE_CACHE_SIZE)> imgcacheaddr_t;
typedef ap_uint<NBITS(MAX_IMAGE_CACHE_SIZE / 4)> pixelperrow_t;
typedef ap_uint<4> numfilterelems_t;  // either =1 or =9

typedef ap_int<NBITS(MAX_DIMENSION) + 2> coordinate_t;
// coordinates run worst-case from -1 ... +W or +H
// -> need bits for W or H + 1 bit more for signed + 1 bit more for +H / +W
// -> could be implemented more efficiently, but coordinates become more ugly.

// ==============================
// = FPGA Top Function / Module =
// ==============================
void fpga_top(data_t *SHARED_DRAM, unsigned int num_layers,
              unsigned int weights_offset, unsigned int input_offset);

// ================================
// = Debugging Output (Helper Fn) =
// ================================
// debug mode, -DEBUG
#if defined(EBUG) && !defined(__SYNTHESIS__)
#define FNAME() \
  fprintf(stdout, "\n%s (%s, line %d)\n", __func__, __FILE__, __LINE__)
#define DBG(...)                  \
  do {                            \
    fprintf(stdout, __VA_ARGS__); \
  } while (0)
#else
#define FNAME() \
  do {          \
  } while (0)
#define DBG(...) \
  do {           \
  } while (0)
#endif  // EBUG

// ===================================================================
// = Pragma Indirection (allows use of DEFINED variables in #pragma) =
// ===================================================================
#define PRAGMA_SUB(x) _Pragma(#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)

#endif
