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
weightaddr_t num_weights_in_layer;

// Addressing + Coordinates
channel_t ci, co;           // current input/output channel
coordinate_t x, y;          // coordinates of input pixel
coordinate_t y_out, x_out;  // coordinates of output pixel
imgdramoffset_t img_dram_offset;
cacheline_t curr_img_cache_line;
imgcacheaddr_t img_cache_addr;

// Pixel-Specific Registers / Wires
bool is_preload_row, is_preload_col;
bool do_pad_top_row, do_pad_bot_row, do_pad_curr_col;
bool is_valid_input_coord;
bool is_odd_input_row, is_odd_input_col;

// ======================
// = FETCH_LAYER_CONFIG =
// ======================
void fetch_layer_config(unsigned int num_layers, float *DRAM_LAYERCONFIG) {
  // Fetch Layer Configuration
  for (int l = 0; l < num_layers; l++) {
    // BRAM_LAYER_CONFIG[l] = ((layer_t *)DRAM_LAYERCONFIG)[l];
    layer_t fetched;
    floats_to_layer(&DRAM_LAYERCONFIG[l * NUM_FLOATS_PER_LAYER], fetched);
    BRAM_LAYER_CONFIG[l] = fetched;
  }
  int bytes_transferred = sizeof(layer_t) * num_layers;
  printf("FPGA: Fetch Layer Config from DRAM to BRAM: (%d Bytes)\n",
         bytes_transferred);
}

// ============================
// = EXTRACT_LAYER_PROPERTIES =
// ============================
void extract_layer_properties() {
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
  num_weights_in_layer = ch_in * ch_out * kernel * kernel + ch_out;

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
  // Make sure we have either 9x 1 weight, or 1x 9 weights per convolution
  assert(num_ch_out_per_convolution == 1 | num_ch_out_per_convolution == 9);
  assert(weights_per_filter == 1 | weights_per_filter == 9);

  // Print Layer Infos
  printf("L%-2d", (int)current_layer);
  printf("%-6s ", layer.name);
  printf("convolution layer, K=%d, S=%d, P=%d\n", (int)layer.kernel,
         (int)layer.stride, (int)layer.pad);

  // Debug Output: Memory Addresses
  DBG("    mem_addr_input: %lu, mem_addr_output: %lu, mem_addr_weights: %lu\n",
      (long)layer.mem_addr_input, (long)layer.mem_addr_output,
      (long)layer.mem_addr_weights);
}

void fetch_weights(data_t *DRAM_WEIGHTS_PTR) {
  // Copy Layer Weights from DRAM -> BRAM
  DBG("    weights cache: fetch (%dB) from @%lu \n",
      (int)(num_weights_in_layer * sizeof(data_t)), (long)DRAM_WEIGHTS_PTR);
  memcpy(WEIGHTS_CACHE, DRAM_WEIGHTS_PTR,
         num_weights_in_layer * sizeof(data_t));
}

void calc_output_coordinates() {
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
}

void calculate_pixel_flags() {
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
  is_preload_row = (pad & (y == 0));
  // Preload Col: Load to Image Cache, push to AA SREG, but no convolution
  is_preload_col = (pad & ((x == -1) | (x == 0)));
  // Padding: Load zero into given row/col of AA SREG
  do_pad_top_row = (pad & (y == 1));          // pad top row (row0)
  do_pad_bot_row = (pad & (y == height_in));  // pad row2
  do_pad_curr_col = (pad & ((x == -1) | (x == width_in)));
  // Valid Input Coordinate: Exists in Input Memory, can load
  is_valid_input_coord =
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
  is_odd_input_row = ((pad == 1) & (y % 2 == 0)) | ((pad == 0) & (y % 2 == 1));
  is_odd_input_col = ((pad == 1) & (x % 2 == 0)) | ((pad == 0) & (x % 2 == 1));
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
}

