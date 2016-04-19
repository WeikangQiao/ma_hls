//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  run_tests.cpp
//
//  Main TestBench. Runs all other Tests.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include <cstdio>
#include <string>

///// ADJUST /////
extern bool test_channel_accum();
extern bool test_processing_element();
extern bool test_DRAM();
extern bool test_filter_unit();
extern bool test_image_unit();

///// ADJUST /////
bool (*test_fun[])(void) = {test_processing_element, test_channel_accum,
                            test_DRAM, test_filter_unit, test_image_unit};
std::string test_title[] = {"Processing Elements", "Channel Accum",
                            "DRAM Simulator", "Filter Unit", "Image Unit"};
int num_tests = sizeof(test_title) / sizeof(*test_title);

int runtest(int test_num, std::string test_title, bool (*test_fun)(void)) {
  printf("\n%d. %s \n", test_num, test_title.c_str());
  printf("–––––––––––––––––––––\n");
  bool success = test_fun();
  printf("–––––––––––––––––––––\n");
  printf("%s!\n\n",
         success ? "SUCCESS" : "ERROR");
  return success;
}

int main() {
  bool success = true;

  printf("RUNNING TEST SUITE\n");
  printf("=====================\n");

  for (int i = 0; i < num_tests; i++) {
    success &= runtest(i + 1, test_title[i], test_fun[i]);
  }

  printf("======================\n");
  printf("FINAL RESULT: %s!\n", success ? "SUCCESS" : "ERROR");
  return success ? 0 : -1;
}