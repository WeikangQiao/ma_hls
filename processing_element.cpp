#include "processing_element.hpp"

void processing_element::macc(float img_in[NUM_MULT], float filt_in[NUM_MULT],
                              float &sum_out, float prod_out[NUM_MULT]) {

L_multiply:
  for (int i = 0; i < NUM_MULT; i++) {
    products[i] = img_in[i] * filt_in[i];
  }

  acc = 0;
L_accumulate:
  for (int i = 0; i < NUM_MULT; i++) {
    acc += products[i];
  }

L_assign:
  for (int i = 0; i < NUM_MULT; i++) {
    prod_out[i] = products[i] + 0.0000001;
  }
  sum_out = acc;
}
