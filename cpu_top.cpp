#include "cpu_top.hpp"

// Pointers to FPGA DRAM Memory
char *DRAM;  // addressed at byte-level
layer_t *DRAM_LAYER_CONFIG;
float *DRAM_WEIGHTS;
float *DRAM_DATA;

void allocate_FPGA_memory(network_t *net_CPU) {
  // For Simulation purposes, allocate space on Heap
  // For actual HW Implementation, set fixed memory addresses in Shared DRAM

  int layerconfig_size = net_CPU->num_layers * sizeof(layer_t);
  int weights_size = net_CPU->num_weights * sizeof(float);

  // FPGA DRAM Memory Allocation: Layer Config
  int configsize = net_CPU->num_layers * sizeof(layer_t);
  int weightsize = net_CPU->num_weights * sizeof(float);
  int datasize = net_CPU->total_pixel_mem * sizeof(float);

  // Reserve Memory and Assign Pointers
  // DRAM:
  //      ______________
  //     | layer config |  0
  //     |              |  ...
  //     |______________|  configsize - 1
  //     |    weights   |  configsize
  //     |              |  ...
  //     |______________|  configsize + weightsize - 1
  //     |     data     |  configsize + weightsize
  //     |  in + output |  ...
  //     |______________|  configsize + weightsize + datasize - 1
  //
  DRAM = (char *)malloc(configsize + weightsize + datasize);
  DRAM_LAYER_CONFIG = (layer_t *)(DRAM + 0);
  DRAM_WEIGHTS = (float *)(DRAM + configsize);
  DRAM_DATA = (float *)(DRAM + configsize + weightsize);

  // FPGA DRAM Memory Allocation: Weights
  printf(
      "CPU: FPGA DRAM Memory Allocation:\nBytes allocated: %d Bytes "
      "(layer config) + %d Bytes (weights)\n",
      layerconfig_size, weights_size);
  printf(
      "CPU: FPGA DRAM Memory Allocation:\nStart Address: %lu, End Address: "
      "%lu\n\n",
      (unsigned long)DRAM,
      (unsigned long)((char *)DRAM + configsize + weightsize + datasize));
}

void copy_config_to_FPGA(network_t *net_CPU) {
  int layerconfig_size = net_CPU->num_layers * sizeof(layer_t);
  int weights_size = net_CPU->num_weights * sizeof(float);

  // Layer Config:
  memcpy(DRAM_LAYER_CONFIG, net_CPU->layers, layerconfig_size);

  // Weights:
  memcpy(DRAM_WEIGHTS, net_CPU->weights, weights_size);

  printf("Bytes copied: %d Bytes (layer config) + %d Bytes (weights)\n\n",
         layerconfig_size, weights_size);
}

void copy_input_image_to_FPGA(network_t *net_CPU, float *image) {
  // Assumption: Input Data goes into Layer 0
  int win = net_CPU->layers[0].width;
  int hin = net_CPU->layers[0].height;
  int chin = net_CPU->layers[0].channels_in;
  int input_image_size = win * hin * chin * sizeof(float);

  // Copy Input Data:
  memcpy(DRAM_DATA, image, input_image_size);
  printf("Input Data copied: %d Bytes (input image, %s)\n\n", input_image_size,
         net_CPU->layers[0].name);
}

// Copy Results back from FPGA memory
// Assumption: Last Layer reduces data to dimensions 1x1xCH_OUT
// Assumption: Output Data is written back to where initial image was placed
// results: (float*) pointer to allocated memory
void copy_results_from_FPGA(network_t *net_CPU, float *results, int ch_out) {
  // Assumption: Output is of Dimension 1x1x(num_channels of last layer)
  int lastlayer = net_CPU->num_layers - 1;
  int results_size = ch_out * sizeof(float);
  // Verify that last layer reduces spatial dimensions to 1x1:
  assert(net_CPU->layers[lastlayer].pool == POOL_GLOBAL);

  // Assumption: Output Data is exactly where original input data was placed
  int result_offset = 0;

  // Copy Results Data:
  memcpy(results, DRAM_DATA + result_offset, results_size);
  printf("CPU: Copy Results from FPGA DRAM: %d Bytes\n", results_size);
}

// Fills input_image with pixels of format YYYXXX.0CH (for debugging)
void generate_structured_input_image(float *input_image, int win, int hin,
                                     int chin) {
  // STRUCTURED INPUT
  for (int x = 0; x < win; x++) {
    for (int y = 0; y < hin; y++) {
      for (int ch = 0; ch < chin; ch++) {
        float value = y * 1000.0 + x + ch / 1000.0;
        input_image[y * win * chin + x * chin + ch] = value;
      }
    }
  }
}

// Fills input_image with random floats between -100 and +100 (for testing)
// Set seed=-1 to shuffle the random generator
void generate_random_input_image(float *input_image, int win, int hin, int chin,
                                 int seed = 1) {
  // Expected Results:
  // For seed = 1 [default] and miniFire8UnitFilter, should get result 14154.350
  // for each class.

  // Enable for real randomness: (time() updates every second)
  if (seed == -1) {
    srand(time(NULL));
  } else {
    srand(seed);
  }

  // RANDOM INPUT
  // Generate Input Image (pixels between +-1, 3 places after zero)
  for (int x = 0; x < win; x++) {
    for (int y = 0; y < hin; y++) {
      for (int ch = 0; ch < chin; ch++) {
        //
        float value = (rand() % 2000 - 1000) / 2000.0 * 100;
        input_image[y * win * chin + x * chin + ch] = value;
      }
    }
  }
}

