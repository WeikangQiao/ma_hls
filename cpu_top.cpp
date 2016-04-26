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
  DRAM = (char *)malloc(configsize + weightsize + datasize);
  DRAM_LAYER_CONFIG = (layer_t *)(DRAM + 0);
  DRAM_WEIGHTS = (float *)(DRAM + configsize);
  DRAM_DATA = (float *)(DRAM + weightsize);

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

  int input_image_size = 256 * 256 * 3 * sizeof(float);
  // Copy Input Data:
  memcpy(DRAM_DATA, image, input_image_size);
  printf("Bytes copied: %d Bytes (input image)\n\n", input_image_size);
}

void do_preparation_on_CPU(network_t *net_CPU) {

  // CPU Memory
  float input_image[256 * 256 * 3];

  // Generate Input Image (pixels of format YYYXXX.0CH)
  for (int x = 0; x < 256; x++) {
    for (int y = 0; y < 256; y++) {
      for (int ch = 0; ch < 3; ch++) {
        float value = y * 1000.0 + x + ch / 1000.0;
        input_image[y * 256 * 3 + x * 3 + ch] = value;
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

  //     (CPU: write network config + weights + input data to shared DRAM)
  do_preparation_on_CPU(net_CPU);

  ///////
  // Execution on FGPA
  ///////

  fpga_top(net_CPU->num_layers, (int*)DRAM, (long)DRAM_LAYER_CONFIG - (long)DRAM,
           (long)DRAM_WEIGHTS - (long)DRAM, (long)DRAM_DATA - (long)DRAM);
}