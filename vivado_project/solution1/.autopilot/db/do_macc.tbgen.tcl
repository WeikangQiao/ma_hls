set C_TypeInfoList {{ 
"do_macc" : [[], { "return": [[], "void"]} , [{"ExternC" : 0}], [ {"img_in": [[], {"array": [ {"scalar": "float"}, [9]]}] }, {"filt_in": [[], {"array": [ {"scalar": "float"}, [9]]}] }, {"sum_out": [[], {"reference":  {"scalar": "float"}}] }, {"prod_out": [[], {"array": [ {"scalar": "float"}, [9]]}] }],[],""]
}}
set moduleName do_macc
set isCombinational 0
set isDatapathOnly 0
set isPipelined 0
set pipeline_type none
set FunctionProtocol ap_ctrl_hs
set isOneStateSeq 0
set C_modelName {do_macc}
set C_modelType { void 0 }
set C_modelArgList { 
	{ img_in float 32 regular {array 9 { 1 1 } 1 1 }  }
	{ filt_in float 32 regular {array 9 { 1 1 } 1 1 }  }
	{ sum_out float 32 regular {pointer 1}  }
	{ prod_out float 32 regular {array 9 { 0 0 } 0 1 }  }
}
set C_modelArgMapList {[ 
	{ "Name" : "img_in", "interface" : "memory", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "img_in","cData": "float","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 8,"step" : 1}]}]}]} , 
 	{ "Name" : "filt_in", "interface" : "memory", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "filt_in","cData": "float","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 8,"step" : 1}]}]}]} , 
 	{ "Name" : "sum_out", "interface" : "wire", "bitwidth" : 32, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "sum_out","cData": "float","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 0,"step" : 1}]}]}]} , 
 	{ "Name" : "prod_out", "interface" : "memory", "bitwidth" : 32, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "prod_out","cData": "float","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 8,"step" : 1}]}]}]} ]}
# RTL Port declarations: 
set portNum 28
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
	{ sum_out sc_out sc_lv 32 signal 2 } 
	{ sum_out_ap_vld sc_out sc_logic 1 outvld 2 } 
	{ prod_out_address0 sc_out sc_lv 4 signal 3 } 
	{ prod_out_ce0 sc_out sc_logic 1 signal 3 } 
	{ prod_out_we0 sc_out sc_logic 1 signal 3 } 
	{ prod_out_d0 sc_out sc_lv 32 signal 3 } 
	{ prod_out_address1 sc_out sc_lv 4 signal 3 } 
	{ prod_out_ce1 sc_out sc_logic 1 signal 3 } 
	{ prod_out_we1 sc_out sc_logic 1 signal 3 } 
	{ prod_out_d1 sc_out sc_lv 32 signal 3 } 
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
 	{ "name": "sum_out", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "sum_out", "role": "default" }} , 
 	{ "name": "sum_out_ap_vld", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "outvld", "bundle":{"name": "sum_out", "role": "ap_vld" }} , 
 	{ "name": "prod_out_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "prod_out", "role": "address0" }} , 
 	{ "name": "prod_out_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "ce0" }} , 
 	{ "name": "prod_out_we0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "we0" }} , 
 	{ "name": "prod_out_d0", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "prod_out", "role": "d0" }} , 
 	{ "name": "prod_out_address1", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "prod_out", "role": "address1" }} , 
 	{ "name": "prod_out_ce1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "ce1" }} , 
 	{ "name": "prod_out_we1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "prod_out", "role": "we1" }} , 
 	{ "name": "prod_out_d1", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "prod_out", "role": "d1" }}  ]}
set Spec2ImplPortList { 
	img_in { ap_memory {  { img_in_address0 mem_address 1 4 }  { img_in_ce0 mem_ce 1 1 }  { img_in_q0 mem_dout 0 32 }  { img_in_address1 mem_address 1 4 }  { img_in_ce1 mem_ce 1 1 }  { img_in_q1 mem_dout 0 32 } } }
	filt_in { ap_memory {  { filt_in_address0 mem_address 1 4 }  { filt_in_ce0 mem_ce 1 1 }  { filt_in_q0 mem_dout 0 32 }  { filt_in_address1 mem_address 1 4 }  { filt_in_ce1 mem_ce 1 1 }  { filt_in_q1 mem_dout 0 32 } } }
	sum_out { ap_vld {  { sum_out out_data 1 32 }  { sum_out_ap_vld out_vld 1 1 } } }
	prod_out { ap_memory {  { prod_out_address0 mem_address 1 4 }  { prod_out_ce0 mem_ce 1 1 }  { prod_out_we0 mem_we 1 1 }  { prod_out_d0 mem_din 1 32 }  { prod_out_address1 mem_address 1 4 }  { prod_out_ce1 mem_ce 1 1 }  { prod_out_we1 mem_we 1 1 }  { prod_out_d1 mem_din 1 32 } } }
}

# RTL port scheduling information:
set fifoSchedulingInfoList { 
}

# RTL bus port read request latency information:
set busReadReqLatencyList { 
}

# RTL bus port write response latency information:
set busWriteResLatencyList { 
}

# RTL array port load latency information:
set memoryLoadLatencyList { 
}