data_t fetch_next_channel_DRAM_to_CACHE(data_t *DRAM_INPUT_PTR,
                                        data_t *DRAM_DATA) {
  // copy from DRAM into IMAGE Cache
  // img_dram_offset set to beginning of row (in Y-Loop body)
  // img_dram_offset incremented by 1 here for every pixel/channel
  if (!is_valid_input_coord) {
    DBG("skip fetch-from-RAM (not a valid pixel (%d,%d))\n", (int)y, (int)x);
    return NAN;
  }
  data_t pixel_from_ram;
  pixel_from_ram = DRAM_INPUT_PTR[img_dram_offset];
  IMAGE_CACHE[img_cache_addr] = pixel_from_ram;
  // debug info
  DBG("load next input channel from RAM: @%lu->@%lu (%010.3f)\n",
      ((long)(DRAM_INPUT_PTR + img_dram_offset) - (long)DRAM_DATA),
      (long)&IMAGE_CACHE[img_cache_addr], pixel_from_ram);
  // advance image cache and dram access addresses
  img_cache_addr++;
  img_dram_offset++;
  return pixel_from_ram;
}

void update_AA_SREG_with_pixel(const channel_t curr_aa, data_t pixel_from_ram) {
  if (kernel == 1) {
    // For 1x1 Convolution: Load current Pixel into all AA SREG Slots
    // debug prints:  << load single pixel into AA sreg ## (a.aa) >>

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
    // For 3x3 Convolution: Shift in column of 3 pixels from Image Cache
    // debug prints:  << shift column into AA sreg ## (a.aa b.bb c.cc)^T >>
    DBG("               shift column into AA SREG #%d ( ", (int)curr_aa);
  L_AA_SREG_shift_H:
    for (int j = 0; j < 3; j++) {  // j: rows
#pragma HLS unroll
    L_AA_SREG_shift_W:
      for (int i = 0; i < 2; i++) {  // i: columns
#pragma HLS unroll
        // Shift SREG contents left (col 0<-1 and 1<-2)
        ACTIVE_AREA[curr_aa][j * 3 + i] = ACTIVE_AREA[curr_aa][j * 3 + i + 1];
      }
      {  // Shift in new pixels from right (col 2<-new_col)
        bool do_pad = do_pad_curr_col | ((j == 0) & do_pad_top_row) |
                      ((j == 2) & do_pad_bot_row);
        // fetch corresponding pixel from image cache
        cacheline_t req_line = (y - 2 + j) % NUM_IMG_CACHE_LINES;
        imgcacheaddr_t pixel_addr = req_line * pixels_per_row + x * ch_in + ci;
        data_t pixel_from_cache = IMAGE_CACHE[pixel_addr];
        // load into AA SREG (padding pixel -> 0 / normal pixel: from cache)
        data_t pixel_to_load = do_pad ? 0 : pixel_from_cache;
        ACTIVE_AREA[curr_aa][j * 3 + 2] = pixel_to_load;
        // debug info
        DBG("%09.3f ", pixel_to_load);
      }
    }
    DBG(")^T");
  }
  DBG("\n");
}

void print_AA_SREG_contents(const channel_t curr_aa) {
  // ========================
  // = Debug: Print AA SREG =
  // ========================
  DBG("AA[%d] Contents: \n[", (int)curr_aa);
  for (int j = 0; j < 3; j++) {    // j: rows
    for (int i = 0; i < 3; i++) {  // i: columns
      DBG(" %9.3f ", ACTIVE_AREA[curr_aa][j * 3 + i]);
    }
    if (j < 2) DBG("]\n[");  // after each line, except last
  }
  DBG("]\n");
}

