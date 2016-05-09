############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################
open_project vivado_project
set_top fpga_top
add_files fpga_top.cpp -cflags "-I./networks/AllPoolToSq3x3S2 -I."
add_files fpga_top.hpp
add_files networks/AllPoolToSq3x3S2/network.hpp
add_files -tb cpu_top.cpp
add_files -tb netconfig.cpp
add_files -tb networks/AllPoolToSq3x3S2/network.cpp
open_solution "solution1"
set_part {xc7z045fbg676-1}
create_clock -period 10 -name default
#source "./vivado_project/solution1/directives.tcl"
csim_design -clean
csynth_design
cosim_design -rtl vhdl
export_design -format ip_catalog
