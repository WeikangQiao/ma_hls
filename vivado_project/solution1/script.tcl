############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
############################################################
open_project vivado_project
set_top fpga_top
add_files network.hpp
add_files netconfig.hpp
add_files fpga_top.hpp
add_files fpga_top.cpp
add_files -tb weights.bin
add_files -tb network.hpp
add_files -tb network.cpp
add_files -tb netconfig.hpp
add_files -tb netconfig.cpp
add_files -tb indata.bin
add_files -tb cpu_top.hpp
add_files -tb cpu_top.cpp
open_solution "solution1"
set_part {xc7z045fbg676-1}
create_clock -period 5 -name default
config_compile -name_max_length 30 -no_signed_zeros -pipeline_loops 0 -unsafe_math_optimizations
config_interface -clock_enable -m_axi_offset direct -register_io scalar_all -trim_dangling_port
#source "./vivado_project/solution1/directives.tcl"
csim_design -clean -O
csynth_design
cosim_design -O -trace_level port -rtl vhdl
export_design -format ip_catalog