void do_2D_convolution(const channel_t curr_aa, const channel_t this_ci,
                       const channel_t this_co, data_t &accumulator,
                       data_t products[9]) {
  // DBG("\n");
  kernel_t i, j;
L_CONV_H:
  for (j = 0; j < 3; j++) {
  L_CONV_W:
    for (i = 0; i < 3; i++) {
      // Filter Arrangement in Memory:
      // ch_in -> ch_out -> kernel_h -> kernel_w
      // for 3x3: ch_in -> 1x ch_out -> 3x3
      // for 1x1: ch_in -> 9x ch_out -> 1x1
      weightaddr_t filter_addr =
          this_ci * ch_out * weights_per_filter +  // start of filters (ci>...)
          this_co * weights_per_filter +           // start of filter (ci>co)
          j * 3 + i;                               // correct filter coefficient
      // multiply:
      data_t multiplication_result =
          WEIGHTS_CACHE[filter_addr] * ACTIVE_AREA[curr_aa][j * 3 + i];
      products[j * 3 + i] = multiplication_result;
      // accumulate:
      accumulator += multiplication_result;

      DBG("filter addr = %4d, ", (int)filter_addr);
      DBG("filter: %9.3f, pixel: %9.3f %s, ", WEIGHTS_CACHE[filter_addr],
          ACTIVE_AREA[curr_aa][j * 3 + i],
          ((co + (j * 3 + i) < ch_out) | (kernel == 3)) ? "(valid)"
                                                        : "(invalid!)");
      DBG("products[%d]: %9.3f, ", (int)(j * 3 + i), products[j * 3 + i]);
      DBG("accumulator = %9.3f\n", accumulator);
    }
  }
}

