
#include "processing_element.hpp"
#include <cstdio>
#include <cmath>
#include <stdlib.h>  

static float ALLOWED_ERR = 1e-6;

processing_element PE;
float input[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
float filt[9] = {0, 0, 0, 1, 0, 0, 0, 0, 0};
float out = 0;
float products[9];

bool check_products() {
  printf("CHECKING PRODUCT:\n");
  // check products:
  for (int i = 0; i < 9; i++) {
    printf("checking element %d: ", i);

    float expected = input[i] * filt[i];
    float acquired = products[i];

    if (fabs(expected - acquired) > ALLOWED_ERR) {
      printf("ERROR! expected: %f != acquired: %f\n", expected, acquired);
      return false;
    } else {
      printf("ok.\n");
    }
  }
  return true;
}

bool check_sum() {
  printf("CHECKING SUM:\n");
  // check sum:
  float mysum = 0;
  for (int i = 0; i < 9; i++) {
    mysum += products[i];
  }
  if (fabs(mysum - out) > ALLOWED_ERR) {
    printf("ERROR! expected: %f != acquired: %f\n", mysum, out);
    return false;
  } else {
    printf("%f, ok.\n", mysum);
    return true;
  }
}

int main() {

  // Test 1: Simple Filter, Fixed Image
  PE.macc(input, filt, out, products);
  if (!check_products())
    return -1;
  if (!check_sum())
    return -1;
  
  // Test 2: Random Data
  for (int i = 0; i < 9; i++) {
    input[i] = rand()/(float)RAND_MAX;
    filt[i] = rand()/(float)RAND_MAX;
    printf("elem %d: %f * %f", i, input[i], filt[i]);
  }
  PE.macc(input, filt, out, products);
  if (!check_products())
    return -1;
  if (!check_sum())
    return -1;
  
}
