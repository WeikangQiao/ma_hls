//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  fpga_top.cpp
//
//  Top-Level Module for SqueezeNetOnFGPA.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "fpga_top.hpp"
#include "math.h"
#include "ap_int.h"

#include "miniFire.hpp"

///////////////
///////////////
// WARNING   //
///////////////
// DISABLES ALL PRINTF STATEMENTS!
///////////////
//#define printf(...) {};
///////////////
// WARNING   //
///////////////
///////////////

// FPGA BRAM Memory
int NUM_LAYERS;
float FILTER_CACHE[MAX_WEIGHTS_PER_LAYER];
float IMAGE_CACHE[MAX_IMAGE_CACHE_SIZE];
float ACTIVE_AREA[TILING_CI][9];
float OUTPUT_CACHE[MAX_NUM_CHOUT];
layer_t BRAM_LAYER_CONFIG[MAX_NUM_LAYERS];

void fpga_top(volatile int num_layers, volatile bus_t *DRAM,
              unsigned int byte_layerconfig_offset,
              unsigned int byte_weights_offset,
              unsigned int byte_input_offset) {

// Interface Specification:

// AXI4 Master
// ap_bus + AXI4 Master for connection to DRAM (via DMA)
#pragma HLS INTERFACE ap_bus port = DRAM
#pragma HLS RESOURCE core = AXI4M variable = DRAM

// AXI4 Lite + Registers for Configuration Data
#pragma HLS RESOURCE core = AXI4LiteS variable = return metadata =             \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = num_layers
#pragma HLS RESOURCE core = AXI4LiteS variable = weights_offset metadata =     \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = weights_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = weights_offset metadata =     \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = layerconfig_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = layerconfig_offset metadata = \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = input_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = input_offset metadata =       \
    "-bus_bundle LITE"

  printf("Start FPGA Module:\n====================\n\n");

  // DRAM Memory Pointers
  void *DRAM_LAYERCONFIG = ((char *)DRAM + byte_layerconfig_offset);
  void *DRAM_WEIGHTS = ((char *)DRAM + byte_weights_offset);
  void *DRAM_DATA = ((char *)DRAM + byte_input_offset);

  printf("FPGA: Fetch Layer Configuration from DRAM to BRAM\n");

  int layercfg_size = num_layers * sizeof(layer_t);
  memcpy(BRAM_LAYER_CONFIG, DRAM_LAYERCONFIG, layercfg_size);

  printf("Bytes copied: %d Bytes\n",
         (int)ceilf(layercfg_size * 8 / 64.0) * 64 / 8);

  // Global Registers

  layer_t layer;
  layernum_t current_layer;
  coordinate_t x;
  coordinate_t y;
  dimension_t width_out;
  dimension_t height_out;
  channel_t ci;
  channel_t co;
  cacheline_t current_img_cache_line;
  imgcacheaddr_t img_cache_addr;
  imgcacheaddr_t img_dram_offset;
  pixelperrow_t pixels_per_row;

  ////////////////////////
  /// Loop Level 1: Layers
  //
  for (current_layer = 0; current_layer < num_layers; current_layer++) {

    // Load Layer Configuration from BRAM

    layer = BRAM_LAYER_CONFIG[current_layer];
    printf("\nL%-2d", (int)current_layer);
    printf("%-6s ", layer.name);

    if (layer.type == LAYER_DATA) {
      printf("data layer: nothing to be done, skip.\n");
      continue;
    }
    if (layer.type == LAYER_POOL) {
      printf("!! pooling layer: cannot execute, skip.\n");
      continue;
    }
    if (layer.type == LAYER_CONV) {
      printf("convolution layer, K=%d, S=%d, P=%d\n", (int)layer.kernel,
             (int)layer.stride, (int)layer.pad);
    }

    printf(
        "    mem_addr_input: %d, mem_addr_output: %d, mem_addr_weights: %d\n",
        layer.mem_addr_input, layer.mem_addr_output, layer.mem_addr_weights);

    // Assertions / Constraints to enable Optimizations:
    assert(layer.width == 0 | layer.width == 1 | layer.width == 2 |
           layer.width == 4 | layer.width == 8 | layer.width == 16 |
           layer.width == 32 | layer.width == 64 | layer.width == 128 |
           layer.width == 256 | layer.width == 512 | layer.width == 1024);
    assert(layer.width == 0 | layer.height == 1 | layer.height == 2 |
           layer.height == 4 | layer.height == 8 | layer.height == 16 |
           layer.height == 32 | layer.height == 64 | layer.height == 128 |
           layer.height == 256 | layer.height == 512 | layer.height == 1024);
    assert(layer.channels_in == 1 | layer.channels_in == 2 |
           layer.channels_in == 3 | layer.channels_in == 4 |
           layer.channels_in == 8 | layer.channels_in == 16 |
           layer.channels_in == 32 | layer.channels_in == 64 |
           layer.channels_in == 128 | layer.channels_in == 256 |
           layer.channels_in == 512 | layer.channels_in == 1024 |
           layer.channels_in == 1000);
    assert(layer.channels_out == 1 | layer.channels_out == 2 |
           layer.channels_out == 4 | layer.channels_out == 8 |
           layer.channels_out == 10 | layer.channels_out == 16 |
           layer.channels_out == 32 | layer.channels_out == 64 |
           layer.channels_out == 128 | layer.channels_out == 256 |
           layer.channels_out == 512 | layer.channels_out == 1024 |
           layer.channels_in == 1000);
    assert(layer.pad == 0 | layer.pad == 1);
    assert(layer.stride == 1 | layer.stride == 2);
    assert(layer.kernel == 1 | layer.kernel == 3);

    // Setup Layer-Specific Registers:

    width_out = layer.width;
    height_out = layer.height;
    if (layer.stride == 2) {
      width_out = width_out / 2;
      height_out = height_out / 2;
    }
    pixels_per_row = layer.width * layer.channels_in;

    // Setup Image Cache

    float *DRAM_INPUT_PTR = ((float *)DRAM_DATA + layer.mem_addr_input);
    int BYTES_PER_ROW = pixels_per_row * sizeof(float);
    int BYTES_PER_IMG = layer.height * BYTES_PER_ROW;
    printf("    setup image cache:   will fetch from %lu (preload %dkB, total "
           "%dkB)\n",
           (unsigned long)DRAM_INPUT_PTR, BYTES_PER_ROW / 1024,
           BYTES_PER_IMG / 1024);

    // Setup Weights Cache

    if (layer.type == LAYER_CONV) {
      float *DRAM_WEIGHTS_PTR =
          ((float *)DRAM_WEIGHTS + layer.mem_addr_weights);
      int FILTER_BYTES = (layer.channels_out +
                          layer.kernel * layer.kernel * layer.channels_in *
                              layer.channels_out) *
                         sizeof(float);
      printf("    setup weights cache: will fetch from %lu (%dkB)\n",
             (unsigned long)DRAM_WEIGHTS_PTR, FILTER_BYTES / 1024);
      memcpy(FILTER_CACHE, DRAM_WEIGHTS_PTR, FILTER_BYTES);
    }

    // Setup Output Cache
    float *DRAM_OUTPUT_PTR = ((float *)DRAM_DATA + layer.mem_addr_output);

    // Setup Convolution Kernels
    int num_ch_out_per_convolution;
    if (layer.kernel == 3)
      num_ch_out_per_convolution = 1;
    else
      num_ch_out_per_convolution = 9;
    assert(num_ch_out_per_convolution == 1 | num_ch_out_per_convolution == 9);

    ////////////////////////
    /// Loop Level 2: Rows Y

    // Convolution Scheduling:
    // =======================
    //
    // Case 1: 3x3, 1px Padding
    // ````````````````````````````
    // need 1 row padding (not necessarily executed)
    // need 1 row preload
    // then (1px padding, 1px preload, width pixels convolutions) x (height)
    // Y: (-1), 0, 1, 2, ... , H-1, H  -1,H = pad  0 = preload  1..H = output
    // X: (-1), 0, 1, 2, ... , W-1, W  -1,W = pad  0 = preload  1..W = output
    // Output Coord: Yout = Y-1 (0..H-1), Xout = X-1 (0..W-1)
    // Total: (WIDTH+2) x (HEIGHT+2) Operations, (WIDTH) x (HEIGHT) Outputs
    //
    // Case 2: 1x1, NO Padding, NOT Packed
    // ````````````````````````````
    // (no preload, width pixels convolutions) x (height rows)
    // Total: WIDTH x HEIGHT Operations
    //
    // Case 3: 1x1, Packed into 3x3 Kernel
    // ````````````````````````````
    // -> ActiveArea contains 9x same pixel, Filter contains 9 output channels
    // no padding, no preload
    // then (width pixels convolutions) x (height)
    // filter loading in batches of 9 (output channel loop)
    // Total: WIDTH x HEIGHT Operations
    //

    // Make sure that all 3x3 kernels use padding, all 1x1 kernels don't.
    assert((layer.kernel == 3 & layer.pad == 1) |
           (layer.kernel == 1 & layer.pad == 0) | (layer.type == LAYER_POOL));

    printf("    start processing rows...\n");

    for (y = -layer.pad; y < layer.height + layer.pad; y++) {

      if (y == -1) { // top padding row, nothing to do.
        printf("    skip top padding row\n");
        continue;
      }

      // Allow Optimizations based on possible values of y:
      // assert(y >= 0 & y <= layer.height);

      printf("L%-2dR%-3d ", (int)current_layer, (int)y);

      // Setup Image Cache:
      /////////

      // Set Image Cache Row (defines where to fetch to, and permutation)

      current_img_cache_line = y % 4;

      // Set Cache + DRAM Addresses to pixel 0, channel 0 of current row.
      // Image Cache + DRAM are linearly accessed.

      img_cache_addr = pixels_per_row * current_img_cache_line;
      img_dram_offset = pixels_per_row * y;

      printf("setup image cache: to cache line %d, addr %d; DRAM offset %d\n",
             (int)current_img_cache_line, (int)img_cache_addr,
             (int)img_dram_offset);

      ////////////////////////
      /// Loop Level 3: Columns X
      //

      printf("        start processing columns...\n");

      for (x = -layer.pad; x < layer.width + layer.pad; x++) {

        printf("L%-2dR%-3dC%-3d ", (int)current_layer, (int)y, (int)x);

        // Check type of current Pixel:
        // For kernel 3x3 (with padding):
        // Y: 0, 1,..., H-1, H             H = pad  0 = preload  1..H = output
        // X: (-1), 0, 1,..., W-1, W    -1,W = pad  0 = preload  1..W = output
        // For kernel 1x1, packed3x3 (no padding):
        // (no preload, width pixels convolutions) x (height rows)

        bool is_preload_row = (layer.pad & (y == 0)); // don't push to AA SREG
        bool do_pad_top_row = (layer.pad & (y == 1)); // pad row0 of AA SREG
        bool do_pad_bot_row = (layer.pad & (y == layer.height)); // pad row2 ...

        bool is_preload_col = (layer.pad & ((x == -1) | (x == 0))); // no conv
        bool do_pad_curr_col = (layer.pad & ((x == -1) | (x == layer.width)));

        bool is_valid_pixel =
            ((x >= 0) & (x < layer.width)) & ((y >= 0) & (y < layer.height));

        /*printf("Pixel (%d,%d) Characteristics:\n  is_preload_row  = %d\n"
               "  do_pad_top_row  = %d\n  do_pad_bot_row  = %d\n"
               "  is_preload_col  = %d\n  do_pad_curr_col = %d\n"
               "  is_valid_pixel  = %d\n",
               (int)y, (int)x, is_preload_row, do_pad_top_row, do_pad_bot_row,
               is_preload_col, do_pad_curr_col, is_valid_pixel);*/
        printf("pixel (%d,%d) (  ", (int)y, (int)x);
        if (is_preload_row)
          printf("is_preload_row  ");
        if (do_pad_top_row)
          printf("do_pad_top_row  ");
        if (do_pad_bot_row)
          printf("do_pad_bot_row  ");
        if (is_preload_col)
          printf("is_preload_col  ");
        if (do_pad_curr_col)
          printf("do_pad_curr_col  ");
        if (is_valid_pixel)
          printf("is_valid_pixel  ");
        printf(")\n");

        ////////////////////////
        /// Loop Level 4: Input Channels Ci

        printf("            start processing input channels...\n");

        for (ci = 0; ci < layer.channels_in; ci++) {

          printf("L%-2dR%-3dC%-3dI%-2d ", (int)current_layer, (int)y, (int)x,
                 (int)ci);

          // Load Next Pixel/Channel from RAM:

          float fetched_pixel = -99;
          if (is_valid_pixel) {
            memcpy(&fetched_pixel, DRAM_INPUT_PTR + img_dram_offset,
                   sizeof(float));
            IMAGE_CACHE[img_cache_addr] = fetched_pixel;
            printf("load next input channel from RAM: @%lu->@%lu (%010.3f)\n",
                   (unsigned long)(DRAM_INPUT_PTR + img_dram_offset) &
                       0xFFFFFFFFFFF,
                   (unsigned long)&IMAGE_CACHE[img_cache_addr] & 0xFFFFFFFFFFF,
                   fetched_pixel);
            // advance image cache and dram access addresses
            img_cache_addr++;
            img_dram_offset++;
          } else {
            printf("skip fetch-from-RAM (not a valid pixel (%d,%d))\n", (int)y,
                   (int)x);
          }

          if (is_preload_row) { // preload row; load next input channel
            printf("               skip to next channel (preload row)\n");
            continue;
          }

          // Shift Into Active Area SREG:

          channel_t current_aa = ci % TILING_CI;
          if (layer.kernel == 1) {
            printf("               shift one pixel into ActiveArea SREG %d "
                   "(%010.4f)",
                   (int)current_aa, fetched_pixel);
            for (int j = 0; j < 3; j++) {
#pragma HLS unroll
              for (int i = 0; i < 3; i++) {
#pragma HLS unroll
                ACTIVE_AREA[current_aa][j * 3 + i] = fetched_pixel;
              }
            }
          } else {
            printf("               shift column into ActiveArea SREG %d ( ",
                   (int)current_aa);
            for (int j = 0; j < 3; j++) {
#pragma HLS unroll
              for (int i = 0; i < 2; i++) {
#pragma HLS unroll
                // shift contents left
                ACTIVE_AREA[current_aa][j * 3 + i] =
                    ACTIVE_AREA[current_aa][j * 3 + i + 1];
              }
              { // shift in new pixels right
                bool do_pad_pixel =
                    (((j == 0) & do_pad_top_row) | ((j == 2) & do_pad_bot_row) |
                     do_pad_curr_col);
                if (do_pad_pixel) { // top/bottom row or left/right col padding:
                  printf("0 ");
                  ACTIVE_AREA[current_aa][j * 3 + 2] = 0;
                } else { // normal pixel - load from BRAM
                  cacheline_t request_line = (y + j - 2) % 4;
                  float cached = IMAGE_CACHE[request_line * pixels_per_row +
                                             layer.channels_in * x + ci];
                  ACTIVE_AREA[current_aa][j * 3 + 2] = cached;
                  printf("%010.3f ", cached);
                }
              }
            }
          }
          printf(")\n");

          if (is_preload_col) { // not enough cols buffered; next ci
            printf("               skip to next channel (preload column)\n");
            continue;
          }

          if ((layer.stride == 2) & (y % 2)) { // odd row left out in stride2
            printf("               skip to next channel (odd row, S=2)\n");
            continue;
          }

          if ((layer.stride == 2) & (x % 2)) { // odd col left out in stride2
            printf("               skip to next channel (odd col, S=2)\n");
            continue;
          }

          ////////////////////////
          /// Loop Level 5: Output Channels Co

          // printf("               start processing Output Channels..\n");

          for (co = 0; co < layer.channels_out;
               co += num_ch_out_per_convolution) {

            if (co == 0)
              printf("               %dx MACC (%dx%d), channels: ",
                     (int)layer.channels_out, (int)layer.kernel,
                     (int)layer.kernel);

            for (int l = 0; l < num_ch_out_per_convolution; l++) {
              if ((co + l) < layer.channels_out) {
                printf("(%d>%d) ", (int)ci, (int)co + l);
              }
            }
            if (layer.kernel == 1)
              printf("[packed] ");

            /*printf("# LAYER %2d, ROW %3d/%3d, COL: %3d/%3d, "
                   "CH_IN: %2d/%2d, CH_OUT: %2d/%2d: ",
                   (int)current_layer, (int)y, (int)layer.height - 1, (int)x,
                   (int)layer.width - 1, (int)ci, (int)layer.channels_in - 1,
                   (int)co, (int)layer.channels_out - 1);
            */

            ////////////////////////
            /// 2D CONVOLUTION (for this output channel)

            float conv2D = 0;
            float products[9];
            kernel_t i, j;

            ////////////////////////
            /// Loop Level 6: Kernel H (j)
            /// Loop Level 7: Kernel W (i)

            for (j = 0; j < 3; j++) {
#pragma HLS unroll
              for (i = 0; i < 3; i++) {
#pragma HLS unroll
                // Filter Arrangement in Memory:
                // ch_in -> ch_out -> kernel_h -> kernel_w
                // for 3x3: ch_in -> ch_out -> 9
                // for 1x1: ch_in -> ch_out -> 1
                int filter_addr =
                    ci * (layer.channels_out * 9) + co * 9 + j * 3 + i;
                float multiply_result = FILTER_CACHE[filter_addr] *
                                        ACTIVE_AREA[current_aa][j * 3 + i];
                conv2D += multiply_result;
                products[j * 3 + i] = multiply_result;
              }
            }

            // Write to Output Cache
            // For 3x3 Kernel: Write MACC Result to Output Cache
            if (layer.kernel == 3) {
              if (ci == 0) {
                // printf("initialize output cache %d\n", (int)co);
                OUTPUT_CACHE[co] = conv2D + 0;
                printf("i");
              } else {
                // printf("update     output cache %d\n", (int)co);
                OUTPUT_CACHE[co] = conv2D + OUTPUT_CACHE[co];
                printf("u");
              }
            } else if (layer.kernel == 1) {
              // For 1x1 Kernel: Write 9 Multiply-Results to Cache
              for (int l = 0; l < 9; l++) {
                if ((co + l) < layer.channels_out) {
                  if (ci == 0) {
                    // printf("initialize output cache %d\n", (int)co + l);
                    OUTPUT_CACHE[co + l] = products[l] + 0;
                    printf("i");
                  } else {
                    // printf("update     output cache %d\n", (int)co + l);
                    OUTPUT_CACHE[co + l] = products[l] + OUTPUT_CACHE[co + l];
                    printf("u");
                  }
                }
              }
            } // end else if kernel == 1

          } // end for co
          printf("\n");

          // one input channel done.

        } // end for ci

        // one pixel done. (all input channels, all output channels)

        // If pixel didn't produce output, no preprocessing is needed
        if (is_preload_row | is_preload_col) {
          printf("            no output for pixel -> no postprocessing\n");
          continue;
        }
        // If we're at stride 2 and row or col is odd, skip to next pixel
        // ( -> came here from "continue" in input-channel loop)
        if ((layer.stride == 2) & ((x % 2) | (y % 2))) {
          printf("            skip to next pixel (odd row or col, S=2)\n");
          continue;
        }

        /////////////////
        /// Post-Processing of Pixel

        printf("            postprocess:\n");

        // Calculate Output Write Address
        dimension_t y_out = y, x_out = x;
        if (layer.stride == 2) {
          y_out = y_out / 2;
          x_out = x_out / 2;
        }
        if (layer.kernel == 3 & layer.pad) {
          y_out -= 1;
          x_out -= 1;
        }
        printf("            saving as pixel(%d,%d)\n", (int)y_out, (int)x_out);

        float *DRAM_OUTPUT_PIXEL_PTR =
            (DRAM_OUTPUT_PTR + (y_out) * (layer.channels_out * width_out) +
             (x_out) * (layer.channels_out));
        if (layer.is_expand_layer) {
          // Expand Layers are interleaved -> leave space for 2x channels_out
          DRAM_OUTPUT_PIXEL_PTR += x_out * (layer.channels_out);
        }

        for (co = 0; co < layer.channels_out; co++) {

          float value = OUTPUT_CACHE[co];

          //////////
          /// ReLU
          printf("            ch%2d: %9.3f->", (int)co, value);
          if (value < 0)
            value = 0;
          printf("%9.3f, ", value);

          //////////
          /// Pooling
          // TODO
          printf("(pool), ");

          //////////
          /// Writeback
          printf("WB(@%lu)\n",
                 (unsigned long)(DRAM_OUTPUT_PIXEL_PTR + co) & 0xFFFFFFFFFFF);
          memcpy(DRAM_OUTPUT_PIXEL_PTR + co, &value, sizeof(value));
        }

      } // end for x
    }   // end for y
  }     // end for layer

  printf("\nALL LAYERS DONE!\n");
}