void update_output_cache(const channel_t this_ci, const channel_t this_co,
                         const data_t accumulator, data_t products[9]) {
  // =========================
  // = Write to Output Cache =
  // =========================
  // Conv3x3: Write MACC Result to Output Cache
  if (kernel == 3) {
    DBG("OutputCache %d, pre-update(%6.2f) ", (int)this_co,
        OUTPUT_CACHE[this_co]);
    if (this_ci == 0) {
      // First Input Channel: Initialize (Reset) Output Cache
      OUTPUT_CACHE[this_co] = accumulator;
      DBG("i");
    } else {
      // All other Input Channels: Update (Accumulate) Output Cache
      OUTPUT_CACHE[this_co] = OUTPUT_CACHE[this_co] + accumulator;
      DBG("u");
    }
    DBG("post-update(%6.2f) ", OUTPUT_CACHE[this_co]);
    DBG(" -> Cache[%d]=(%6.2f) ", (int)this_co, OUTPUT_CACHE[this_co]);
  } else {
  // Conv1x1: Write 9 Multiply-Results to Cache
  L_WRITE_OUTCACHE_1x1:
    for (int l = 0; l < 9; l++) {
      if ((this_co + l) < ch_out) {
        // valid output channel
        if (this_ci == 0) {
          // First Input Channel: Initialize Output Cache
          OUTPUT_CACHE[this_co + l] = products[l];
          DBG("i");
        } else {
          // All other Input Channels: Update (Accumulate) Cache
          OUTPUT_CACHE[this_co + l] = OUTPUT_CACHE[this_co + l] + products[l];
          DBG("u");
        }
      }  // end if (valid output channel)
    }    // end for l
  }
}

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
void fpga_top(data_t *SHARED_DRAM, unsigned int num_layers,
              unsigned int weights_offset, unsigned int input_offset) {
//#pragma HLS DATA_PACK variable=DRAM_LAYERCONFIG struct_level
#pragma HLS INTERFACE m_axi depth = 1000 port = SHARED_DRAM offset = direct
#pragma HLS INTERFACE m_axi depth = 1000 port = DRAM_LAYERCONFIG offset = direct

  //
  // ===========================
  // = Interface Specification =
  // ===========================

  // =============================
  // = Shared DRAM Memory Config =
  // =============================

  float *DRAM_LAYERCONFIG;
  data_t *DRAM_WEIGHTS;
  data_t *DRAM_DATA;
  data_t *DRAM_WEIGHTS_PTR;       // curr layer's weights (mem region start)
  data_t *DRAM_INPUT_PTR;         // curr layer's input (mem region start)
  data_t *DRAM_OUTPUT_PTR;        // curr layer's output (mem region start)
  data_t *DRAM_INPUT_PIXEL_PTR;   // current input pixel
  data_t *DRAM_OUTPUT_PIXEL_PTR;  // current output pixel

  printf("\nStart FPGA Module:\n====================\n\n");

  // ======================================
  // = Initial Setup (DRAM, Layer Config) =
  // ======================================

  // Setup Pointers into shared DRAM
  DRAM_LAYERCONFIG = SHARED_DRAM + 0;
  DRAM_WEIGHTS = SHARED_DRAM + weights_offset;
  DRAM_DATA = SHARED_DRAM + input_offset;

  // Fetch Layer Configuration
  fetch_layer_config(num_layers, DRAM_LAYERCONFIG);

// ========================
// = Loop Level 1: Layers =
// ========================
L_LAYERS:
  for (current_layer = 0; current_layer < num_layers; current_layer++) {
    // ============================
    // = Load Layer Configuration =
    // ============================
    // Get Layer Config (from BRAM)
    layer = BRAM_LAYER_CONFIG[current_layer];
    extract_layer_properties();
    // Set DRAM Memory Pointer
    DRAM_INPUT_PTR = (DRAM_DATA + layer.mem_addr_input);
    // Set DRAM Weights Pointer
    DRAM_WEIGHTS_PTR = (DRAM_WEIGHTS + layer.mem_addr_weights);
    // Copy Layer Weights from DRAM -> BRAM
    fetch_weights(DRAM_WEIGHTS_PTR);

    // =================
    // = Debug Helpers =
    // =================
    // Debug: Print Infos about Image Cache Addresses
    DBG("    setup image cache: fetch from %lu (preload %dkB, total %dkB)\n",
        (long)DRAM_INPUT_PTR, (int)(pixels_per_row * sizeof(data_t) / 1024),
        (int)(pixels_per_row * height_in * sizeof(data_t) / 1024));

    // =======================
    // = Weights Cache Setup =
    // =======================

    // ======================
    // = Output Cache Setup =
    // ======================
    // Set DRAM Memory Pointer (to beginning of output section)
    DRAM_OUTPUT_PTR = (DRAM_DATA + layer.mem_addr_output);

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

        // ==============================
        // = Padding / Preloading Flags =
        // ==============================
        // is_preload_row, is_preload_col?
        // do_pad_top_row, do_pad_bot_row, do_pad_curr_col ?
        // is_valid_input_coord ?
        // =====================
        // = Stride-2 Handling =
        // =====================
        // is_odd_input_row, is_odd_input_col ?
        calculate_pixel_flags();

        // ============================
        // = Output Pixel Coordinates =
        // ============================
        // depending on stride, padding, conv1x1/conv3x3
        calc_output_coordinates();

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
          // fetch next pixel / channel from DRAM
          // returns -9999 for invalid addresses
          data_t pixel_from_ram =
              fetch_next_channel_DRAM_to_CACHE(DRAM_INPUT_PTR, DRAM_DATA);

          // For preload rows, skip to next channel (only store to Image Cache)
          if (is_preload_row) {
            DBG("               skip to next channel (preload row)\n");
            continue;
          }

          // ===============================
          // = Shift Into Active Area SREG =
          // ===============================
          channel_t curr_aa = ci % TILING_CI;
          update_AA_SREG_with_pixel(curr_aa, pixel_from_ram);

          // ========================
          // = Debug: Print AA SREG =
          // ========================
          // print_AA_SREG_contents(curr_aa);

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
          // debug prints: (top: for 3x3, bottom: for 1x1)
          // <<< ## MACC (3x3), channels: (A>0)i\n (A>1)i \n (A>2)i >>>
          // <<< ## MACC (1x1), channels: (A>0) (A>1) (A>2) [packed] iii \n >>>
          DBG("               %dx MACC (%dx%d), channels:\n", (int)ch_out,
              (int)kernel, (int)kernel);

        // For Conv1x1: Each "Convolution Kernel" calculates 9 output channels
        // For Conv3x3: Each "Convolution Kernel" calculates 1 output channel
        // -> "co" points to start of current such "block of output channels"
        L_CH_OUT:
          for (co = 0; co < ch_out; co += num_ch_out_per_convolution) {
            //
            // Debug: What Channel(s) (in>out) MACC'ed in this "co" iteration
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

            // ==============================
            // = Loop Level 6: Kernel H (j) =
            // = Loop Level 7: Kernel W (i) =
            // ==============================
            do_2D_convolution(curr_aa, ci, co, accumulator, products);
            printf("accumulator = %9.3f\n", accumulator);

            // =========================
            // = Write to Output Cache =
            // =========================
            // Conv3x3: Write MACC Result to Output Cache
            update_output_cache(ci, co, accumulator, products);

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
        // Expand Layers are interleaved -> leave space for 2x channels_out
        int offset = ch_out * (width_out * y_out + x_out);
        DRAM_OUTPUT_PIXEL_PTR = DRAM_OUTPUT_PTR;
        DRAM_OUTPUT_PIXEL_PTR += layer.is_expand_layer ? 2 * offset : offset;
        if (layer.is_expand_layer)
          DBG("            (expand layer, doubling virtual output channels)\n");

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
              GLOBAL_POOL_CACHE[co] = GLOBAL_POOL_CACHE[co] + value;
              DBG("(GPOOL u): %9.3f, ", GLOBAL_POOL_CACHE[co]);
            }
          }  // other types of pooling: not supported. use all-conv networks!

          // =============================
          // = Write-Back Output Channel =
          // =============================
          DBG("WB(@%lu): %6.2f\n",
              ((long)(DRAM_OUTPUT_PIXEL_PTR + co) - (long)DRAM_DATA), value);
          DRAM_OUTPUT_PIXEL_PTR[co] = value;

        }  // end for co

        DBG("\n");

      }  // end for x
    }    // end for y
  }      // end for layer

