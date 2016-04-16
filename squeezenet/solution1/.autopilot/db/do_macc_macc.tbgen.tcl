set moduleName do_macc_macc
set isCombinational 0
set isDatapathOnly 0
set isPipelined 1
set pipeline_type function
set FunctionProtocol ap_ctrl_hs
set isOneStateSeq 0
set pipeII 5
set pipeLatency 53
set C_modelName {do_macc_macc}
set C_modelType { float 32 }
set C_modelArgList { 
	{ img_in float 32 regular {array 9 { 1 1 } 1 1 }  }
	{ filt_in float 32 regular {array 9 { 1 1 } 1 1 }  }
	{ prod_out float 32 regular {array 9 { 0 0 } 0 1 }  }
}
set C_modelArgMapList {[ 
	{ "Name" : "img_in", "interface" : "memory", "bitwidth" : 32, "direction" : "READONLY"} , 
 	{ "Name" : "filt_in", "interface" : "memory", "bitwidth" : 32, "direction" : "READONLY"} , 
 	{ "Name" : "prod_out", "interface" : "memory", "bitwidth" : 32, "direction" : "WRITEONLY"} , 
 	{ "Name" : "ap_return", "interface" : "wire", "bitwidth" : 32} ]}
# RTL Port declarations: 
set portNum 27
set portList { 
	{ ap_clk sc_in sc_logic 1 clock -1 } 
	{ ap_rst sc_in sc_logic 1 reset -1 active_high_sync } 
	{ ap_start sc_in sc_logic 1 start -1 } 
	{ ap_done sc_out sc_logic 1 predone -1 } 
	{ ap_idle sc_out sc_logic 1 done -1 } 
	{ ap_ready sc_out sc_logic 1 ready -1 } 
	{ img_in_address0 sc_out sc_lv 4 signal 0 } 
	{ img_in_ce0 sc_out sc_logic 1 signal 0 } 
	{ img_in_q0 sc_in sc_lv 32 signal 0 } 
	{ img_in_address1 sc_out sc_lv 4 signal 0 } 
	{ img_in_ce1 sc_out sc_logic 1 signal 0 } 
	{ img_in_q1 sc_in sc_lv 32 signal 0 } 
	{ filt_in_address0 sc_out sc_lv 4 signal 1 } 
	{ filt_in_ce0 sc_out sc_logic 1 signal 1 } 
	{ filt_in_q0 sc_in sc_lv 32 signal 1 } 
	{ filt_in_address1 sc_out sc_lv 4 signal 1 } 
	{ filt_in_ce1 sc_out sc_logic 1 signal 1 } 
	{ filt_in_q1 sc_in sc_lv 32 signal 1 } 
	{ prod_out_address0 sc_out sc_lv 4 signal 2 } 
	{ prod_out_ce0 sc_out sc_logic 1 signal 2 } 
	{ prod_out_we0 sc_out sc_logic 1 signal 2 } 
	{ prod_out_d0 sc_out sc_lv 32 signal 2 } 
	{ prod_out_address1 sc_out sc_lv 4 signal 2 } 
	{ prod_out_ce1 sc_out sc_logic 1 signal 2 } 
	{ prod_out_we1 sc_out sc_logic 1 signal 2 } 
	{ prod_out_d1 sc_out sc_lv 32 signal 2 } 
	{ ap_return sc_out sc_lv 32 signal -1 } 
}
set NewPortList {[ 
	{ "name": "ap_clk", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "clock", "bundle":{"name": "ap_clk", "role": "default" }} , 
 	{ "name": "ap_rst", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "reset", "bundle":{"name": "ap_rst", "role": "default" }} , 
 	{ "name": "ap_start", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "start", "bundle":{"name": "ap_start", "role": "default" }} , 
 	{ "name": "ap_done", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "predone", "bundle":{"name": "ap_done", "role": "default" }} , 
 	{ "name": "ap_idle", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "done", "bundle":{"name": "ap_idle", "role": "default" }} , 
 	{ "name": "ap_ready", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "ready", "bundle":{"name": "ap_ready", "role": "default" }} , 
 	{ "name": "img_in_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "img_in", "role": "address0" }} , 
 	{ "name": "img_in_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "img_in", "role": "ce0" }} , 
 	{ "name": "img_in_q0", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "img_in", "role": "q0" }} , 
 	{ "name": "img_in_address1", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "img_in", "role": "address1" }} , 
 	{ "name": "img_in_ce1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "img_in", "role": "ce1" }} , 
 	{ "name": "img_in_q1", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "img_in", "role": "q1" }} , 
 	{ "name": "filt_in_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "filt_in", "role": "address0" }} , 
 	{ "name": "filt_in_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "filt_in", "role": "ce0" }} , 
 	{ "name": "filt_in_q0", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "filt_in", "role": "q0" }} , 
 	{ "name": "filt_in_address1", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "filt_in", "role": "address1" }} , 
 	{ "name": "filt_in_ce1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "filt_in", "role": "ce1" }} , 
 	{ "name": "filt_in_q1", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "filt_in", "role": "q1" }} , 
 	{ "name": "prod_out_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "prod_out", "role": "address0" }} , 
 	{ "name": "prod_out_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "ce0" }} , 
 	{ "name": "prod_out_we0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "we0" }} , 
 	{ "name": "prod_out_d0", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "prod_out", "role": "d0" }} , 
 	{ "name": "prod_out_address1", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "prod_out", "role": "address1" }} , 
 	{ "name": "prod_out_ce1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "ce1" }} , 
 	{ "name": "prod_out_we1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "we1" }} , 
 	{ "name": "prod_out_d1", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "prod_out", "role": "d1" }} , 
 	{ "name": "ap_return", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "ap_return", "role": "default" }}  ]}
set Spec2ImplPortList { 
	img_in { ap_memory {  { img_in_address0 mem_address 1 4 }  { img_in_ce0 mem_ce 1 1 }  { img_in_q0 mem_dout 0 32 }  { img_in_address1 mem_address 1 4 }  { img_in_ce1 mem_ce 1 1 }  { img_in_q1 mem_dout 0 32 } } }
	filt_in { ap_memory {  { filt_in_address0 mem_address 1 4 }  { filt_in_ce0 mem_ce 1 1 }  { filt_in_q0 mem_dout 0 32 }  { filt_in_address1 mem_address 1 4 }  { filt_in_ce1 mem_ce 1 1 }  { filt_in_q1 mem_dout 0 32 } } }
	prod_out { ap_memory {  { prod_out_address0 mem_address 1 4 }  { prod_out_ce0 mem_ce 1 1 }  { prod_out_we0 mem_we 1 1 }  { prod_out_d0 mem_din 1 32 }  { prod_out_address1 mem_address 1 4 }  { prod_out_ce1 mem_ce 1 1 }  { prod_out_we1 mem_we 1 1 }  { prod_out_d1 mem_din 1 32 } } }
}
