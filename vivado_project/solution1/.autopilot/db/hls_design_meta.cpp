#include "hls_design_meta.h"
const Port_Property HLS_Design_Meta::port_props[]={
	Port_Property("ap_clk", 1, hls_in, -1, "", "", 1),
	Port_Property("ap_rst", 1, hls_in, -1, "", "", 1),
	Port_Property("ap_start", 1, hls_in, -1, "", "", 1),
	Port_Property("ap_done", 1, hls_out, -1, "", "", 1),
	Port_Property("ap_idle", 1, hls_out, -1, "", "", 1),
	Port_Property("ap_ready", 1, hls_out, -1, "", "", 1),
	Port_Property("img_in_address0", 4, hls_out, 0, "ap_memory", "mem_address", 1),
	Port_Property("img_in_ce0", 1, hls_out, 0, "ap_memory", "mem_ce", 1),
	Port_Property("img_in_q0", 32, hls_in, 0, "ap_memory", "mem_dout", 1),
	Port_Property("img_in_address1", 4, hls_out, 0, "ap_memory", "mem_address", 1),
	Port_Property("img_in_ce1", 1, hls_out, 0, "ap_memory", "mem_ce", 1),
	Port_Property("img_in_q1", 32, hls_in, 0, "ap_memory", "mem_dout", 1),
	Port_Property("filt_in_address0", 4, hls_out, 1, "ap_memory", "mem_address", 1),
	Port_Property("filt_in_ce0", 1, hls_out, 1, "ap_memory", "mem_ce", 1),
	Port_Property("filt_in_q0", 32, hls_in, 1, "ap_memory", "mem_dout", 1),
	Port_Property("filt_in_address1", 4, hls_out, 1, "ap_memory", "mem_address", 1),
	Port_Property("filt_in_ce1", 1, hls_out, 1, "ap_memory", "mem_ce", 1),
	Port_Property("filt_in_q1", 32, hls_in, 1, "ap_memory", "mem_dout", 1),
	Port_Property("sum_out", 32, hls_out, 2, "ap_vld", "out_data", 1),
	Port_Property("sum_out_ap_vld", 1, hls_out, 2, "ap_vld", "out_vld", 1),
	Port_Property("prod_out_address0", 4, hls_out, 3, "ap_memory", "mem_address", 1),
	Port_Property("prod_out_ce0", 1, hls_out, 3, "ap_memory", "mem_ce", 1),
	Port_Property("prod_out_we0", 1, hls_out, 3, "ap_memory", "mem_we", 1),
	Port_Property("prod_out_d0", 32, hls_out, 3, "ap_memory", "mem_din", 1),
	Port_Property("prod_out_address1", 4, hls_out, 3, "ap_memory", "mem_address", 1),
	Port_Property("prod_out_ce1", 1, hls_out, 3, "ap_memory", "mem_ce", 1),
	Port_Property("prod_out_we1", 1, hls_out, 3, "ap_memory", "mem_we", 1),
	Port_Property("prod_out_d1", 32, hls_out, 3, "ap_memory", "mem_din", 1),
};
const char* HLS_Design_Meta::dut_name = "do_macc";