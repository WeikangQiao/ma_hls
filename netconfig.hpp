//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  netconfig.h
//
//  Declaration of <layer_t> and <network_t> structs that hold configuration.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef _NETCONFIG_H_
#define _NETCONFIG_H_

#include <string>
#include <cmath>
#include <cstdlib>
#include "ap_int.h"

typedef ap_uint<6> layernum_t;
typedef ap_uint<9> dimension_t;
typedef ap_uint<10> channel_t;
typedef ap_uint<2> kernel_t;
typedef ap_uint<1> pad_t;
typedef ap_uint<2> stride_t;  // only stride 1 or 2 supported

#define NET_NAME_MAX_LEN 6

typedef enum {
  LAYER_DATA,
  LAYER_CONV,
  // LAYER_RELU,  // implicit after LAYER_CONV
  LAYER_POOL,
  LAYER_NONE
} layertype_t;

struct layer_t {
  char name[NET_NAME_MAX_LEN];
  layertype_t type;
  dimension_t width;
  dimension_t height;
  channel_t channels_in;
  channel_t channels_out;
  kernel_t kernel;
  pad_t pad;
  stride_t stride;
  int mem_addr_input;
  int mem_addr_output;
  int mem_addr_weights;
  bool is_expand_layer;
  layer_t(char *n, layertype_t t, int w, int h, int ci, int co, int k, int p,
          int s, int mem_i = 0, int mem_o = 0, int mem_w = 0,
          bool is_expand = false)
      : type(t), width(w), height(h), channels_in(ci), channels_out(co),
        kernel(k), pad(p), stride(s), mem_addr_input(mem_i),
        mem_addr_output(mem_o), mem_addr_weights(mem_w),
        is_expand_layer(is_expand) {
    strncpy(name, n, NET_NAME_MAX_LEN);
  };
  layer_t()
      : name(""), type(LAYER_NONE), width(0), height(0), channels_in(0), channels_out(0),
        kernel(0), pad(0), stride(0), mem_addr_input(0), mem_addr_output(0),
        mem_addr_weights(0), is_expand_layer(0) {
  };
};

struct network_t {
  layer_t *layers;
  int num_layers;
  float *weights;
  int num_weights;
  int total_pixel_mem;
  network_t(int max_num_layers, int max_num_weights)
      : num_layers(0), num_weights(0), total_pixel_mem(0) {
    layers = (layer_t *)malloc((sizeof(layer_t)) * max_num_layers);
    weights = (float *)malloc((sizeof(float)) * max_num_weights);
  }
};

void print_layers(network_t *net);

void addLayer(network_t *net, layer_t layer, bool is_expand_layer = false,
              bool update_memory_address = true);

void loadWeightsFromFile(network_t *net, char *filename);

#endif