// Loads input_image with data from given file
// (should be prepared using convert_image.py)
void load_prepared_input_image(float *input_image, const char *filename,
                               int win, int hin, int chin) {
  // load from given file...
  int num_pixels = win * hin * chin;
  printf("Loading Input from File %s, %lu Bytes.\n", filename,
         num_pixels * sizeof(float));
  FILE *filehandle = fopen(filename, "r");
  // read portion of input file
  fread(input_image, sizeof(float), num_pixels, filehandle);
  fclose(filehandle);
}

void load_image_file(float *input_image, const char *filename, int win, int hin,
                     int chin) {}

void do_preprocess(float *input_image, int win, int hin, int chin) {
  for (int y = 0; y < hin; y++) {
    for (int x = 0; x < win; x++) {
      // Subtract Mean Pixel (defined in network.hpp)
      input_image[y * win * chin + x * chin + 0] -= MEAN_R;
      input_image[y * win * chin + x * chin + 1] -= MEAN_G;
      input_image[y * win * chin + x * chin + 2] -= MEAN_B;
    }
  }
}

void do_preparation_on_CPU(network_t *net_CPU) {
  // Print Network Config
  printf("\n\nNetwork Setup on CPU:\n=====================\n\n");
  print_layers(net_CPU);
  printf("\n");

  // Setup FPGA DRAM Memory: Layer Config + Weights
  allocate_FPGA_memory(net_CPU);

  // Copy Network Config: CPU Memory -> FPGA DRAM Memory
  printf("CPU: Copy Layer Config + Weights to FPGA DRAM:\n");
  copy_config_to_FPGA(net_CPU);

  // Copy Input Image to Memory:
  int win = net_CPU->layers[0].width;
  int hin = net_CPU->layers[0].height;
  int chin = net_CPU->layers[0].channels_in;
  float *input_image = (float *)malloc(win * hin * chin * sizeof(float));

  // generate_structured_input_image(input_image,win,hin,chin);
  // pseudo-random input
  // generate_random_input_image(input_image, win, hin, chin, 1);
  // really random input (srand-shuffled)
  // generate_random_input_image(input_image, win, hin, chin, -1);
  // load from image file (prepared by convert_image.py):
  load_prepared_input_image(input_image, "./indata.bin", win, hin, chin);
  // load_image_file(input_image, "./puppy-500x350.jpg", win, hin, chin);
  // do_preprocess(input_image, win, hin, chin);

  printf("CPU: Copy Input Image to FPGA DRAM:\n");
  copy_input_image_to_FPGA(net_CPU, input_image);
}

int main() {
  // CPU Memory: Create Network Config
  network_t *net_CPU;
  net_CPU = get_network_config();

  ///////
  // Preparation on CPU
  ///////

  // CPU: write network config + weights + input data to shared DRAM
  do_preparation_on_CPU(net_CPU);

  ///////
  // Execute Accelerator on FGPA
  ///////

  fpga_top(net_CPU->num_layers, (int *)DRAM,
           (unsigned long)DRAM_LAYER_CONFIG - (unsigned long)DRAM,
           (unsigned long)DRAM_WEIGHTS - (unsigned long)DRAM,
           (unsigned long)DRAM_DATA - (unsigned long)DRAM);

  ///////
  // Copy Results back from FPGA
  ///////

  // Fetch Results back from shared DRAM
  int ch_out = net_CPU->layers[net_CPU->num_layers - 1].channels_out;
  float *results = (float *)malloc(ch_out * sizeof(float));
  copy_results_from_FPGA(net_CPU, results, ch_out);

  ///////
  // Calculate Softmax on CPU
  // p_i = e^{r_i} / (\sum(e^{r_i}))
  ///////

  // First, finish AVERAGE Pooling (FPGA does just accumulation, no division)
  // Then, subtract maximum to avoid Numerical Issues in Exponentiation
  // Divide by WxH of Output Maps:
  int w = net_CPU->layers[net_CPU->num_layers - 1].width;
  int h = net_CPU->layers[net_CPU->num_layers - 1].height;
  if (net_CPU->layers[net_CPU->num_layers - 1].stride == 2) {
    w /= 2;
    h /= 2;
  }
  float out_dim = w * h;
  float maxresult = 0;
  DBG("average: ");
  for (int i = 0; i < ch_out; i++) {
    results[i] /= out_dim;
    maxresult = std::fmax(maxresult, results[i]);
    DBG(" %6.2f ", results[i]);
  }
  DBG(" (maximum: %6.2f)\n", maxresult);
  
  // Calculate Exponentials and Sum
  float expsum = 0;
  std::vector<float> exponentials(ch_out);
  for (int i = 0; i < ch_out; i++) {
    exponentials[i] = exp(results[i] - maxresult);
    expsum += exponentials[i];
  }
  DBG("sum of exponentials: %f\n", expsum);

  // Calculate Probabilities
  std::vector<std::pair<float, int> > probabilities(ch_out);
  for (int i = 0; i < ch_out; i++) {
    probabilities[i] = std::pair<float, int>(exponentials[i] / expsum, i);
    // printf("P(class %3d) = %4.2f%%\n", i, 100 * probabilities[i].first);
  }

  // Sort and Get Top5 / Top1
  std::sort(probabilities.begin(), probabilities.end());
  std::reverse(probabilities.begin(), probabilities.end());
  printf("\nTop-1: class %d (%4.2f%%)\n", probabilities[0].second,
         100 * probabilities[0].first);
  printf("Top-5:\n");
  for (int i = 0; i < std::min(5, ch_out); i++) {
    printf("    %4.2f%%: class %3d (output %6.2f)\n",
           100 * probabilities[i].first, probabilities[i].second,
           results[probabilities[i].second]);
  }
}