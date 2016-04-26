; ModuleID = '/home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/a.o.3.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@llvm_global_ctors_1 = appending global [1 x void ()*] [void ()* @_GLOBAL__I_a] ; [#uses=0 type=[1 x void ()*]*]
@llvm_global_ctors_0 = appending global [1 x i32] [i32 65535] ; [#uses=0 type=[1 x i32]*]
@do_macc_str = internal unnamed_addr constant [8 x i8] c"do_macc\00" ; [#uses=1 type=[8 x i8]*]
@p_str = private unnamed_addr constant [1 x i8] zeroinitializer, align 1 ; [#uses=1 type=[1 x i8]*]

; [#uses=1]
define internal fastcc float @do_macc_macc([9 x float]* nocapture %img_in, [9 x float]* nocapture %filt_in, [9 x float]* nocapture %prod_out) {
  call void @llvm.dbg.value(metadata !{[9 x float]* %img_in}, i64 0, metadata !7), !dbg !27 ; [debug line = 3:37] [debug variable = img_in]
  call void @llvm.dbg.value(metadata !{[9 x float]* %filt_in}, i64 0, metadata !28), !dbg !29 ; [debug line = 3:61] [debug variable = filt_in]
  call void @llvm.dbg.value(metadata !{[9 x float]* %prod_out}, i64 0, metadata !30), !dbg !31 ; [debug line = 4:53] [debug variable = prod_out]
  call void (...)* @_ssdm_op_SpecPipeline(i32 1, i32 1, i32 1, i32 0, [1 x i8]* @p_str) nounwind, !dbg !32 ; [debug line = 5:1]
  %img_in_addr = getelementptr [9 x float]* %img_in, i64 0, i64 0, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load = load float* %img_in_addr, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr = getelementptr [9 x float]* %filt_in, i64 0, i64 0, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load = load float* %filt_in_addr, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products = fmul float %img_in_load, %filt_in_load, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_1 = getelementptr [9 x float]* %img_in, i64 0, i64 1, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_1 = load float* %img_in_addr_1, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_1 = getelementptr [9 x float]* %filt_in, i64 0, i64 1, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_1 = load float* %filt_in_addr_1, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_1_s = fmul float %img_in_load_1, %filt_in_load_1, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_2 = getelementptr [9 x float]* %img_in, i64 0, i64 2, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_2 = load float* %img_in_addr_2, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_2 = getelementptr [9 x float]* %filt_in, i64 0, i64 2, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_2 = load float* %filt_in_addr_2, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_2_s = fmul float %img_in_load_2, %filt_in_load_2, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_3 = getelementptr [9 x float]* %img_in, i64 0, i64 3, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_3 = load float* %img_in_addr_3, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_3 = getelementptr [9 x float]* %filt_in, i64 0, i64 3, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_3 = load float* %filt_in_addr_3, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_3_s = fmul float %img_in_load_3, %filt_in_load_3, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_4 = getelementptr [9 x float]* %img_in, i64 0, i64 4, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_4 = load float* %img_in_addr_4, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_4 = getelementptr [9 x float]* %filt_in, i64 0, i64 4, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_4 = load float* %filt_in_addr_4, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_4_s = fmul float %img_in_load_4, %filt_in_load_4, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_5 = getelementptr [9 x float]* %img_in, i64 0, i64 5, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_5 = load float* %img_in_addr_5, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_5 = getelementptr [9 x float]* %filt_in, i64 0, i64 5, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_5 = load float* %filt_in_addr_5, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_5_s = fmul float %img_in_load_5, %filt_in_load_5, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_6 = getelementptr [9 x float]* %img_in, i64 0, i64 6, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_6 = load float* %img_in_addr_6, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_6 = getelementptr [9 x float]* %filt_in, i64 0, i64 6, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_6 = load float* %filt_in_addr_6, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_6_s = fmul float %img_in_load_6, %filt_in_load_6, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_7 = getelementptr [9 x float]* %img_in, i64 0, i64 7, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_7 = load float* %img_in_addr_7, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_7 = getelementptr [9 x float]* %filt_in, i64 0, i64 7, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_7 = load float* %filt_in_addr_7, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_7_s = fmul float %img_in_load_7, %filt_in_load_7, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in_addr_8 = getelementptr [9 x float]* %img_in, i64 0, i64 8, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in_load_8 = load float* %img_in_addr_8, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in_addr_8 = getelementptr [9 x float]* %filt_in, i64 0, i64 8, !dbg !34 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in_load_8 = load float* %filt_in_addr_8, align 4, !dbg !34 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element_products_8_s = fmul float %img_in_load_8, %filt_in_load_8, !dbg !34 ; [#uses=2 type=float] [debug line = 10:5]
  %tmp_s = fadd float %processing_element_products, 0.000000e+00, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_1 = fadd float %tmp_s, %processing_element_products_1_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_2 = fadd float %tmp_5_1, %processing_element_products_2_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_3 = fadd float %tmp_5_2, %processing_element_products_3_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_4 = fadd float %tmp_5_3, %processing_element_products_4_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_5 = fadd float %tmp_5_4, %processing_element_products_5_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_6 = fadd float %tmp_5_5, %processing_element_products_6_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_7 = fadd float %tmp_5_6, %processing_element_products_7_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp_5_8 = fadd float %tmp_5_7, %processing_element_products_8_s, !dbg !37 ; [#uses=1 type=float] [debug line = 17:5]
  %prod_out_addr = getelementptr [9 x float]* %prod_out, i64 0, i64 0, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products, float* %prod_out_addr, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_1 = getelementptr [9 x float]* %prod_out, i64 0, i64 1, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_1_s, float* %prod_out_addr_1, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_2 = getelementptr [9 x float]* %prod_out, i64 0, i64 2, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_2_s, float* %prod_out_addr_2, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_3 = getelementptr [9 x float]* %prod_out, i64 0, i64 3, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_3_s, float* %prod_out_addr_3, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_4 = getelementptr [9 x float]* %prod_out, i64 0, i64 4, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_4_s, float* %prod_out_addr_4, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_5 = getelementptr [9 x float]* %prod_out, i64 0, i64 5, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_5_s, float* %prod_out_addr_5, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_6 = getelementptr [9 x float]* %prod_out, i64 0, i64 6, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_6_s, float* %prod_out_addr_6, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_7 = getelementptr [9 x float]* %prod_out, i64 0, i64 7, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_7_s, float* %prod_out_addr_7, align 4, !dbg !40 ; [debug line = 23:5]
  %prod_out_addr_8 = getelementptr [9 x float]* %prod_out, i64 0, i64 8, !dbg !40 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element_products_8_s, float* %prod_out_addr_8, align 4, !dbg !40 ; [debug line = 23:5]
  ret float %tmp_5_8
}

; [#uses=7]
declare void @llvm.dbg.value(metadata, i64, metadata) nounwind readnone

; [#uses=0]
define void @do_macc([9 x float]* %img_in, [9 x float]* %filt_in, float* %sum_out, [9 x float]* %prod_out) nounwind uwtable {
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %img_in) nounwind, !map !43
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %filt_in) nounwind, !map !49
  call void (...)* @_ssdm_op_SpecBitsMap(float* %sum_out) nounwind, !map !53
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %prod_out) nounwind, !map !57
  call void (...)* @_ssdm_op_SpecTopModule([8 x i8]* @do_macc_str) nounwind
  call void @llvm.dbg.value(metadata !{[9 x float]* %img_in}, i64 0, metadata !61), !dbg !65 ; [debug line = 29:20] [debug variable = img_in]
  call void @llvm.dbg.value(metadata !{[9 x float]* %filt_in}, i64 0, metadata !66), !dbg !67 ; [debug line = 29:44] [debug variable = filt_in]
  call void @llvm.dbg.value(metadata !{float* %sum_out}, i64 0, metadata !68), !dbg !69 ; [debug line = 30:38] [debug variable = sum_out]
  call void @llvm.dbg.value(metadata !{[9 x float]* %prod_out}, i64 0, metadata !70), !dbg !71 ; [debug line = 30:53] [debug variable = prod_out]
  %call_ret = call fastcc float @do_macc_macc([9 x float]* %img_in, [9 x float]* %filt_in, [9 x float]* %prod_out) nounwind, !dbg !72 ; [#uses=1 type=float] [debug line = 32:2]
  call void @_ssdm_op_Write.ap_auto.floatP(float* %sum_out, float %call_ret) nounwind, !dbg !72 ; [debug line = 32:2]
  ret void, !dbg !74                              ; [debug line = 33:1]
}

; [#uses=1]
define weak void @_ssdm_op_Write.ap_auto.floatP(float*, float) {
entry:
  store float %1, float* %0
  ret void
}

; [#uses=1]
define weak void @_ssdm_op_SpecTopModule(...) {
entry:
  ret void
}

; [#uses=1]
define weak void @_ssdm_op_SpecPipeline(...) nounwind {
entry:
  ret void
}

; [#uses=4]
define weak void @_ssdm_op_SpecBitsMap(...) {
entry:
  ret void
}

; [#uses=0]
declare i16 @_ssdm_op_HSub(...)

; [#uses=0]
declare i16 @_ssdm_op_HMul(...)

; [#uses=0]
declare i16 @_ssdm_op_HDiv(...)

; [#uses=0]
declare i16 @_ssdm_op_HAdd(...)

; [#uses=1]
declare void @_GLOBAL__I_a() nounwind section ".text.startup"

!hls.encrypted.func = !{}
!llvm.map.gv = !{!0}

!0 = metadata !{metadata !1, [1 x i32]* @llvm_global_ctors_0}
!1 = metadata !{metadata !2}
!2 = metadata !{i32 0, i32 31, metadata !3}
!3 = metadata !{metadata !4}
!4 = metadata !{metadata !"llvm.global_ctors.0", metadata !5, metadata !"", i32 0, i32 31}
!5 = metadata !{metadata !6}
!6 = metadata !{i32 0, i32 0, i32 1}
!7 = metadata !{i32 786689, metadata !8, metadata !"img_in", null, i32 3, metadata !17, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!8 = metadata !{i32 786478, i32 0, null, metadata !"macc", metadata !"macc", metadata !"_ZN18processing_element4maccEPfS0_RfS0_", metadata !9, i32 3, metadata !10, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, null, null, metadata !22, metadata !23, i32 4} ; [ DW_TAG_subprogram ]
!9 = metadata !{i32 786473, metadata !"processing_element.cpp", metadata !"/home/sem14h17/MA/ma_hls", null} ; [ DW_TAG_file_type ]
!10 = metadata !{i32 786453, i32 0, metadata !"", i32 0, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !11, i32 0, i32 0} ; [ DW_TAG_subroutine_type ]
!11 = metadata !{null, metadata !12, metadata !25, metadata !25, metadata !26, metadata !25}
!12 = metadata !{i32 786447, i32 0, metadata !"", i32 0, i32 0, i64 64, i64 64, i64 0, i32 64, metadata !13} ; [ DW_TAG_pointer_type ]
!13 = metadata !{i32 786434, null, metadata !"processing_element", metadata !14, i32 3, i64 320, i64 32, i32 0, i32 0, null, metadata !15, i32 0, null, null} ; [ DW_TAG_class_type ]
!14 = metadata !{i32 786473, metadata !"./processing_element.hpp", metadata !"/home/sem14h17/MA/ma_hls", null} ; [ DW_TAG_file_type ]
!15 = metadata !{metadata !16, metadata !21, metadata !22}
!16 = metadata !{i32 786445, metadata !13, metadata !"products", metadata !14, i32 6, i64 288, i64 32, i64 0, i32 1, metadata !17} ; [ DW_TAG_member ]
!17 = metadata !{i32 786433, null, metadata !"", null, i32 0, i64 288, i64 32, i32 0, i32 0, metadata !18, metadata !19, i32 0, i32 0} ; [ DW_TAG_array_type ]
!18 = metadata !{i32 786468, null, metadata !"float", null, i32 0, i64 32, i64 32, i64 0, i32 0, i32 4} ; [ DW_TAG_base_type ]
!19 = metadata !{metadata !20}
!20 = metadata !{i32 786465, i64 0, i64 8}        ; [ DW_TAG_subrange_type ]
!21 = metadata !{i32 786445, metadata !13, metadata !"acc", metadata !14, i32 7, i64 32, i64 32, i64 288, i32 1, metadata !18} ; [ DW_TAG_member ]
!22 = metadata !{i32 786478, i32 0, metadata !13, metadata !"macc", metadata !"macc", metadata !"_ZN18processing_element4maccEPfS0_RfS0_", metadata !14, i32 11, metadata !10, i1 false, i1 false, i32 0, i32 0, null, i32 256, i1 false, null, null, i32 0, metadata !23, i32 11} ; [ DW_TAG_subprogram ]
!23 = metadata !{metadata !24}
!24 = metadata !{i32 786468}                      ; [ DW_TAG_base_type ]
!25 = metadata !{i32 786447, null, metadata !"", null, i32 0, i64 64, i64 64, i64 0, i32 0, metadata !18} ; [ DW_TAG_pointer_type ]
!26 = metadata !{i32 786448, null, null, null, i32 0, i64 0, i64 0, i64 0, i32 0, metadata !18} ; [ DW_TAG_reference_type ]
!27 = metadata !{i32 3, i32 37, metadata !8, null}
!28 = metadata !{i32 786689, metadata !8, metadata !"filt_in", null, i32 3, metadata !17, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!29 = metadata !{i32 3, i32 61, metadata !8, null}
!30 = metadata !{i32 786689, metadata !8, metadata !"prod_out", null, i32 4, metadata !17, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!31 = metadata !{i32 4, i32 53, metadata !8, null}
!32 = metadata !{i32 5, i32 1, metadata !33, null}
!33 = metadata !{i32 786443, metadata !8, i32 4, i32 73, metadata !9, i32 0} ; [ DW_TAG_lexical_block ]
!34 = metadata !{i32 10, i32 5, metadata !35, null}
!35 = metadata !{i32 786443, metadata !36, i32 9, i32 38, metadata !9, i32 2} ; [ DW_TAG_lexical_block ]
!36 = metadata !{i32 786443, metadata !33, i32 9, i32 3, metadata !9, i32 1} ; [ DW_TAG_lexical_block ]
!37 = metadata !{i32 17, i32 5, metadata !38, null}
!38 = metadata !{i32 786443, metadata !39, i32 16, i32 38, metadata !9, i32 4} ; [ DW_TAG_lexical_block ]
!39 = metadata !{i32 786443, metadata !33, i32 16, i32 3, metadata !9, i32 3} ; [ DW_TAG_lexical_block ]
!40 = metadata !{i32 23, i32 5, metadata !41, null}
!41 = metadata !{i32 786443, metadata !42, i32 22, i32 38, metadata !9, i32 6} ; [ DW_TAG_lexical_block ]
!42 = metadata !{i32 786443, metadata !33, i32 22, i32 3, metadata !9, i32 5} ; [ DW_TAG_lexical_block ]
!43 = metadata !{metadata !44}
!44 = metadata !{i32 0, i32 31, metadata !45}
!45 = metadata !{metadata !46}
!46 = metadata !{metadata !"img_in", metadata !47, metadata !"float", i32 0, i32 31}
!47 = metadata !{metadata !48}
!48 = metadata !{i32 0, i32 8, i32 1}
!49 = metadata !{metadata !50}
!50 = metadata !{i32 0, i32 31, metadata !51}
!51 = metadata !{metadata !52}
!52 = metadata !{metadata !"filt_in", metadata !47, metadata !"float", i32 0, i32 31}
!53 = metadata !{metadata !54}
!54 = metadata !{i32 0, i32 31, metadata !55}
!55 = metadata !{metadata !56}
!56 = metadata !{metadata !"sum_out", metadata !5, metadata !"float", i32 0, i32 31}
!57 = metadata !{metadata !58}
!58 = metadata !{i32 0, i32 31, metadata !59}
!59 = metadata !{metadata !60}
!60 = metadata !{metadata !"prod_out", metadata !47, metadata !"float", i32 0, i32 31}
!61 = metadata !{i32 786689, metadata !62, metadata !"img_in", null, i32 29, metadata !17, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!62 = metadata !{i32 786478, i32 0, metadata !9, metadata !"do_macc", metadata !"do_macc", metadata !"_Z7do_maccPfS_RfS_", metadata !9, i32 29, metadata !63, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, null, null, null, metadata !23, i32 30} ; [ DW_TAG_subprogram ]
!63 = metadata !{i32 786453, i32 0, metadata !"", i32 0, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !64, i32 0, i32 0} ; [ DW_TAG_subroutine_type ]
!64 = metadata !{null, metadata !25, metadata !25, metadata !26, metadata !25}
!65 = metadata !{i32 29, i32 20, metadata !62, null}
!66 = metadata !{i32 786689, metadata !62, metadata !"filt_in", null, i32 29, metadata !17, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!67 = metadata !{i32 29, i32 44, metadata !62, null}
!68 = metadata !{i32 786689, metadata !62, metadata !"sum_out", metadata !9, i32 50331678, metadata !26, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!69 = metadata !{i32 30, i32 38, metadata !62, null}
!70 = metadata !{i32 786689, metadata !62, metadata !"prod_out", null, i32 30, metadata !17, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!71 = metadata !{i32 30, i32 53, metadata !62, null}
!72 = metadata !{i32 32, i32 2, metadata !73, null}
!73 = metadata !{i32 786443, metadata !62, i32 30, i32 73, metadata !9, i32 7} ; [ DW_TAG_lexical_block ]
!74 = metadata !{i32 33, i32 1, metadata !73, null}
