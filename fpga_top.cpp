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

// FPGA BRAM Memory
layer_t BRAM_LAYER_CONFIG[MAX_NUM_LAYERS];
data_t WEIGHTS_CACHE[MAX_WEIGHTS_PER_LAYER];
data_t IMAGE_CACHE[MAX_IMAGE_CACHE_SIZE];
data_t ACTIVE_AREA[TILING_CI][9];
data_t OUTPUT_CACHE[MAX_NUM_CHOUT];
data_t GLOBAL_POOL_CACHE[MAX_NUM_CHOUT];

// Shared DRAM Memory Pointers
bus_t *DRAM_LAYERCONFIG;
data_t *DRAM_WEIGHTS;
data_t *DRAM_DATA;
data_t *DRAM_WEIGHTS_PTR;       // current layer weights (mem region start)
data_t *DRAM_INPUT_PTR;         // current layer's input (mem region start)
data_t *DRAM_OUTPUT_PTR;        // current layer's output (mem region start)
data_t *DRAM_INPUT_PIXEL_PTR;   // current input pixel
data_t *DRAM_OUTPUT_PIXEL_PTR;  // current output pixel

// Layer-Specific Registers / Wires
layer_t layer;
layerid_t current_layer;
dimension_t width_in, height_in;
dimension_t width_out, height_out;
channel_t ch_in, ch_out;
kernel_t kernel;
stride_t stride;
bool pad;
pixelperrow_t pixels_per_row;                 // ch_in * width_in
numfilterelems_t num_ch_out_per_convolution;  // 9 for 1x1 conv, 1 for 3x3 conv
numfilterelems_t weights_per_filter;          // 1 for 1x1 conv, 9 for 3x3 conv

channel_t ci, co;           // current input/output channel
coordinate_t x, y;          // coordinates of input pixel
coordinate_t y_out, x_out;  // coordinates of output pixel
imgdramoffset_t img_dram_offset;
cacheline_t curr_img_cache_line;
imgcacheaddr_t img_cache_addr;