// =============================
// = Write-Back GLOBAL POOLING =
// =============================
L_copy_globalpool_results:
  // ch_out still holds value from last layer
  for (int co = 0; co < ch_out; co++) {
    DRAM_DATA[co] = GLOBAL_POOL_CACHE[co];
  }
  // memcpy(DRAM_DATA, GLOBAL_POOL_CACHE, ch_out * sizeof(data_t));
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
#ifndef __SYNTHESIS__
  int nbytes;
  FILE *outfile;

  // Write whole DATA DRAM Region to File:
  outfile = fopen("DRAM_DATA.bin", "wb");
  // DRAM_OUTPUT_PIXEL_PTR still points to last written entry...
  nbytes = (long)(DRAM_OUTPUT_PIXEL_PTR + layer.channels_out) - (long)DRAM_DATA;
  fwrite((data_t *)DRAM_DATA, sizeof(char), nbytes, outfile);
  fclose(outfile);

  // Write whole WEIGHT DRAM Region to File:
  outfile = fopen("DRAM_WEIGHTS.bin", "wb");
  // Calculate End Address of Weight memory:
  int n_weights_last_layer = kernel * kernel * ch_in * ch_out + ch_out;
  data_t *DRAM_WEIGHTS_END_PTR =
      ((data_t *)DRAM_WEIGHTS + layer.mem_addr_weights + n_weights_last_layer);
  nbytes = (long)DRAM_WEIGHTS_END_PTR - (long)DRAM_WEIGHTS;
  fwrite((data_t *)DRAM_WEIGHTS, sizeof(char), nbytes, outfile);
  fclose(outfile);
#endif

  // ===============================
  // = Debug: Memory Addr. Offsets =
  // ===============================
  for (int layer_id = 0; layer_id < num_layers; layer_id++) {
    DBG("%s Result Memory Offset: %lu\n", BRAM_LAYER_CONFIG[layer_id].name,
        (long)BRAM_LAYER_CONFIG[layer_id].mem_addr_output * sizeof(data_t));
  }
}
