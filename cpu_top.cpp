#include "cpu_top.hpp"

// Pointers to FPGA DRAM Memory
char *DRAM; // addressed at byte-level
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
  printf("CPU: FPGA DRAM Memory Allocation:\nBytes allocated: %d Bytes "
         "(layer config) + %d Bytes (weights)\n\n",
         layerconfig_size, weights_size);
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
  printf("Input Data copied: %d Bytes (input image, %s)\n\n",
         net_CPU->layers[0].name, input_image_size);
}

// Copies Results back from FPGA memory;
// Assumes Last Layer reduces data to dimensions 1x1xCH_OUT
// Second Parameter: (float*) pointer to allocated memory
void copy_results_from_FPGA(network_t *net_CPU, float *results) {

  // Assumption: Output is of Dimension 1x1x(num_channels of last layer)
  int lastlayer = net_CPU->num_layers - 1;
  int chout = net_CPU->layers[lastlayer].channels_in;
  char *name = net_CPU->layers[lastlayer].name;
  int result_offset = net_CPU->layers[lastlayer].mem_addr_output;

  int results_size = chout * sizeof(float);
  results = (float *)malloc(results_size);
  printf("malloc returned %lu\n", (unsigned long)results);

  // Copy Results Data:
  memcpy(results, DRAM_DATA + result_offset, results_size);
  printf("Output Layer %s copied: %d Bytes (class results)\n\n", name,
         results_size);
}

void do_preparation_on_CPU(network_t *net_CPU) {

  int win = net_CPU->layers[1].width;
  int hin = net_CPU->layers[1].height;
  int chin = net_CPU->layers[1].channels_in;

  // CPU Memory
  float *input_image = (float *)malloc(win * hin * chin * sizeof(float));

  // STRUCTURED INPUT
  // Generate Input Image (pixels of format YYYXXX.0CH)
  for (int x = 0; x < win; x++) {
    for (int y = 0; y < hin; y++) {
      for (int ch = 0; ch < chin; ch++) {
        float value = y * 1000.0 + x + ch / 1000.0;
        input_image[y * win * chin + x * chin + ch] = value;
      }
    }
  }
  
  // RANDOM INPUT
  // Generate Input Image (pixels of format YYYXXX.0CH)
  for (int x = 0; x < win; x++) {
    for (int y = 0; y < hin; y++) {
      for (int ch = 0; ch < chin; ch++) {
        float value = (rand()*1.0/RAND_MAX - 0.5)*10;
        input_image[y * win * chin + x * chin + ch] = value;
      }
    }
  }

  // Print Network Config
  printf("\n\nNetwork Setup on CPU:\n=====================\n\n");
  print_layers(net_CPU);
  printf("\n");

  // Setup FPGA DRAM Memory: Layer Config + Weights
  allocate_FPGA_memory(net_CPU);

  // Copy Network Config: CPU Memory -> FPGA DRAM Memory
  printf("CPU: Copy Layer Config + Weights to FPGA DRAM:\n");
  copy_config_to_FPGA(net_CPU);

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
           (long)DRAM_LAYER_CONFIG - (long)DRAM,
           (long)DRAM_WEIGHTS - (long)DRAM, (long)DRAM_DATA - (long)DRAM);

  ///////
  // Finish on CPU (Softmax)
  ///////

  // Fetch Results back from shared DRAM
  int ch_out = net_CPU->layers[net_CPU->num_layers - 1].channels_in;
  float *results = (float *)malloc(ch_out * sizeof(float));
  copy_results_from_FPGA(net_CPU, results);

  printf("got %d results\n", ch_out);
  printf("placed at %lu\n", (unsigned long)results);
  for (int i = 0; i < ch_out; i++) {
    printf("Result %d: %010.4f\n", i, results[i]);
  }
}