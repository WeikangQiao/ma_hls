#include "netconfig.hpp"

void addLayer(network_t *net, layer_t layer, bool is_expand_layer,
              bool update_memory_address) {

  // Keep Track of Memory Locations for Activations + Weights
  static int next_activation_mem = 0;
  static int last_activation_mem = 0;
  static int next_weights_mem = 0;
  // align to 1MB memory borders
  float mem_border = 1024 * 1024;

  // Normal Layers:
  // - increase "next_activation_memory" to next available 1MB memory border
  // Data + First Expand1x1 Layers:
  // - do not increase memory addresses
  if (update_memory_address) { // "normal layer"
    last_activation_mem = next_activation_mem;
    // next free memory address = behind input of current layer
    next_activation_mem += layer.width * layer.height * layer.channels_in;
    // align to 1MB memory borders
    next_activation_mem = ceil(next_activation_mem / mem_border) * mem_border;
  }

  // Write Memory Addresses into Layer Config
  layer.mem_addr_input = last_activation_mem;
  layer.mem_addr_output = next_activation_mem;
  layer.mem_addr_weights = next_weights_mem;

  // Update next available Weights Memory Address for Layers which have Weights:
  if (layer.type == LAYER_CONV) {
    int num_weights = // conv + bias weights
        layer.channels_out * layer.channels_in * layer.kernel * layer.kernel +
        layer.channels_out;
    layer.mem_addr_weights = next_weights_mem;
    next_weights_mem += num_weights;
    // do not align --> calculation of necessary memory beforehand too difficult
    // next_weights_mem = ceil(next_weights_mem / mem_border) * mem_border;
  }

  // For "expand" layers:
  // - Read from same input location twice
  // - Write to slightly offset memory locations (interleave output channels)
  if (is_expand_layer && !update_memory_address) {
    next_activation_mem += layer.channels_out;
    layer.mem_addr_output = next_activation_mem;
    next_activation_mem = ceil(next_activation_mem / mem_border) * mem_border;
  }
  layer.is_expand_layer = is_expand_layer;

  // Add Layer to network
  net->layers[net->num_layers] = layer;
  net->num_layers++;
  net->total_pixel_mem = next_activation_mem;
};

void loadWeightsFromFile(network_t *net, char *filename) {

  FILE *f = fopen(filename, "r");

  for (int i = 0; i < net->num_layers; i++) {
    layer_t *layer = &net->layers[i];
    if (layer->type == LAYER_CONV) {
      int num_weights = // conv + bias weights
          layer->channels_out * layer->channels_in * layer->kernel *
              layer->kernel +
          layer->channels_out;
      float *weights_addr =
          (float *)(net->weights + sizeof(float) * layer->mem_addr_weights);
      fread(weights_addr, sizeof(float), num_weights, f);
    }
  }

  fclose(f);
}

// char *stradd(char *name, char *suffix) {
//   char *fullname = (char *)malloc(NET_NAME_MAX_LEN * sizeof(char));
//   strncpy(fullname, name, NET_NAME_MAX_LEN);
//   int l = strlen(fullname);
//   strncpy(&fullname[l], suffix, NET_NAME_MAX_LEN - l);
//   return fullname;
// }

// Print List of all Layers + Attributes + Memory Locations
void print_layers(network_t *net) {

  for (int i = 0; i < net->num_layers; i++) {
    layer_t *layer = &net->layers[i];
    int memory_needed = layer->height * layer->width * layer->channels_in;
    int weights_size = layer->kernel * layer->kernel * layer->channels_in *
                           layer->channels_out +
                       layer->channels_out;
    
    printf("%6s: IN %3d x %3d x %3d @mem(%6lu-%6lukB), OUT @mem(%6lukB)",
           layer->name, (int)layer->height, (int)layer->width,
           (int)layer->channels_in,
           layer->mem_addr_input * sizeof(float) / 1024,
           (layer->mem_addr_input + memory_needed) * sizeof(float) / 1024,
           layer->mem_addr_output * sizeof(float) / 1024);

    switch (layer->type) {
    case LAYER_CONV:
      printf(", OP (%dx%d)/%d%s", (int)layer->kernel, (int)layer->kernel,
             (int)layer->stride, layer->pad ? "p" : " ");
      printf(", PARAM @mem(%4lu-%4lukB)",
             layer->mem_addr_weights * sizeof(float) / 1024,
             (layer->mem_addr_weights + weights_size) * sizeof(float) / 1024);
      if (layer->is_expand_layer)
        printf(" (exact out %luB)", layer->mem_addr_output * sizeof(float));
      printf("\n");
      break;

    case LAYER_POOL:
      printf(", OP (%dx%d)/%d%s", (int)layer->kernel, (int)layer->kernel,
             (int)layer->stride, (int)layer->pad ? "p" : " ");
      printf("\n");
      break;

    default:
      printf("\n");
      break;
    }

  }
}