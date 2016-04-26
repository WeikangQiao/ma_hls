############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################
open_project squeezenet
set_top do_macc
add_files processing_element.cpp
add_files -tb test_processing_element.cpp
open_solution "solution1"
set_part {xc7z045fbg676-1}
create_clock -period 10 -name default
#source "./squeezenet/solution1/directives.tcl"
csim_design -clean
csynth_design
cosim_design -rtl vhdl
export_design -format ip_catalog
