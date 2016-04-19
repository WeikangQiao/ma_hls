//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  DRAM.hpp
//
//  DRAM Simulator
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

// Size of all Filters together: 1.42M Floats -> Memory for 2 Million Floats
static const int DRAM_MAX_FILTER_SIZE = 2 * 1024 * 1024;
// Largest Feature Map: 128x128x128M = 2M -> provide Memory for 2 Million Floats
static const int DRAM_MAX_IMAGE_SIZE = 2 * 1024 * 1024;

// DRAM Simulator
template <typename T>
class DRAM {

private:
  // Internal Wires / Connections:
  T mem[DRAM_MAX_IMAGE_SIZE];

public:
  // Public Interface:
  void write(int addr, T data) { mem[addr] = data; }
  T read(int addr) { return mem[addr]; }
};
