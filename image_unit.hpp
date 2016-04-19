//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  image_unit.hpp
//
//  Implements the Image Fetcher, Image Cache and Active Region Cache.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "DRAM.hpp"
#include <assert.h> /* assert */
#include <cstdio>

// Worst-Case Image Cache needed: 512 channels, 32px wide, 4 lines cache:
// ~55k elements, 1.8Mb for float32
static const int IMAGE_CACHE_LINES = 4;
static const int IMAGE_CACHE_SIZE = 512 * 32 * IMAGE_CACHE_LINES;
static const int WINDOW_SIZE = 3;
static const int NUM_WINDOWS = 8;

// Memory Layout:
//   [ p0 p1 p2
//     p3 p4 p5
//     p6 p7 p8 ]
struct patch3x3 {
  float pixel[3 * 3];
};

class active_area_3x3 {
private:
// Shift-Register Memory (dim=0: partitioned into individual regs in all dims)
#pragma HLS ARRAY_PARTITION variable = patch complete dim = 0
  patch3x3 patch;

public:
  // Shift in 3 new Pixels into Patch
  void shift_in(float new_col[3]);
  // Expose Patch to public
  patch3x3 get_patch() { return patch; }
  
};

class image_unit {

private:
  // Internal Wires / Connections

  // Internal Cache Memory (BRAM)
  float cache[IMAGE_CACHE_SIZE];

  // External RAM Interface
  DRAM<float> *ram;

  // Internal Configuration
  int _base_addr;
  int _ch_in;
  int _img_width;
  int _img_height;
  int _pixels_per_row;

  // State Variables
  int _last_row_fetched; // which rows are in Cache?
  int _current_line;
  int _ram_addr;
  int _cache_addr;
  int _current_col;
  int _current_ch;

public:
  // Public Interface
  // Constructor -> link to RAM
  image_unit(DRAM<float> &ext_ram) : ram(&ext_ram){};

  // Change Configuration
  void configure(int base_addr, int ch_in, int img_width, int img_height);

  // Setup Addresses for fetching given row
  void set_row(int row);

  // Fetch given Image Row from DRAM into Cache
  // (with previously set configuration)
  void fetch_row(int row);

  // Fetch next Pixel into Cache (all channels)
  // (with previously set configuration)
  void fetch_next_pixel();
  
  // Load next Column into given Active Area Patch
  void load_next_col_into_AA(active_area_3x3 &aa, int ch);

  // Read out one pixel; poor performance --> use only for TestBench!
  float get_pixel(int row, int col, int ch);

};
