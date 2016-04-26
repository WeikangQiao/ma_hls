//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  image_unit.cpp
//
//  Implements the Image Fetcher, Image Cache and Active Region Cache.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "image_unit.hpp"

void image_unit::configure(int base_addr, int ch_in, int img_width,
                           int img_height) {

  _base_addr = base_addr;
  _ch_in = ch_in;
  _img_width = img_width;
  _img_height = img_height;
  _pixels_per_row = _img_width * _ch_in;
  _current_line = -1;
  _current_col = -1;
  _current_ch = -1;
}

// Memory Layout:
//  row0col0channel0
//  row0col0channel1
//  row0col0channel2
//  row0col0channel...
//  row0col0channelN
//  row0col1channel0
//  row0col1channel1
//  row0col1channel2
//  row0col1channel0
//  row0col1channel...
//  row0col1channelN
//  ...
//  row0col2
//  row0col3
//  row0col...
//  row0colM
//  row1col0
//  row1col1
//  row1col2
//  row1col...
//  row1colM
//  ...
//  row2
//  row3
//  row4
//  row...
//  rowL

// Read one Image Row (with configured width/channels) from DRAM to Cache
//  row_offset: chose row to fetch
void image_unit::fetch_row(int row) {

  // Setup Parameters and internal Variables for given row.
  set_row(row);

// Fetch all Channels in all Columns of given row:
L_COLUMN:
  for (int x = 0; x < _img_width; x++) {

  L_CHANNEL:
    for (int ch = 0; ch < _ch_in; ch++) {

      // Fetch (Col X, Channel ch)
      fetch_next_pixel();
    }
  }
}

// Set Parameters for fetching given row from RAM to Cache
//  row_offset: chose row to fetch
void image_unit::set_row(int row) {

  // Determine Cache Line to read to (direct-mapped cache)
  _current_line = row % IMAGE_CACHE_LINES;

  // Remember last row fetched
  _last_row_fetched = row;

  // Set Cache & RAM addresses for row Y, col 0, ch 0 MINUS 1
  // -> always points to last written/read pixel
  _cache_addr = _pixels_per_row * _current_line - 1;
  _ram_addr = _base_addr + _pixels_per_row * row - 1;

  // Set Column Counter
  _current_col = 0;
  _current_ch = -1;

  printf("SET_ROW: _current_line=%d, _last_row_fetched=%d, _cache_addr=%d, "
         "_ram_addr=%d, _current_col=%d, _current_ch=%d\n",
         _current_line, _last_row_fetched, _cache_addr, _ram_addr, _current_col,
         _current_ch);
}

// Fetch next Image Pixel from DRAM to Cache
// (all channels, with configured width/channels)
void image_unit::fetch_next_pixel() {

  // Update Internal Counters
  if (_current_ch == _ch_in - 1) {
    _current_col++;
    _current_ch = 0;
  } else {
    _current_ch++;
  }

  // Next Channel (and possibly next column)
  _ram_addr++;
  _cache_addr++;

  // Fetch Pixel at current Memory address
  float pixel = ram->read(_ram_addr);
  cache[_cache_addr] = pixel;
}

// Read Out given Pixel from Cache --> poor performance, use just for TestBench!
float image_unit::get_pixel(int row, int col, int ch) {

  // Check that row is actually in Cache
  if (!(row == _last_row_fetched - 0 || row == _last_row_fetched - 1 ||
        row == _last_row_fetched - 2)) {
    printf("\nERROR: last row fetched = %d, now tried %d. Not in Cache!",
           _last_row_fetched, row);
  }

  // Calculate cache line
  int pixel_cache_line = row % IMAGE_CACHE_LINES;

  // Calculate Cache Address of given Pixel
  int pixel_cache_addr = _pixels_per_row * pixel_cache_line + _ch_in * col + ch;

  float pixel = cache[pixel_cache_addr];

  if (row == 3) {
    printf("Fetching px (%2d,%2d,%2d) from line %d, addr %d, val %10.3f \n", row,
           col, ch, pixel_cache_line, pixel_cache_addr, pixel);
  }

  return pixel;
}

// Load next Column into Active Area
void image_unit::load_next_col_into_AA(active_area_3x3 &aa, int ch) {
  float new_col[3];

  for (int i = 0; i < 3; i++) {
    // determine permutation of cache lines...
    int cache_line = (_current_line + -3 + i) % IMAGE_CACHE_LINES;
    int cache_addr = _pixels_per_row * cache_line + _ch_in * _current_col + ch;
    new_col[i] = cache[cache_addr];
  }

  aa.shift_in(new_col);
}

// Shift in 3 new Pixels into Active Area Patch
void active_area_3x3::shift_in(float new_col[3]) {
  for (int row = 0; row < 3; row++) {
#pragma HLS unroll
    for (int col = 0; col < 2; col++) {
#pragma HLS unroll
      patch.pixel[3 * row + col] = patch.pixel[3 * row + col + 1];
    }
    // Shift in "from right to left"
    patch.pixel[3 * row + 2] = new_col[row];
  }
}