//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  test_image_unit.cpp
//
//  TestBench for Image Unit.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <string>
#include <cmath>
#include <stdlib.h>

#include "image_unit.hpp"

static float MAX_FLOAT_ERR = 1e-6;

// Settings
const int iutest1_ch_in = 100;
const int iutest1_w_in = 160;
const int iutest1_h_in = 120;

// Test Instance and Memories
DRAM<float> iu_image_RAM;
image_unit testIU(iu_image_RAM);
float golden_images[iutest1_h_in][iutest1_w_in][iutest1_ch_in];

void print_patch(patch3x3 &patch) {
  printf("[ ");
  for (int y = 0; y < 3; y++) {
    if (y > 0)
      printf("  ");
    for (int x = 0; x < 3; x++) {
      printf("%10.3f ", patch.pixel[y * 3 + x]);
    }
    if (y < 2)
      printf("\n");
  }
  printf(" ];\n");
}

bool check_all_patches(patch3x3 *patches, int current_col) {
  bool success = true;
  for (int ch = 0; ch < iutest1_ch_in; ch++) {
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        int col = current_col - 3 + x;
        float expected = golden_images[y][col][ch];
        float acquired = patches[ch].pixel[y * 3 + x];
        if (fabs(expected - acquired) > MAX_FLOAT_ERR) {
          printf("ERROR: result didn't match.\n"
                 "X:%d,\tY:%d,\tCh:%d\n"
                 "expected: %10.3f\nacquired: %10.3f\n",
                 col, y, ch, expected, acquired);
          success = false;
        }
      }
    }
  }
  return success;
}

// Main TestBench for Image Unit.
// Checks Loading images from DRAM to Cache.
// Checks Getting images from Cache.
// Returns true if all goes well, false otherwise.
bool test_image_unit() {

  // Test 1: Load images from DRAM to Cache.

  int mem_base_addr = 256;

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

  const int h_in = iutest1_h_in;
  const int w_in = iutest1_w_in;
  const int ch_in = iutest1_ch_in;
  active_area_3x3 aa_ch[ch_in];

  // Write to DRAM and Golden Images.
  for (int y = 0; y < h_in; y++) {
    for (int x = 0; x < w_in; x++) {
      for (int ch = 0; ch < ch_in; ch++) {
        float value = y * 1000.0 + x + ch / 1000.0;
        golden_images[y][x][ch] = value;
        int mem_addr = mem_base_addr + y * w_in * ch_in + x * ch_in + ch;
        iu_image_RAM.write(mem_addr, value);
      }
    }
  }

  // Configure Image Unit
  testIU.configure(mem_base_addr, ch_in, w_in, h_in);

  printf("Prefetching first 3 rows.\n");

  // Prefetch first three Rows
  int current_row = 0;
  testIU.fetch_row(current_row);
  testIU.fetch_row(++current_row);
  testIU.fetch_row(++current_row);

  // Check 3 Rows in IU cache (in different order)
  bool success = true;
  for (int x = 0; x < w_in; x++) {
    for (int ch = 0; ch < ch_in; ch++) {
      for (int y = 0; y < 3; y++) {
        float expected = golden_images[y][x][ch];
        float acquired = testIU.get_pixel(y, x, ch);
        // printf("%08.2f\t%08.2f\n",expected,acquired);
        if (fabs(expected - acquired) > MAX_FLOAT_ERR) {
          printf("ERROR: result didn't match.\nexpected: %10.3f\nacquired: "
                 "%10.3f\n",
                 expected, acquired);
          success = false;
        }
      }
    }
  }
  if (success)
    printf("- fetch_row checked OK.\n");
  else
    printf("- fetch_row check failed.\n");

  // Check 3 rows in AA cache
  printf("Load rows into AA cache || preload 4th row\n");

  // Preload first 3 columns
  testIU.set_row(++current_row);
  printf("loading into row: %d\n", current_row);
  int curr_col = 0;
  for (int col = 0; col < 3; col++) {
    for (int i = 0; i < ch_in; i++) {
      testIU.fetch_next_pixel();                 // Fetch new Pixel
      testIU.load_next_col_into_AA(aa_ch[i], i); // then load into AA
    }
    curr_col++;
  }

  // Check 3 Rows x 3 Cols in all AA SREGs

  // Fetch Data from AA
  patch3x3 patch[ch_in];
  for (int i = 0; i < ch_in; i++)
    patch[i] = aa_ch[i].get_patch();

  // Print Out AA SREGs content
  printf("Example SREG Content CH42:\n");
  print_patch(patch[42]);

  // Check all Data Elements
  success &= check_all_patches(patch, curr_col);

  if (success)
    printf("- active area SREG memory ok.\n");
  else
    printf("- active area SREG memory check FAILED!\n");

  printf("Load 4th column, and shift AA right.\n");
  for (int i = 0; i < ch_in; i++) {
    testIU.fetch_next_pixel();                 // Fetch new Pixel
    testIU.load_next_col_into_AA(aa_ch[i], i); // then load into AA
    patch[i] = aa_ch[i].get_patch();           // and fetch from there
  }
  curr_col++;

  // Print Out AA SREGs content
  printf("Example SREG Content CH42:\n");
  print_patch(patch[42]);

  // Check all Data Elements in AA Patches
  success &= check_all_patches(patch, curr_col);

  printf("Load 5th to 160th column, and shift AA right.\n");

  for (int i = 4; i < w_in; i++) {
    for (int j = 0; j < ch_in; j++) {
      testIU.fetch_next_pixel();                 // Fetch new Pixel
      testIU.load_next_col_into_AA(aa_ch[i], i); // then load into AA
      //patch[i] = aa_ch[i].get_patch();           // and fetch from there
    }
    curr_col++;
  }

  // Print Out AA SREGs content
  printf("Example SREG Content CH42:\n");
  print_patch(patch[42]);

  // Check all Data Elements in AA Patches
  success &= check_all_patches(patch, curr_col-3);

  testIU.set_row(++current_row);

  printf("\nIMAGE UNIT TEST!\n\n   !!!TODO!!!\n\n");

  return success;
}