// ====================
// = Main FPGA Module =
// ====================
//
// DRAM:
//    AXI4 Master Bus Interface to Memory
//
// num_layers, byte_..._offset:
//    AXI4 Slave Interface. Registers that hold information on where to
//    fetch actual configuration from in DRAM.
//
void fpga_top(volatile bus_t *DRAM, unsigned int num_layers,
              unsigned int byte_layerconfig_offset,
              unsigned int byte_weights_offset,
              unsigned int byte_input_offset) {
// ===========================
// = Interface Specification =
// ===========================

// AXI4 Master
// ap_bus + AXI4 Master for connection to DRAM (via DMA)
#pragma HLS INTERFACE ap_bus port = DRAM
#pragma HLS RESOURCE core = AXI4M variable = DRAM

// AXI4 Lite + Registers for Configuration Data
#pragma HLS RESOURCE core = AXI4LiteS variable = return metadata = \
    "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = num_layers
#pragma HLS RESOURCE core = AXI4LiteS variable = \
    byte_weights_offset metadata = "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = byte_weights_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = \
    byte_weights_offset metadata = "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = byte_layerconfig_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = \
    byte_layerconfig_offset metadata = "-bus_bundle LITE"
#pragma HLS INTERFACE ap_none register port = byte_input_offset
#pragma HLS RESOURCE core = AXI4LiteS variable = byte_input_offset metadata = \
    "-bus_bundle LITE"

#ifndef __SYNTHESIS__
  printf("\nStart FPGA Module:\n====================\n\n");
#endif

  // ======================================
  // = Initial Setup (DRAM, Layer Config) =
  // ======================================

  // Setup Pointers into shared DRAM
  DRAM_LAYERCONFIG = (bus_t *)(DRAM + byte_layerconfig_offset / sizeof(bus_t));
  DRAM_WEIGHTS = (data_t *)(DRAM + byte_weights_offset / sizeof(bus_t));
  DRAM_DATA = (data_t *)(DRAM + byte_input_offset / sizeof(bus_t));

  // Fetch Layer Configuration
  bus_t rx_transaction[transactions_per_layer];
  int bytes_transferred = 0;
  for (int l = 0; l < num_layers; l++) {
    for (int t = 0; t < transactions_per_layer; t++) {
      memcpy(&rx_transaction[t],
             (DRAM_LAYERCONFIG + l * transactions_per_layer + t),
             sizeof(bus_t));
      bytes_transferred += sizeof(bus_t);
    }
    BRAM_LAYER_CONFIG[l] = *((layer_t*)rx_transaction);
  }
// memcpy(BRAM_LAYER_CONFIG, DRAM_LAYERCONFIG, cfg_bytes);
#ifndef __SYNTHESIS__
  printf("FPGA: Fetch Layer Config from DRAM to BRAM: (%d Bytes)\n",
         bytes_transferred);
#endif

// ========================
// = Loop Level 1: Layers =
// ========================
L_LAYERS:
  for (current_layer = 0; current_layer < num_layers; current_layer++) {
    // ============================
    // = Load Layer Configuration =
    // ============================
    layer = BRAM_LAYER_CONFIG[current_layer];

#ifndef __SYNTHESIS__
    // Print Layer Infos
    printf("L%-2d", (int)current_layer);
    printf("%-6s ", layer.name);
    printf("convolution layer, K=%d, S=%d, P=%d\n", (int)layer.kernel,
           (int)layer.stride, (int)layer.pad);
#endif

    // Debug Output: Memory Addresses
    DBG("    mem_addr_input: %d, mem_addr_output: %d, mem_addr_weights: %d\n",
        layer.mem_addr_input, layer.mem_addr_output, layer.mem_addr_weights);

    // ============================
    // = Constraints / Assertions =
    // ============================
    // Assertions / Constraints to enable Optimizations:
    assert(layer.width == 0 | layer.width == 1 | layer.width == 2 |
           layer.width == 4 | layer.width == 8 | layer.width == 16 |
           layer.width == 32 | layer.width == 64 | layer.width == 128 |
           layer.width == 256);
    assert(layer.width == 0 | layer.height == 1 | layer.height == 2 |
           layer.height == 4 | layer.height == 8 | layer.height == 16 |
           layer.height == 32 | layer.height == 64 | layer.height == 128 |
           layer.height == 256);
    assert(layer.channels_in == 1 | layer.channels_in == 2 |
           layer.channels_in == 3 | layer.channels_in == 4 |
           layer.channels_in == 8 | layer.channels_in == 16 |
           layer.channels_in == 32 | layer.channels_in == 64 |
           layer.channels_in == 128 | layer.channels_in == 256 |
           layer.channels_in == 512);
    assert(layer.channels_out == 1 | layer.channels_out == 2 |
           layer.channels_out == 4 | layer.channels_out == 8 |
           layer.channels_out == 10 | layer.channels_out == 16 |
           layer.channels_out == 32 | layer.channels_out == 64 |
           layer.channels_out == 128 | layer.channels_out == 256 |
           layer.channels_out == 512 | layer.channels_out == 1000);
    assert(layer.pad == 0 | layer.pad == 1);
    assert(layer.stride == 1 | layer.stride == 2);
    assert(layer.kernel == 1 | layer.kernel == 3);
    // Make sure that all 3x3 kernels use padding, all 1x1 kernels don't.
    assert((layer.kernel == 3 & layer.pad) | (layer.kernel == 1 & !layer.pad));

    // ===================================
    // = Layer-Specific Global Registers =
    // ===================================
    width_in = layer.width;
    height_in = layer.height;
    ch_in = layer.channels_in;
    ch_out = layer.channels_out;
    kernel = layer.kernel;
    stride = layer.stride;
    pad = layer.pad;
    width_out = (stride == 2) ? (width_in / 2) : (width_in / 1);
    height_out = (stride == 2) ? (height_in / 2) : (height_in / 1);
    pixels_per_row = width_in * ch_in;

    // ==================================
    // = Image Cache Setup (DRAM Input) =
    // ==================================
    // Set DRAM Memory Pointer
    DRAM_INPUT_PTR = ((data_t *)DRAM_DATA + layer.mem_addr_input);

    // Debug: Print Infos about Image Cache Addresses
    DBG("    setup image cache: fetch from %lu (preload %dkB, total %dkB)\n",
        (long)DRAM_INPUT_PTR, (int)(pixels_per_row * sizeof(data_t) / 1024),
        (int)(pixels_per_row * height_in * sizeof(data_t) / 1024));

    // =======================
    // = Weights Cache Setup =
    // =======================
    // Copy Layer Weights from DRAM -> BRAM
    DRAM_WEIGHTS_PTR = ((data_t *)DRAM_WEIGHTS + layer.mem_addr_weights);
    int num_weights_in_layer = ch_in * ch_out * kernel * kernel + ch_out;
    int weights_size_bytes = num_weights_in_layer * sizeof(data_t);
    DBG("    setup weights cache: will fetch from %lu (%dkB)\n",
        (long)DRAM_WEIGHTS_PTR, weights_size_bytes / 1024);
    memcpy(WEIGHTS_CACHE, DRAM_WEIGHTS_PTR, weights_size_bytes);

    // ======================
    // = Output Cache Setup =
    // ======================
    // Set DRAM Memory Pointer (to beginning of output section)
    data_t *DRAM_OUTPUT_PTR = ((data_t *)DRAM_DATA + layer.mem_addr_output);

    // ==========================================
    // = Convolution Kernel Setup (1x1 vs. 3x3) =
    // ==========================================
    if (kernel == 3) {
      num_ch_out_per_convolution = 1;
      weights_per_filter = 9;
    } else {
      num_ch_out_per_convolution = 9;
      weights_per_filter = 1;
    }
    assert(num_ch_out_per_convolution == 1 | num_ch_out_per_convolution == 9);
    assert(weights_per_filter == 1 | weights_per_filter == 9);

    // ========================
    // = Loop Level 2: Rows Y =
    // ========================

    // Convolution Scheduling:
    // =======================
    //
    // Case 1: 3x3, 1px Padding
    // ````````````````````````````
    // need 1 row padding (not necessarily executed)
    // need 1 row preload
    // then (1px padding, 1px preload, width pixels convolutions) x (height)
    // Y: (-1), 0, 1, 2, ... , H-1, H | (-1),H = pad  0 = preload  1..H = output
    // X: (-1), 0, 1, 2, ... , W-1, W | (-1),W = pad  0 = preload  1..W = output
    // Output Coord: Yout = Y-1 (0..H-1), Xout = X-1 (0..W-1)
    // Total: (WIDTH+2) x (HEIGHT+2) Operations, (WIDTH) x (HEIGHT) Outputs
    // -> Y=(-1) is unnecessary (padding handled in AA SREG) -> left away.
    //
    // Case 2: 1x1, no padding, Packed into 3x3 "kernel"
    // ````````````````````````````````````````````````````
    // -> ActiveArea contains 9x same pixel, same input channel.
    //    Filter contains 9 different output channels (for given input channel).
    // no padding, no preload
    // then (width pixels convolutions) x (height)
    // Y: 0, 1, 2, ... , H-1     |    0..H-1 = output
    // X: 0, 1, 2, ... , W-1     |    0..W-1 = output
    // Output Coord: Yout = Y, Xout = X
    // Total: WIDTH x HEIGHT Operations
    // -> filter loading in batches of 9 (output channel loop)
    //

    // ========================
    // = Loop Level 2: Rows Y =
    // ========================
    DBG("    start processing rows...\n");

  L_Y:
    for (y = 0; y < height_in + pad; y++) {
      // Conv3x3: pad=1 and Y = 0 ... H
      // Conv1x1: pad=0 and Y = 0 ... H-1
      // y=(-1) is unnecessary, padding handled in AA SREG -> left away

      DBG("L%-2dR%-3d ", (int)current_layer, (int)y);

      // ==================================
      // = Image Cache Setup (Line / Row) =
      // ==================================

      // Set Image Cache Row (defines where to fetch to, and permutation)
      curr_img_cache_line = y % NUM_IMG_CACHE_LINES;

      // Set Cache + DRAM Addresses to pixel 0, channel 0 of current row.
      // Image Cache + DRAM are linearly accessed.
      img_cache_addr = pixels_per_row * curr_img_cache_line;
      img_dram_offset = pixels_per_row * y;

      DBG("setup image cache: from DRAM offset %d to cache addr %d (line%d)\n",
          (int)img_dram_offset, (int)img_cache_addr, (int)curr_img_cache_line);

      // ===========================
      // = Loop Level 3: Columns X =
      // ===========================
      DBG("        start processing columns...\n");

    L_X:
      for (x = -pad; x < width_in + pad; x++) {
        // Conv3x3: pad=1 and X = -1 ... H
        // Conv1x1: pad=0 and X =  0 ... H-1

        DBG("L%-2dR%-3dC%-3d ", (int)current_layer, (int)y, (int)x);

        // ============================
        // = Output Pixel Coordinates =
        // ============================
        // Conv3x3: pad=1 and Xout = X-1
        // Conv1x1: pad=0 and Xout = X
        // Stride 2: Xout = X/2
        x_out = (kernel == 3) ? (x - 1) : (x - 0);
        y_out = (kernel == 3) ? (y - 1) : (y - 0);
        x_out = (stride == 2) ? x_out / 2 : x_out / 1;
        y_out = (stride == 2) ? y_out / 2 : y_out / 1;

        // ==============================
        // = Padding / Preloading Flags =
        // ==============================
        // Type of current Pixel:
        // * Kernel 3x3 (with padding):
        //   Y: 0, 1,..., H-1, H             H = pad  0 = preload  1..H = output
        //   X: (-1), 0, 1,..., W-1, W    -1,W = pad  0 = preload  1..W = output
        // * Kernel 1x1, packed3x3 (no padding):
        //   (no preload, no padding, width pixels convolutions) x (height rows)

        // Preload Row: Load to Image Cache, but don't push to ActiveArea SREG
        bool is_preload_row = (pad & (y == 0));
        // Preload Col: Load to Image Cache, push to AA SREG, but no convolution
        bool is_preload_col = (pad & ((x == -1) | (x == 0)));

        // Padding: Load zero into given row/col of AA SREG
        bool do_pad_top_row = (pad & (y == 1));          // pad top row (row0)
        bool do_pad_bot_row = (pad & (y == height_in));  // pad row2
        bool do_pad_curr_col = (pad & ((x == -1) | (x == width_in)));

        // Valid Input Coordinate: Exists in Input Memory, can load
        bool is_valid_input_coord =
            ((x >= 0) & (x < width_in)) & ((y >= 0) & (y < height_in));

        // =====================
        // = Stride-2 Handling =
        // =====================
        // Need to keep output rows/cols [0, 2, 4, 6, ...]
        // Conv3x3 and pad=1: corresponds to x and y in [1, 3, 5, ...]
        // Conv1x1 and pad=0: corresponds to x and y in [0, 2, 4, ...]
        // -> other x/y are considered 'odd' and skipped:
        // for pad=1, skip input rows/cols 2,4,6,... (x%2==0, y%2==0)
        // for pad=0, skip input rows/cols 1,3,5,... (x%2==1, y%2==1)
        bool is_odd_input_row =
            ((pad == 1) & (y % 2 == 0)) | ((pad == 0) & (y % 2 == 1));
        bool is_odd_input_col =
            ((pad == 1) & (x % 2 == 0)) | ((pad == 0) & (x % 2 == 1));

        // Debug: Print Pixel Characterstics
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

        // ===================================
        // = Loop Level 4: Input Channels Ci =
        // ===================================
        DBG("            start processing input channels...\n");

      L_CH_IN:
        for (ci = 0; ci < ch_in; ci++) {
          // Input Channels: Not ready for tiling!
          // Would need different schedule and summation.

          DBG("L%-2dR%-3dC%-3dI%-2d ", (int)current_layer, (int)y, (int)x,
              (int)ci);

          // ====================================
          // = Load Next Pixel/Channel from RAM =
          // ====================================
          data_t pixel_from_ram = -9999;

          if (is_valid_input_coord) {
            // copy from DRAM into IMAGE Cache
            // img_dram_offset is set to beginning of row in Y-Loop Body
            // img_dram_offset is incremented by 1 here for every pixel/channel
            memcpy(&pixel_from_ram, DRAM_INPUT_PTR + img_dram_offset,
                   sizeof(data_t));
            IMAGE_CACHE[img_cache_addr] = pixel_from_ram;
            DBG("load next input channel from RAM: @%lu->@%lu (%010.3f)\n",
                ((long)(DRAM_INPUT_PTR + img_dram_offset) - (long)DRAM_DATA),
                (long)&IMAGE_CACHE[img_cache_addr], pixel_from_ram);
            // advance image cache and dram access addresses
            img_cache_addr++;
            img_dram_offset++;
          } else {
            // not a valid pixel -> skip
            DBG("skip fetch-from-RAM (not a valid pixel (%d,%d))\n", (int)y,
                (int)x);
          }

          // For preload rows, skip to next channel (only store to Image Cache)
          if (is_preload_row) {
            DBG("               skip to next channel (preload row)\n");
            continue;
          }

          // ===============================
          // = Shift Into Active Area SREG =
          // ===============================
          channel_t curr_aa = ci % TILING_CI;
          if (kernel == 1) {
            // For 1x1 Convolution: Load current Pixel into all AA SREG Slots
            DBG("               load single pixel into AA SREG #%d (%010.4f)",
                (int)curr_aa, pixel_from_ram);
          L_AA_SREG_loadAll_H:
            for (int j = 0; j < 3; j++) {
#pragma HLS unroll
            L_AA_SREG_loadAll_W:
              for (int i = 0; i < 3; i++) {
#pragma HLS unroll
                ACTIVE_AREA[curr_aa][j * 3 + i] = pixel_from_ram;
              }
            }
          } else {
            // For 3x3 Convolution: Shift in column of 3 pixels from image cache
            DBG("               shift column into AA SREG #%d (", (int)curr_aa);
          L_AA_SREG_shift_H:
            for (int j = 0; j < 3; j++) {  // j: rows
#pragma HLS unroll
            L_AA_SREG_shift_W:
              for (int i = 0; i < 2; i++) {  // i: columns
#pragma HLS unroll
                // Shift SREG contents left (col 0<-1 and 1<-2)
                ACTIVE_AREA[curr_aa][j * 3 + i] =
                    ACTIVE_AREA[curr_aa][j * 3 + i + 1];
              }
              {  // Shift in new pixels from right (col 2<-new_col)
                bool do_pad_pixel = do_pad_curr_col |
                                    ((j == 0) & do_pad_top_row) |
                                    ((j == 2) & do_pad_bot_row);
                if (do_pad_pixel) {
                  // PADDING PIXEL
                  ACTIVE_AREA[curr_aa][j * 3 + 2] = 0;
                  DBG("0 ");
                } else {
                  // NORMAL PIXEL - load from Cache
                  cacheline_t request_line = (y - 2 + j) % NUM_IMG_CACHE_LINES;
                  imgcacheaddr_t pixel_addr =
                      request_line * pixels_per_row + x * ch_in + ci;
                  ACTIVE_AREA[curr_aa][j * 3 + 2] = IMAGE_CACHE[pixel_addr];
                  DBG("%010.3f ", IMAGE_CACHE[pixel_addr]);
                }
              }
            }
          }
          DBG(")^T\n");

          // ========================
          // = Debug: Print AA SREG =
          // ========================
          /*DBG("AA[%d] Contents: \n[", (int)curr_aa);
          for (int j = 0; j < 3; j++) {   // j: rows
            for (int i = 0; i < 3; i++) { // i: columns             
              DBG(" %9.3f ", ACTIVE_AREA[curr_aa][j * 3 + i]);
            }
            if (j < 2) DBG("]\n["); // after each line, except last
          }
          DBG("]\n");*/  // after last line

          // =======================================
          // = Conditional Skipping of Convolution =
          // =======================================

          // Skip Convolution for preload cols (not enough cols buffered)
          if (is_preload_col) {
            DBG("               skip to next channel (preload column)\n");
            continue;
          }
          // Skip Convolution for stride-2:
          if ((stride == 2) & (is_odd_input_row | is_odd_input_col)) {
            DBG("               skip to next channel (odd %s, S=2)\n",
                is_odd_input_row ? "row" : "col");
            continue;
          }

          // ====================================
          // = Loop Level 5: Output Channels Co =
          // ====================================

          DBG("               start processing Output Channels.\n");
          DBG("               %dx MACC (%dx%d), channels:\n", (int)ch_out,
              (int)kernel, (int)kernel);

        // For Conv1x1: Each "Convolution Kernel" calculates 9 output channels
        // For Conv3x3: Each "Convolution Kernel" calculates 1 output channel
        // -> co points to start of such a "block of output channels"
        L_CH_OUT:
          for (co = 0; co < ch_out; co += num_ch_out_per_convolution) {
            // Debug: What Channel(s) (in > out) MACC'ed in this co-iteration
            for (int l = 0; l < num_ch_out_per_convolution; l++) {
              if ((co + l) < ch_out) DBG("(%d>%d) ", (int)ci, (int)co + l);
            }
            // Debug: 9 1x1 MACCs can be packed into 1 conv kernel:
            if (kernel == 1) DBG("[packed] ");  // = 9 packed 1x1 convs

            // ==================
            // = 2D CONVOLUTION =
            // ==================
            data_t accumulator = 0;  // accumulator
            data_t products[9];      // products from individual multiplications
            kernel_t i, j;

          // ==============================
          // = Loop Level 6: Kernel H (j) =
          // = Loop Level 7: Kernel W (i) =
          // ==============================
          // DBG("\n");
          L_CONV_H:
            for (j = 0; j < 3; j++) {
#pragma HLS unroll
            L_CONV_W:
              for (i = 0; i < 3; i++) {
#pragma HLS unroll
                // Filter Arrangement in Memory:
                // ch_in -> ch_out -> kernel_h -> kernel_w
                // for 3x3: ch_in -> 1x ch_out -> 3x3
                // for 1x1: ch_in -> 9x ch_out -> 1x1
                weightaddr_t filter_addr = ci * ch_out * weights_per_filter +
                                           co * weights_per_filter + j * 3 + i;
                // multiply:
                data_t multiplication_result = WEIGHTS_CACHE[filter_addr] *
                                               ACTIVE_AREA[curr_aa][j * 3 + i];
                products[j * 3 + i] = multiplication_result;
                // accumulate:
                accumulator += multiplication_result;

                /*DBG("filter addr = %4d, ", (int)filter_addr);
                DBG("filter: %9.3f, pixel: %9.3f %s, ",
                    WEIGHTS_CACHE[filter_addr], ACTIVE_AREA[curr_aa][j * 3 + i],
                    ((co + (j * 3 + i) < ch_out) | (kernel == 3))
                        ? "(valid)"
                        : "(invalid!)");
                DBG("products[%d]: %9.3f, ", (int)(j * 3 + i),
                    products[j * 3 + i]);
                DBG("accumulator = %9.3f\n", accumulator);*/
              }
            }
            // DBG("\n");

            // =========================
            // = Write to Output Cache =
            // =========================
            // Conv3x3: Write MACC Result to Output Cache
            if (kernel == 3) {
              if (ci == 0) {
                // First Input Channel: Initialize (Reset) Output Cache
                OUTPUT_CACHE[co] = accumulator;
                DBG("i");
              } else {
                // All other Input Channels: Update (Accumulate) Output Cache
                OUTPUT_CACHE[co] += accumulator;
                DBG("u");
              }
              DBG(" -> Cache(%6.2f) ", OUTPUT_CACHE[co]);
            } else {
            // Conv1x1: Write 9 Multiply-Results to Cache
            L_WRITE_OUTCACHE_1x1:
              for (int l = 0; l < 9; l++) {
                if ((co + l) < ch_out) {
                  // valid output channel
                  if (ci == 0) {
                    // First Input Channel: Initialize Output Cache
                    OUTPUT_CACHE[co + l] = products[l];
                    DBG("i");
                  } else {
                    // All other Input Channels: Update (Accumulate) Cache
                    OUTPUT_CACHE[co + l] += products[l];
                    DBG("u");
                  }
                }  // end if (valid output channel)
              }    // end for l
            }      // end if (kernel == 3 or 1)

          }  // End of Output Channel Loop (co)

          // One Input Channel finished.
          DBG("\n");

        }  // End of Input Channel Loop (ci)

        // One Pixel (all Input Channels, all Output Channels) finished.

        // ===========================================
        // = Conditional Skipping of Post-Processing =
        // ===========================================

        // If pixel didn't produce output -> no postprocessing necessary
        if (is_preload_row | is_preload_col) {
          DBG("            no output for pixel -> no postprocessing\n");
          continue;
        }

        // For stride 2 and odd row/col, skip to next pixel
        // (-> came here from "continue" in input-channel-loop)
        if ((stride == 2) & (is_odd_input_row | is_odd_input_col)) {
          DBG("            skip to next pixel (odd row or col, S=2)\n");
          continue;
        }

        // ===================
        // = Post-Processing =
        // ===================
        DBG("            postprocess:\n");
        DBG("L%-2dR%-3dC%-3d ", (int)current_layer, (int)y, (int)x);
        DBG("saving as output pixel(%d,%d)\n", (int)y_out, (int)x_out);

        assert((0 <= x_out) & (x_out < width_out));
        assert((0 <= y_out) & (y_out < height_out));

        // ==================================
        // = Write-Back Address Calculation =
        // ==================================
        if (layer.is_expand_layer) {
          // Expand Layers are interleaved -> leave space for 2x channels_out
          DBG("            (expand layer, doubling virtual output channels)\n");
          DRAM_OUTPUT_PIXEL_PTR =
              (DRAM_OUTPUT_PTR + 2 * ch_out * (width_out * y_out + x_out));
        } else {
          DRAM_OUTPUT_PIXEL_PTR =
              (DRAM_OUTPUT_PTR + ch_out * (width_out * y_out + x_out));
        }

      // =====================================
      // = Output Channel Loop (Postprocess) =
      // =====================================
      L_postprocess_ch_out:
        for (co = 0; co < layer.channels_out; co++) {
          data_t value = OUTPUT_CACHE[co];

          // ========
          // = BIAS =
          // ========
          DBG("            ch%2d Bias: %7.3f->", (int)co, value);
          weightaddr_t bias_addr =
              num_weights_in_layer - layer.channels_out + co;
          // DBG("bias addr: %d", (int)bias_addr);
          data_t bias = WEIGHTS_CACHE[bias_addr];
          value += bias;
          DBG("%7.3f, ", value);

          // ========
          // = ReLU =
          // ========
          DBG("ReLU: %7.3f->", value);
          if (value < 0) value = 0;
          DBG("%7.3f, ", value);

          // ==================
          // = GLOBAL POOLING =
          // ==================
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
          }  // other types of pooling: not supported. use all-conv networks!

          // =============================
          // = Write-Back Output Channel =
          // =============================
          DBG("WB(@%lu): %6.2f\n",
              ((long)(DRAM_OUTPUT_PIXEL_PTR + co) - (long)DRAM_DATA), value);
          memcpy(DRAM_OUTPUT_PIXEL_PTR + co, &value, sizeof(value));
        }  // end for co

        DBG("\n");

      }  // end for x
    }    // end for y
  }      // end for layer

  // =============================
  // = Write-Back GLOBAL POOLING =
  // =============================
  // ch_out still holds value from last layer
  memcpy(DRAM_DATA, GLOBAL_POOL_CACHE, ch_out * sizeof(data_t));
