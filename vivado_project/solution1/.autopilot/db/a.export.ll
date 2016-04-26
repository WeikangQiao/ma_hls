; ModuleID = '/home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/a.o.2.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@llvm_global_ctors_1 = appending global [1 x void ()*] [void ()* @_GLOBAL__I_a]
@llvm_global_ctors_0 = appending global [1 x i32] [i32 65535]
@do_macc_str = internal unnamed_addr constant [8 x i8] c"do_macc\00"
@p_str = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

define internal fastcc float @do_macc_macc([9 x float]* nocapture %img_in, [9 x float]* nocapture %filt_in, [9 x float]* nocapture %prod_out) {
  call void (...)* @_ssdm_op_SpecPipeline(i32 1, i32 1, i32 1, i32 0, [1 x i8]* @p_str) nounwind
  %img_in_addr = getelementptr [9 x float]* %img_in, i64 0, i64 0
  %img_in_load = load float* %img_in_addr, align 4
  %filt_in_addr = getelementptr [9 x float]* %filt_in, i64 0, i64 0
  %filt_in_load = load float* %filt_in_addr, align 4
  %processing_element_products = fmul float %img_in_load, %filt_in_load
  %img_in_addr_1 = getelementptr [9 x float]* %img_in, i64 0, i64 1
  %img_in_load_1 = load float* %img_in_addr_1, align 4
  %filt_in_addr_1 = getelementptr [9 x float]* %filt_in, i64 0, i64 1
  %filt_in_load_1 = load float* %filt_in_addr_1, align 4
  %processing_element_products_1_s = fmul float %img_in_load_1, %filt_in_load_1
  %img_in_addr_2 = getelementptr [9 x float]* %img_in, i64 0, i64 2
  %img_in_load_2 = load float* %img_in_addr_2, align 4
  %filt_in_addr_2 = getelementptr [9 x float]* %filt_in, i64 0, i64 2
  %filt_in_load_2 = load float* %filt_in_addr_2, align 4
  %processing_element_products_2_s = fmul float %img_in_load_2, %filt_in_load_2
  %img_in_addr_3 = getelementptr [9 x float]* %img_in, i64 0, i64 3
  %img_in_load_3 = load float* %img_in_addr_3, align 4
  %filt_in_addr_3 = getelementptr [9 x float]* %filt_in, i64 0, i64 3
  %filt_in_load_3 = load float* %filt_in_addr_3, align 4
  %processing_element_products_3_s = fmul float %img_in_load_3, %filt_in_load_3
  %img_in_addr_4 = getelementptr [9 x float]* %img_in, i64 0, i64 4
  %img_in_load_4 = load float* %img_in_addr_4, align 4
  %filt_in_addr_4 = getelementptr [9 x float]* %filt_in, i64 0, i64 4
  %filt_in_load_4 = load float* %filt_in_addr_4, align 4
  %processing_element_products_4_s = fmul float %img_in_load_4, %filt_in_load_4
  %img_in_addr_5 = getelementptr [9 x float]* %img_in, i64 0, i64 5
  %img_in_load_5 = load float* %img_in_addr_5, align 4
  %filt_in_addr_5 = getelementptr [9 x float]* %filt_in, i64 0, i64 5
  %filt_in_load_5 = load float* %filt_in_addr_5, align 4
  %processing_element_products_5_s = fmul float %img_in_load_5, %filt_in_load_5
  %img_in_addr_6 = getelementptr [9 x float]* %img_in, i64 0, i64 6
  %img_in_load_6 = load float* %img_in_addr_6, align 4
  %filt_in_addr_6 = getelementptr [9 x float]* %filt_in, i64 0, i64 6
  %filt_in_load_6 = load float* %filt_in_addr_6, align 4
  %processing_element_products_6_s = fmul float %img_in_load_6, %filt_in_load_6
  %img_in_addr_7 = getelementptr [9 x float]* %img_in, i64 0, i64 7
  %img_in_load_7 = load float* %img_in_addr_7, align 4
  %filt_in_addr_7 = getelementptr [9 x float]* %filt_in, i64 0, i64 7
  %filt_in_load_7 = load float* %filt_in_addr_7, align 4
  %processing_element_products_7_s = fmul float %img_in_load_7, %filt_in_load_7
  %img_in_addr_8 = getelementptr [9 x float]* %img_in, i64 0, i64 8
  %img_in_load_8 = load float* %img_in_addr_8, align 4
  %filt_in_addr_8 = getelementptr [9 x float]* %filt_in, i64 0, i64 8
  %filt_in_load_8 = load float* %filt_in_addr_8, align 4
  %processing_element_products_8_s = fmul float %img_in_load_8, %filt_in_load_8
  %tmp_s = fadd float %processing_element_products, 0.000000e+00
  %tmp_5_1 = fadd float %tmp_s, %processing_element_products_1_s
  %tmp_5_2 = fadd float %tmp_5_1, %processing_element_products_2_s
  %tmp_5_3 = fadd float %tmp_5_2, %processing_element_products_3_s
  %tmp_5_4 = fadd float %tmp_5_3, %processing_element_products_4_s
  %tmp_5_5 = fadd float %tmp_5_4, %processing_element_products_5_s
  %tmp_5_6 = fadd float %tmp_5_5, %processing_element_products_6_s
  %tmp_5_7 = fadd float %tmp_5_6, %processing_element_products_7_s
  %tmp_5_8 = fadd float %tmp_5_7, %processing_element_products_8_s
  %prod_out_addr = getelementptr [9 x float]* %prod_out, i64 0, i64 0
  store float %processing_element_products, float* %prod_out_addr, align 4
  %prod_out_addr_1 = getelementptr [9 x float]* %prod_out, i64 0, i64 1
  store float %processing_element_products_1_s, float* %prod_out_addr_1, align 4
  %prod_out_addr_2 = getelementptr [9 x float]* %prod_out, i64 0, i64 2
  store float %processing_element_products_2_s, float* %prod_out_addr_2, align 4
  %prod_out_addr_3 = getelementptr [9 x float]* %prod_out, i64 0, i64 3
  store float %processing_element_products_3_s, float* %prod_out_addr_3, align 4
  %prod_out_addr_4 = getelementptr [9 x float]* %prod_out, i64 0, i64 4
  store float %processing_element_products_4_s, float* %prod_out_addr_4, align 4
  %prod_out_addr_5 = getelementptr [9 x float]* %prod_out, i64 0, i64 5
  store float %processing_element_products_5_s, float* %prod_out_addr_5, align 4
  %prod_out_addr_6 = getelementptr [9 x float]* %prod_out, i64 0, i64 6
  store float %processing_element_products_6_s, float* %prod_out_addr_6, align 4
  %prod_out_addr_7 = getelementptr [9 x float]* %prod_out, i64 0, i64 7
  store float %processing_element_products_7_s, float* %prod_out_addr_7, align 4
  %prod_out_addr_8 = getelementptr [9 x float]* %prod_out, i64 0, i64 8
  store float %processing_element_products_8_s, float* %prod_out_addr_8, align 4
  ret float %tmp_5_8
}

