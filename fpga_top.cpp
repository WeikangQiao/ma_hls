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

#include "network.hpp"

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
float GLOBAL_POOL_CACHE[MAX_NUM_CHOUT];
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
#pragma HLS RESOURCE core = AXI4LiteS variable = return metadata = \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = num_layers
#pragma HLS RESOURCE core = AXI4LiteS variable = weights_offset metadata = \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = weights_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = weights_offset metadata = \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = layerconfig_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = layerconfig_offset metadata = \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = input_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = input_offset metadata = \
    "-bus_bundle LITE"

  printf("Start FPGA Module:\n====================\n\n");

  // DRAM Memory Pointers
  layer_t *DRAM_LAYERCONFIG =
      (layer_t *)((char *)DRAM + byte_layerconfig_offset);
  float *DRAM_WEIGHTS = (float *)((char *)DRAM + byte_weights_offset);
  float *DRAM_DATA = (float *)((char *)DRAM + byte_input_offset);
  float *DRAM_OUTPUT_PIXEL_PTR;

  printf("FPGA: Fetch Layer Configuration from DRAM to BRAM\n");

  int layercfg_size = num_layers * sizeof(layer_t);
  memcpy(BRAM_LAYER_CONFIG, DRAM_LAYERCONFIG, layercfg_size);

  DBG("Bytes copied: %d Bytes\n", (int)layercfg_size);

  // Global Registers

  layer_t layer;
  layernum_t current_layer;
  coordinate_t x, y;
  coordinate_t y_out, x_out;
  dimension_t width_out, height_out;
  channel_t ci;
  channel_t co;
  cacheline_t current_img_cache_line;
  imgcacheaddr_t img_cache_addr;
  imgcacheaddr_t img_dram_offset;
  weightaddr_t weight_addr;
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
      DBG("data layer: nothing to be done, skip.\n");
      continue;
    }
    if (layer.type == LAYER_POOL) {
      DBG("!! pooling layer: cannot execute, skip.\n");
      continue;
    }
    if (layer.type == LAYER_CONV) {
      printf("convolution layer, K=%d, S=%d, P=%d\n", (int)layer.kernel,
             (int)layer.stride, (int)layer.pad);
    }

    DBG("    mem_addr_input: %d, mem_addr_output: %d, mem_addr_weights: %d\n",
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
           layer.channels_out == 1000);
    assert(layer.pad == 0 | layer.pad == 1);
    assert(layer.stride == 1 | layer.stride == 2);
    assert(layer.kernel == 1 | layer.kernel == 3);
    // Make sure that all 3x3 kernels use padding, all 1x1 kernels don't.
    assert((layer.kernel == 3 & layer.pad == 1) |
           (layer.kernel == 1 & layer.pad == 0) | (layer.type == LAYER_POOL));

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
    DBG(
        "    setup image cache:   will fetch from %lu (preload %dkB, total "
        "%dkB)\n",
        (unsigned long)DRAM_INPUT_PTR, BYTES_PER_ROW / 1024,
        BYTES_PER_IMG / 1024);

    // Setup Weights Cache

    int num_weights_in_layer =
        layer.kernel * layer.kernel * layer.channels_in * layer.channels_out +
        layer.channels_out;
    if (layer.type == LAYER_CONV) {
      float *DRAM_WEIGHTS_PTR =
          ((float *)DRAM_WEIGHTS + layer.mem_addr_weights);
      int FILTER_BYTES = num_weights_in_layer * sizeof(float);
      DBG("    setup weights cache: will fetch from %lu (%dkB)\n",
          (unsigned long)DRAM_WEIGHTS_PTR, FILTER_BYTES / 1024);
      memcpy(FILTER_CACHE, DRAM_WEIGHTS_PTR, FILTER_BYTES);
      weight_addr = 0;
    }

    // Setup Output Cache
    float *DRAM_OUTPUT_PTR = ((float *)DRAM_DATA + layer.mem_addr_output);

    // Setup Convolution Kernels
    int num_ch_out_per_convolution;
    int weights_per_filter;
    if (layer.kernel == 3) {
      num_ch_out_per_convolution = 1;
      weights_per_filter = 9;
    } else {
      num_ch_out_per_convolution = 9;
      weights_per_filter = 1;
    }
    assert(num_ch_out_per_convolution == 1 | num_ch_out_per_convolution == 9);
    assert(weights_per_filter == 1 | weights_per_filter == 9);

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

    DBG("    start processing rows...\n");

    for (y = -layer.pad; y < layer.height + layer.pad; y++) {
      if (y == -1) {  // top padding row, nothing to do.
        DBG("        skip top padding row\n");
        continue;
      }

      // Allow Optimizations based on possible values of y:
      assert(y >= 0 & y <= layer.height);

      DBG("L%-2dR%-3d ", (int)current_layer, (int)y);

      // Setup Image Cache:
      /////////

      // Set Image Cache Row (defines where to fetch to, and permutation)

      current_img_cache_line = y % 4;

      // Set Cache + DRAM Addresses to pixel 0, channel 0 of current row.
      // Image Cache + DRAM are linearly accessed.

      img_cache_addr = pixels_per_row * current_img_cache_line;
      img_dram_offset = pixels_per_row * y;

      DBG("setup image cache: to cache line %d, addr %d; DRAM offset %d\n",
          (int)current_img_cache_line, (int)img_cache_addr,
          (int)img_dram_offset);

      ////////////////////////
      /// Loop Level 3: Columns X
      //

      DBG("        start processing columns...\n");

      for (x = -layer.pad; x < layer.width + layer.pad; x++) {
        DBG("L%-2dR%-3dC%-3d ", (int)current_layer, (int)y, (int)x);

        // Calculate Coordinates of Output Pixel
        x_out = x;
        y_out = y;
        if (layer.kernel == 3) {
          x_out -= 1;
          y_out -= 1;
        }
        if (layer.stride == 2) {
          y_out = y_out / 2;
          x_out = x_out / 2;
        }

        // Check type of current Pixel:
        // For kernel 3x3 (with padding):
        // Y: 0, 1,..., H-1, H             H = pad  0 = preload  1..H = output
        // X: (-1), 0, 1,..., W-1, W    -1,W = pad  0 = preload  1..W = output
        // For kernel 1x1, packed3x3 (no padding):
        // (no preload, width pixels convolutions) x (height rows)

        bool is_preload_row = (layer.pad & (y == 0));  // don't push to AA SREG
        bool do_pad_top_row =
            (layer.pad & (y == 1));  // pad top row (row0)  of AA SREG
        bool do_pad_bot_row =
            (layer.pad &
             (y == layer.height));  // pad bottom row (row2) of AA SREG

        bool is_preload_col = (layer.pad & ((x == -1) | (x == 0)));  // no conv
        bool do_pad_curr_col = (layer.pad & ((x == -1) | (x == layer.width)));

        bool is_valid_input_coord =
            ((x >= 0) & (x < layer.width)) & ((y >= 0) & (y < layer.height));

        // Odd rows/cols:
        // THE RATIONAL WAY:
        // We want to keep (output) rows/cols [0, 2, 4, 6, ...]
        // If we have 3x3 and pad=1, this is x or y = [1, 3, 5, ...]
        // If we have 1x1 and pad=0, this is x or y = [0, 2, 4, ...]
        // -> all other x/y are considered 'odd' and skipped:
        // for pad=1, skip 0,2,4,... (x%2==0)
        // for pad=0, skip 1,3,5,... (x%2==1)
        bool is_odd_input_row = ((layer.pad == 0) & (y % 2 == 1)) |
                                ((layer.pad == 1) & (y % 2 == 0));
        bool is_odd_input_col = ((layer.pad == 0) & (x % 2 == 1)) |
                                ((layer.pad == 1) & (x % 2 == 0));

        // Pixel Characterstics:
        DBG("input pixel (%d,%d) (  ", (int)y, (int)x);
        if (is_preload_row) DBG("is_preload_row  ");
        if (do_pad_top_row) DBG("do_pad_top_row  ");
        if (do_pad_bot_row) DBG("do_pad_bot_row  ");
        if (is_preload_col) DBG("is_preload_col  ");
        if (do_pad_curr_col) DBG("do_pad_curr_col  ");
        if (is_odd_input_row) DBG("odd_row  ");
        if (is_odd_input_col) DBG("odd_col  ");
        if (is_valid_input_coord) DBG("is_valid_input_coord  ");
        DBG(")\n");

        ////////////////////////
        /// Loop Level 4: Input Channels Ci

        DBG("            start processing input channels...\n");

        for (ci = 0; ci < layer.channels_in; ci++) {
          DBG("L%-2dR%-3dC%-3dI%-2d ", (int)current_layer, (int)y, (int)x,
              (int)ci);

          // Load Next Pixel/Channel from RAM:
          float fetched_pixel = -99;
          if (is_valid_input_coord) {
            // copy from DRAM into IMAGE Cache
            memcpy(&fetched_pixel, DRAM_INPUT_PTR + img_dram_offset,
                   sizeof(float));
            IMAGE_CACHE[img_cache_addr] = fetched_pixel;
            DBG("load next input channel from RAM: @%lu->@%lu (%010.3f)\n",
                ((long)(DRAM_INPUT_PTR + img_dram_offset) - (long)DRAM_DATA),
                (long)&IMAGE_CACHE[img_cache_addr], fetched_pixel);
            // advance image cache and dram access addresses
            img_cache_addr++;
            img_dram_offset++;
          } else {
            // not a valid pixel -> skip
            DBG("skip fetch-from-RAM (not a valid pixel (%d,%d))\n", (int)y,
                (int)x);
          }

          // For preload rows, only store into IMAGE Cache, skip rest.
          if (is_preload_row) {
            DBG("               skip to next channel (preload row)\n");
            continue;
          }

          // Shift Into Active Area SREG:
          channel_t current_aa = ci % TILING_CI;
          if (layer.kernel == 1) {
            DBG(
                "               shift single pixel into ActiveArea SREG %d "
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
            DBG("               shift column into ActiveArea SREG %d ( ",
                (int)current_aa);
            for (int j = 0; j < 3; j++) {
#pragma HLS unroll
              for (int i = 0; i < 2; i++) {
#pragma HLS unroll
                // shift contents left
                ACTIVE_AREA[current_aa][j * 3 + i] =
                    ACTIVE_AREA[current_aa][j * 3 + i + 1];
              }
              {  // shift in new pixels right
                bool do_pad_pixel =
                    (((j == 0) & do_pad_top_row) | ((j == 2) & do_pad_bot_row) |
                     do_pad_curr_col);
                if (do_pad_pixel) {  // top/bottom row or left/right col
                                     // padding:
                  DBG("0 ");
                  ACTIVE_AREA[current_aa][j * 3 + 2] = 0;
                } else {  // normal pixel - load from BRAM
                  cacheline_t request_line = (y + j - 2) % 4;
                  float cached = IMAGE_CACHE[request_line * pixels_per_row +
                                             layer.channels_in * x + ci];
                  ACTIVE_AREA[current_aa][j * 3 + 2] = cached;
                  DBG("%010.3f ", cached);
                }
              }
            }
          }
          DBG(")\n");

          // Print Contents of current AA SREG
          /*DBG("AA[%d] Contents: \n[", (int)current_aa);
          for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 3; i++) {
              DBG(" %9.3f ", ACTIVE_AREA[current_aa][j * 3 + i]);
            }
            if (j < 2) DBG("]\n[");
          }
          DBG("]\n");*/

          // Skip following Calculations if not necessary (odd row/col, )

          if (is_preload_col) {  // not enough cols buffered; next ci
            DBG("               skip to next channel (preload column)\n");
            continue;
          }

          if (is_odd_input_row &&
              (layer.stride == 2)) {  // odd row left out in stride2
            DBG("               skip to next channel (odd row, S=2)\n");
            continue;
          }

          if (is_odd_input_col &&
              (layer.stride == 2)) {  // odd col left out in stride2
            DBG("               skip to next channel (odd col, S=2)\n");
            continue;
          }

          ////////////////////////
          /// Loop Level 5: Output Channels Co

          // DBG("               start processing Output Channels..\n");

          for (co = 0; co < layer.channels_out;
               co += num_ch_out_per_convolution) {
            // Print Infos about Channels being convolved:
            if (co == 0)
              // General, Type of Operation
              DBG("               %dx MACC (%dx%d), channels: ",
                  (int)layer.channels_out, (int)layer.kernel,
                  (int)layer.kernel);
            for (int l = 0; l < num_ch_out_per_convolution; l++) {
              if ((co + l) < layer.channels_out) {
                // Notification for each valid In->Out Conv Operation
                DBG("(%d>%d) ", (int)ci, (int)co + l);
              }
            }
            if (layer.kernel == 1) DBG("[packed] ");

            ////////////////////////
            /// 2D CONVOLUTION (for this output channel)
            //
            float conv2D = 0;
            float products[9];
            kernel_t i, j;
            ////////////////////////
            /// Loop Level 6: Kernel H (j)
            /// Loop Level 7: Kernel W (i)

            DBG("\n");
            for (j = 0; j < 3; j++) {
#pragma HLS unroll
              for (i = 0; i < 3; i++) {
#pragma HLS unroll

                // Filter Arrangement in Memory:
                // ch_in -> ch_out -> kernel_h -> kernel_w
                // for 3x3: ch_in -> 1x ch_out -> 3x3
                // for 1x1: ch_in -> 9x ch_out -> 1x1
                weightaddr_t filter_addr =
                    ci * (layer.channels_out * weights_per_filter) +
                    co * weights_per_filter + j * 3 + i;
                float multiply_result = FILTER_CACHE[filter_addr] *
                                        ACTIVE_AREA[current_aa][j * 3 + i];
                conv2D += multiply_result;
                products[j * 3 + i] = multiply_result;

                DBG("filter addr = %4d, ", (int)filter_addr);
                DBG("filter: %9.3f, pixel: %9.3f %s, ",
                    FILTER_CACHE[filter_addr],
                    ACTIVE_AREA[current_aa][j * 3 + i],
                    (co + (j * 3 + i) < layer.channels_out | layer.kernel == 3)
                        ? "(valid)"
                        : "(invalid!)");
                DBG("product[%d]: %9.3f, ", (int)(j * 3 + i),
                    products[j * 3 + i]);
                DBG("conv2D = %9.3f\n", conv2D);
              }
            }
            DBG("\n");

            // Write to Output Cache

            // For 3x3 Kernel: Write MACC Result to Output Cache
            if (layer.kernel == 3) {
              if (ci == 0) {
                // Initialize (Reset) Output Cache for First Input Channel.
                OUTPUT_CACHE[co] = conv2D;
                DBG("i");
              } else {
                // Update (Accumulate) Output Cache for all other Input Channels
                OUTPUT_CACHE[co] += conv2D;
                DBG("u");
              }
              DBG(" -> Cache(%6.2f) ", OUTPUT_CACHE[co]);
            }
            // For 1x1 Kernel: Write 9 Multiply-Results to Cache
            else {
              for (int l = 0; l < 9; l++) {
                if ((co + l) < layer.channels_out) {
                  if (ci == 0) {
                    // Initialize Output Cache for First Input Channel.
                    OUTPUT_CACHE[co + l] = products[l];
                    DBG("i");
                  } else {
                    // Update Output Cache for all other Input Channels
                    OUTPUT_CACHE[co + l] += products[l];
                    DBG("u");
                  }
                }  // end if (co+l)<channels_out
              }    // end for l
            }      // end else if kernel == 1

          }  // end for co

          // one input channel done.
          DBG("\n");

        }  // end for ci

        // one pixel done. (all input channels, all output channels)

        // If pixel didn't produce output, no preprocessing is needed
        if (is_preload_row | is_preload_col) {
          DBG("            no output for pixel -> no postprocessing\n");
          continue;
        }

        // If we're at stride 2 and row or col is odd, skip to next pixel
        // ( -> came here from "continue" in input-channel loop)
        if ((is_odd_input_row | is_odd_input_col) && (layer.stride == 2)) {
          DBG("            skip to next pixel (odd row or col, S=2)\n");
          continue;
        }

        /////////////////
        /// Post-Processing of Pixel

        DBG("            postprocess:\n");

        DBG("L%-2dR%-3dC%-3d ", (int)current_layer, (int)y, (int)x);
        DBG("saving as output pixel(%d,%d)\n", (int)y_out, (int)x_out);

        assert(x_out < width_out && 0 <= x_out);
        assert(y_out < height_out && 0 <= y_out);

        int ch_out = layer.channels_out;
        if (layer.is_expand_layer) {
          // Expand Layers are interleaved -> leave space for 2x channels_out
          DBG("            (expand layer, doubling virtual output channels)\n");
          ch_out = 2 * layer.channels_out;
        }
        DRAM_OUTPUT_PIXEL_PTR =
            (DRAM_OUTPUT_PTR + y_out * width_out * ch_out + x_out * ch_out);

        for (co = 0; co < layer.channels_out; co++) {
          float value = OUTPUT_CACHE[co];

          //////////
          /// BIAS
          DBG("            ch%2d Bias: %7.3f->", (int)co, value);
          weightaddr_t bias_addr =
              num_weights_in_layer - layer.channels_out + co;
          // DBG("bias addr: %d", (int)bias_addr);
          float bias = FILTER_CACHE[bias_addr];
          value += bias;
          DBG("%7.3f, ", value);

          //////////
          /// ReLU
          DBG("ReLU: %7.3f->", value);
          if (value < 0) value = 0;
          DBG("%7.3f, ", value);

          //////////
          /// Pooling
          if (layer.pool != POOL_NONE) {
            if (layer.pool == POOL_GLOBAL) {
              // Global Pooling: Does not really average, but just accumulate.
              // Execute Division by CH_OUT on CPU!
              if (x_out == 0 & y_out == 0) {
                GLOBAL_POOL_CACHE[co] = value;
                DBG("(GPOOL i): %9.3f, ", GLOBAL_POOL_CACHE[co]);
              } else {
                GLOBAL_POOL_CACHE[co] += value;
                DBG("(GPOOL u): %9.3f, ", GLOBAL_POOL_CACHE[co]);
              }
            } else {
              printf("(pool%d), ", (int)layer.kernel);
              assert(false && "Pooling Not Implemented!");
            }
          }

          //////////
          /// Writeback
          DBG("WB(@%lu): %6.2f\n",
              ((long)(DRAM_OUTPUT_PIXEL_PTR + co) - (long)DRAM_DATA),
              *(&value));
          memcpy((void *)(DRAM_OUTPUT_PIXEL_PTR + co), &value, sizeof(value));
        }  // end for co

        DBG("\n");

      }  // end for x
    }    // end for y
  }      // end for layer

  printf("\n\nFPGA: ALL LAYERS DONE!\n");
  printf("Results: [");
  for (co = 0; co < layer.channels_out;
       co++) {  // last layer still in registers
    printf("%9.3f ", GLOBAL_POOL_CACHE[co]);
  }
  printf("]\n");

  // Copy Results to beginning of "Data" DRAM
  memcpy(DRAM_DATA, GLOBAL_POOL_CACHE, layer.channels_out * sizeof(float));
  printf("FPGA: Copy results back to DRAM (%d Bytes)\n\n",
         (int)(layer.channels_out * sizeof(float)));

  // For Debugging: Write DATA Region and WEIGHT Region to files...

  int nbytes;
  FILE *filehandle;

  printf("CONV1 Result Memory Offset: %lu\n",
         BRAM_LAYER_CONFIG[0].mem_addr_output * sizeof(float));

  // Write whole DATA DRAM Region to File:
  filehandle = fopen("DRAM_DATA.bin", "wb");
  // DRAM_OUTPUT_PIXEL_PTR still points to last written entry...
  nbytes = (unsigned long)(DRAM_OUTPUT_PIXEL_PTR + layer.channels_out) -
           (unsigned long)DRAM_DATA;
  fwrite(DRAM_DATA, sizeof(char), nbytes, filehandle);
  fclose(filehandle);

  // Write whole WEIGHT DRAM Region to File:
  filehandle = fopen("DRAM_WEIGHTS.bin", "wb");
  // Calculate Start / End Address of Weight space:
  int num_weights_in_layer =
      layer.kernel * layer.kernel * layer.channels_in * layer.channels_out +
      layer.channels_out;
  float *DRAM_WEIGHTS_END_PTR =
      ((float *)DRAM_WEIGHTS + layer.mem_addr_weights + num_weights_in_layer);
  nbytes = (unsigned long)DRAM_WEIGHTS_END_PTR - (unsigned long)DRAM_WEIGHTS;
  fwrite(DRAM_WEIGHTS, sizeof(char), nbytes, filehandle);
  fclose(filehandle);
}