#ifndef __SYNTHESIS__
  printf("\nFPGA: Copy results back to DRAM (%d Bytes)\n",
         (int)(ch_out * sizeof(data_t)));

  // ================
  // = Final Report =
  // ================
  printf("SqueezeNet on FPGA: DONE!\n\n");
  DBG("Results: [");
  for (co = 0; co < ch_out; co++) {
    DBG("%7.3f ", GLOBAL_POOL_CACHE[co]);
  }
  DBG("]\n");

  // =============================
  // = Debug: Write DRAM to File =
  // =============================
  // For Debugging: Write DATA Region and WEIGHT Region to files...

  int nbytes;
  FILE *outfile;

  // Write whole DATA DRAM Region to File:
  outfile = fopen("DRAM_DATA.bin", "wb");
  // DRAM_OUTPUT_PIXEL_PTR still points to last written entry...
  nbytes = (long)(DRAM_OUTPUT_PIXEL_PTR + layer.channels_out) - (long)DRAM_DATA;
  fwrite(DRAM_DATA, sizeof(char), nbytes, outfile);
  fclose(outfile);

  // Write whole WEIGHT DRAM Region to File:
  outfile = fopen("DRAM_WEIGHTS.bin", "wb");
  // Calculate End Address of Weight memory:
  int n_weights_last_layer = kernel * kernel * ch_in * ch_out + ch_out;
  data_t *DRAM_WEIGHTS_END_PTR =
      ((data_t *)DRAM_WEIGHTS + layer.mem_addr_weights + n_weights_last_layer);
  nbytes = (long)DRAM_WEIGHTS_END_PTR - (long)DRAM_WEIGHTS;
  fwrite(DRAM_WEIGHTS, sizeof(char), nbytes, outfile);
  fclose(outfile);

  // ===============================
  // = Debug: Memory Addr. Offsets =
  // ===============================
  for (int layer_id = 0; layer_id < num_layers; layer_id++) {
    DBG("%s Result Memory Offset: %lu\n", BRAM_LAYER_CONFIG[layer_id].name,
        BRAM_LAYER_CONFIG[layer_id].mem_addr_output * sizeof(data_t));
  }
#endif
}