declare void @llvm.dbg.value(metadata, i64, metadata) nounwind readnone

define void @do_macc([9 x float]* %img_in, [9 x float]* %filt_in, float* %sum_out, [9 x float]* %prod_out) nounwind uwtable {
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %img_in) nounwind, !map !7
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %filt_in) nounwind, !map !13
  call void (...)* @_ssdm_op_SpecBitsMap(float* %sum_out) nounwind, !map !17
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %prod_out) nounwind, !map !21
  call void (...)* @_ssdm_op_SpecTopModule([8 x i8]* @do_macc_str) nounwind
  %call_ret = call fastcc float @do_macc_macc([9 x float]* %img_in, [9 x float]* %filt_in, [9 x float]* %prod_out) nounwind
  call void @_ssdm_op_Write.ap_auto.floatP(float* %sum_out, float %call_ret) nounwind
  ret void
}

define weak void @_ssdm_op_Write.ap_auto.floatP(float*, float) {
entry:
  store float %1, float* %0
  ret void
}

define weak void @_ssdm_op_SpecTopModule(...) {
entry:
  ret void
}

define weak void @_ssdm_op_SpecPipeline(...) nounwind {
entry:
  ret void
}

define weak void @_ssdm_op_SpecBitsMap(...) {
entry:
  ret void
}

declare i16 @_ssdm_op_HSub(...)

declare i16 @_ssdm_op_HMul(...)

declare i16 @_ssdm_op_HDiv(...)

declare i16 @_ssdm_op_HAdd(...)

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
!7 = metadata !{metadata !8}
!8 = metadata !{i32 0, i32 31, metadata !9}
!9 = metadata !{metadata !10}
!10 = metadata !{metadata !"img_in", metadata !11, metadata !"float", i32 0, i32 31}
!11 = metadata !{metadata !12}
!12 = metadata !{i32 0, i32 8, i32 1}
!13 = metadata !{metadata !14}
!14 = metadata !{i32 0, i32 31, metadata !15}
!15 = metadata !{metadata !16}
!16 = metadata !{metadata !"filt_in", metadata !11, metadata !"float", i32 0, i32 31}
!17 = metadata !{metadata !18}
!18 = metadata !{i32 0, i32 31, metadata !19}
!19 = metadata !{metadata !20}
!20 = metadata !{metadata !"sum_out", metadata !5, metadata !"float", i32 0, i32 31}
!21 = metadata !{metadata !22}
!22 = metadata !{i32 0, i32 31, metadata !23}
!23 = metadata !{metadata !24}
!24 = metadata !{metadata !"prod_out", metadata !11, metadata !"float", i32 0, i32 31}
