; ModuleID = '/home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/a.o.2.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@llvm.global_ctors.1 = appending global [1 x void ()*] [void ()* @_GLOBAL__I_a] ; [#uses=0 type=[1 x void ()*]*]
@llvm.global_ctors.0 = appending global [1 x i32] [i32 65535] ; [#uses=0 type=[1 x i32]*]
@do_macc.str = internal unnamed_addr constant [8 x i8] c"do_macc\00" ; [#uses=1 type=[8 x i8]*]
@.str = private unnamed_addr constant [1 x i8] zeroinitializer, align 1 ; [#uses=1 type=[1 x i8]*]

; [#uses=1]
define internal fastcc float @"processing_element::macc"([9 x float]* nocapture %img_in, [9 x float]* nocapture %filt_in, [9 x float]* nocapture %prod_out) {
  call void @llvm.dbg.value(metadata !{[9 x float]* %img_in}, i64 0, metadata !49), !dbg !50 ; [debug line = 3:37] [debug variable = img_in]
  call void @llvm.dbg.value(metadata !{[9 x float]* %filt_in}, i64 0, metadata !51), !dbg !52 ; [debug line = 3:61] [debug variable = filt_in]
  call void @llvm.dbg.value(metadata !{[9 x float]* %prod_out}, i64 0, metadata !53), !dbg !54 ; [debug line = 4:53] [debug variable = prod_out]
  call void (...)* @_ssdm_op_SpecPipeline(i32 1, i32 1, i32 1, i32 0, [1 x i8]* @.str) nounwind, !dbg !55 ; [debug line = 5:1]
  %img_in.addr = getelementptr [9 x float]* %img_in, i64 0, i64 0, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load = load float* %img_in.addr, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr = getelementptr [9 x float]* %filt_in, i64 0, i64 0, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load = load float* %filt_in.addr, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products = fmul float %img_in.load, %filt_in.load, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.1 = getelementptr [9 x float]* %img_in, i64 0, i64 1, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.1 = load float* %img_in.addr.1, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.1 = getelementptr [9 x float]* %filt_in, i64 0, i64 1, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.1 = load float* %filt_in.addr.1, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.1.assign.1 = fmul float %img_in.load.1, %filt_in.load.1, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.2 = getelementptr [9 x float]* %img_in, i64 0, i64 2, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.2 = load float* %img_in.addr.2, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.2 = getelementptr [9 x float]* %filt_in, i64 0, i64 2, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.2 = load float* %filt_in.addr.2, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.2.assign.1 = fmul float %img_in.load.2, %filt_in.load.2, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.3 = getelementptr [9 x float]* %img_in, i64 0, i64 3, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.3 = load float* %img_in.addr.3, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.3 = getelementptr [9 x float]* %filt_in, i64 0, i64 3, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.3 = load float* %filt_in.addr.3, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.3.assign.1 = fmul float %img_in.load.3, %filt_in.load.3, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.4 = getelementptr [9 x float]* %img_in, i64 0, i64 4, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.4 = load float* %img_in.addr.4, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.4 = getelementptr [9 x float]* %filt_in, i64 0, i64 4, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.4 = load float* %filt_in.addr.4, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.4.assign.1 = fmul float %img_in.load.4, %filt_in.load.4, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.5 = getelementptr [9 x float]* %img_in, i64 0, i64 5, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.5 = load float* %img_in.addr.5, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.5 = getelementptr [9 x float]* %filt_in, i64 0, i64 5, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.5 = load float* %filt_in.addr.5, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.5.assign.1 = fmul float %img_in.load.5, %filt_in.load.5, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.6 = getelementptr [9 x float]* %img_in, i64 0, i64 6, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.6 = load float* %img_in.addr.6, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.6 = getelementptr [9 x float]* %filt_in, i64 0, i64 6, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.6 = load float* %filt_in.addr.6, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.6.assign.1 = fmul float %img_in.load.6, %filt_in.load.6, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.7 = getelementptr [9 x float]* %img_in, i64 0, i64 7, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.7 = load float* %img_in.addr.7, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.7 = getelementptr [9 x float]* %filt_in, i64 0, i64 7, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.7 = load float* %filt_in.addr.7, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.7.assign.1 = fmul float %img_in.load.7, %filt_in.load.7, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %img_in.addr.8 = getelementptr [9 x float]* %img_in, i64 0, i64 8, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load.8 = load float* %img_in.addr.8, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %filt_in.addr.8 = getelementptr [9 x float]* %filt_in, i64 0, i64 8, !dbg !57 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load.8 = load float* %filt_in.addr.8, align 4, !dbg !57 ; [#uses=1 type=float] [debug line = 10:5]
  %processing_element.products.8.assign.1 = fmul float %img_in.load.8, %filt_in.load.8, !dbg !57 ; [#uses=2 type=float] [debug line = 10:5]
  %tmp. = fadd float %processing_element.products, 0.000000e+00, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.1 = fadd float %tmp., %processing_element.products.1.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.2 = fadd float %tmp.5.1, %processing_element.products.2.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.3 = fadd float %tmp.5.2, %processing_element.products.3.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.4 = fadd float %tmp.5.3, %processing_element.products.4.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.5 = fadd float %tmp.5.4, %processing_element.products.5.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.6 = fadd float %tmp.5.5, %processing_element.products.6.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.7 = fadd float %tmp.5.6, %processing_element.products.7.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %tmp.5.8 = fadd float %tmp.5.7, %processing_element.products.8.assign.1, !dbg !60 ; [#uses=1 type=float] [debug line = 17:5]
  %prod_out.addr = getelementptr [9 x float]* %prod_out, i64 0, i64 0, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products, float* %prod_out.addr, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.1 = getelementptr [9 x float]* %prod_out, i64 0, i64 1, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.1.assign.1, float* %prod_out.addr.1, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.2 = getelementptr [9 x float]* %prod_out, i64 0, i64 2, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.2.assign.1, float* %prod_out.addr.2, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.3 = getelementptr [9 x float]* %prod_out, i64 0, i64 3, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.3.assign.1, float* %prod_out.addr.3, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.4 = getelementptr [9 x float]* %prod_out, i64 0, i64 4, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.4.assign.1, float* %prod_out.addr.4, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.5 = getelementptr [9 x float]* %prod_out, i64 0, i64 5, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.5.assign.1, float* %prod_out.addr.5, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.6 = getelementptr [9 x float]* %prod_out, i64 0, i64 6, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.6.assign.1, float* %prod_out.addr.6, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.7 = getelementptr [9 x float]* %prod_out, i64 0, i64 7, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.7.assign.1, float* %prod_out.addr.7, align 4, !dbg !63 ; [debug line = 23:5]
  %prod_out.addr.8 = getelementptr [9 x float]* %prod_out, i64 0, i64 8, !dbg !63 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %processing_element.products.8.assign.1, float* %prod_out.addr.8, align 4, !dbg !63 ; [debug line = 23:5]
  ret float %tmp.5.8
}

; [#uses=7]
declare void @llvm.dbg.value(metadata, i64, metadata) nounwind readnone

; [#uses=0]
define void @do_macc([9 x float]* %img_in, [9 x float]* %filt_in, float* %sum_out, [9 x float]* %prod_out) nounwind uwtable {
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %img_in) nounwind, !map !66
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %filt_in) nounwind, !map !72
  call void (...)* @_ssdm_op_SpecBitsMap(float* %sum_out) nounwind, !map !76
  call void (...)* @_ssdm_op_SpecBitsMap([9 x float]* %prod_out) nounwind, !map !80
  call void (...)* @_ssdm_op_SpecTopModule([8 x i8]* @do_macc.str) nounwind
  call void @llvm.dbg.value(metadata !{[9 x float]* %img_in}, i64 0, metadata !84), !dbg !85 ; [debug line = 29:20] [debug variable = img_in]
  call void @llvm.dbg.value(metadata !{[9 x float]* %filt_in}, i64 0, metadata !86), !dbg !87 ; [debug line = 29:44] [debug variable = filt_in]
  call void @llvm.dbg.value(metadata !{float* %sum_out}, i64 0, metadata !88), !dbg !89 ; [debug line = 30:38] [debug variable = sum_out]
  call void @llvm.dbg.value(metadata !{[9 x float]* %prod_out}, i64 0, metadata !90), !dbg !91 ; [debug line = 30:53] [debug variable = prod_out]
  %call.ret = call fastcc float @"processing_element::macc"([9 x float]* %img_in, [9 x float]* %filt_in, [9 x float]* %prod_out) nounwind, !dbg !92 ; [#uses=1 type=float] [debug line = 32:2]
  store float %call.ret, float* %sum_out, align 4, !dbg !92 ; [debug line = 32:2]
  ret void, !dbg !94                              ; [debug line = 33:1]
}

; [#uses=1]
declare void @_ssdm_op_SpecTopModule(...)

; [#uses=1]
declare void @_ssdm_op_SpecPipeline(...) nounwind

; [#uses=4]
declare void @_ssdm_op_SpecBitsMap(...)

; [#uses=1]
declare void @_GLOBAL__I_a() nounwind section ".text.startup"

!llvm.dbg.cu = !{!0}
!hls.encrypted.func = !{}
!llvm.map.gv = !{!42}

!0 = metadata !{i32 786449, i32 0, i32 4, metadata !"/home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/processing_element.pragma.2.cpp", metadata !"/home/sem14h17/MA/ma_hls", metadata !"clang version 3.1 ", i1 true, i1 false, metadata !"", i32 0, metadata !1, metadata !1, metadata !3, metadata !35} ; [ DW_TAG_compile_unit ]
!1 = metadata !{metadata !2}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4}
!4 = metadata !{metadata !5, metadata !24, metadata !27, metadata !34}
!5 = metadata !{i32 786478, i32 0, null, metadata !"macc", metadata !"macc", metadata !"_ZN18processing_element4maccEPfS0_RfS0_", metadata !6, i32 3, metadata !7, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, null, null, metadata !19, metadata !20, i32 4} ; [ DW_TAG_subprogram ]
!6 = metadata !{i32 786473, metadata !"processing_element.cpp", metadata !"/home/sem14h17/MA/ma_hls", null} ; [ DW_TAG_file_type ]
!7 = metadata !{i32 786453, i32 0, metadata !"", i32 0, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !8, i32 0, i32 0} ; [ DW_TAG_subroutine_type ]
!8 = metadata !{null, metadata !9, metadata !22, metadata !22, metadata !23, metadata !22}
!9 = metadata !{i32 786447, i32 0, metadata !"", i32 0, i32 0, i64 64, i64 64, i64 0, i32 64, metadata !10} ; [ DW_TAG_pointer_type ]
!10 = metadata !{i32 786434, null, metadata !"processing_element", metadata !11, i32 3, i64 320, i64 32, i32 0, i32 0, null, metadata !12, i32 0, null, null} ; [ DW_TAG_class_type ]
!11 = metadata !{i32 786473, metadata !"./processing_element.hpp", metadata !"/home/sem14h17/MA/ma_hls", null} ; [ DW_TAG_file_type ]
!12 = metadata !{metadata !13, metadata !18, metadata !19}
!13 = metadata !{i32 786445, metadata !10, metadata !"products", metadata !11, i32 6, i64 288, i64 32, i64 0, i32 1, metadata !14} ; [ DW_TAG_member ]
!14 = metadata !{i32 786433, null, metadata !"", null, i32 0, i64 288, i64 32, i32 0, i32 0, metadata !15, metadata !16, i32 0, i32 0} ; [ DW_TAG_array_type ]
!15 = metadata !{i32 786468, null, metadata !"float", null, i32 0, i64 32, i64 32, i64 0, i32 0, i32 4} ; [ DW_TAG_base_type ]
!16 = metadata !{metadata !17}
!17 = metadata !{i32 786465, i64 0, i64 8}        ; [ DW_TAG_subrange_type ]
!18 = metadata !{i32 786445, metadata !10, metadata !"acc", metadata !11, i32 7, i64 32, i64 32, i64 288, i32 1, metadata !15} ; [ DW_TAG_member ]
!19 = metadata !{i32 786478, i32 0, metadata !10, metadata !"macc", metadata !"macc", metadata !"_ZN18processing_element4maccEPfS0_RfS0_", metadata !11, i32 11, metadata !7, i1 false, i1 false, i32 0, i32 0, null, i32 256, i1 false, null, null, i32 0, metadata !20, i32 11} ; [ DW_TAG_subprogram ]
!20 = metadata !{metadata !21}
!21 = metadata !{i32 786468}                      ; [ DW_TAG_base_type ]
!22 = metadata !{i32 786447, null, metadata !"", null, i32 0, i64 64, i64 64, i64 0, i32 0, metadata !15} ; [ DW_TAG_pointer_type ]
!23 = metadata !{i32 786448, null, null, null, i32 0, i64 0, i64 0, i64 0, i32 0, metadata !15} ; [ DW_TAG_reference_type ]
!24 = metadata !{i32 786478, i32 0, metadata !6, metadata !"do_macc", metadata !"do_macc", metadata !"_Z7do_maccPfS_RfS_", metadata !6, i32 29, metadata !25, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, null, null, null, metadata !20, i32 30} ; [ DW_TAG_subprogram ]
!25 = metadata !{i32 786453, i32 0, metadata !"", i32 0, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !26, i32 0, i32 0} ; [ DW_TAG_subroutine_type ]
!26 = metadata !{null, metadata !22, metadata !22, metadata !23, metadata !22}
!27 = metadata !{i32 786478, i32 0, null, metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !"_ZN51ssdm_global_array_processing_elementpp0cppaplinecppC1Ev", metadata !6, i32 37, metadata !28, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, null, null, metadata !33, metadata !20, i32 37} ; [ DW_TAG_subprogram ]
!28 = metadata !{i32 786453, i32 0, metadata !"", i32 0, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !29, i32 0, i32 0} ; [ DW_TAG_subroutine_type ]
!29 = metadata !{null, metadata !30}
!30 = metadata !{i32 786447, i32 0, metadata !"", i32 0, i32 0, i64 64, i64 64, i64 0, i32 64, metadata !31} ; [ DW_TAG_pointer_type ]
!31 = metadata !{i32 786434, null, metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !6, i32 35, i64 8, i64 8, i32 0, i32 0, null, metadata !32, i32 0, null, null} ; [ DW_TAG_class_type ]
!32 = metadata !{metadata !33}
!33 = metadata !{i32 786478, i32 0, metadata !31, metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !"", metadata !6, i32 37, metadata !28, i1 false, i1 false, i32 0, i32 0, null, i32 256, i1 false, null, null, i32 0, metadata !20, i32 37} ; [ DW_TAG_subprogram ]
!34 = metadata !{i32 786478, i32 0, null, metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !"ssdm_global_array_processing_elementpp0cppaplinecpp", metadata !"_ZN51ssdm_global_array_processing_elementpp0cppaplinecppC2Ev", metadata !6, i32 37, metadata !28, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, null, null, metadata !33, metadata !20, i32 37} ; [ DW_TAG_subprogram ]
!35 = metadata !{metadata !36}
!36 = metadata !{metadata !37, metadata !37, metadata !37, metadata !37, metadata !37, metadata !37, metadata !37, metadata !37, metadata !37, metadata !40, metadata !41}
!37 = metadata !{i32 786484, i32 0, metadata !11, metadata !"NUM_MULT", metadata !"NUM_MULT", metadata !"NUM_MULT", metadata !11, i32 1, metadata !38, i32 1, i32 1, i32 9} ; [ DW_TAG_variable ]
!38 = metadata !{i32 786470, null, metadata !"", null, i32 0, i64 0, i64 0, i64 0, i32 0, metadata !39} ; [ DW_TAG_const_type ]
!39 = metadata !{i32 786468, null, metadata !"int", null, i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ]
!40 = metadata !{i32 786484, i32 0, null, metadata !"ssdm_global_array_ins", metadata !"ssdm_global_array_ins", metadata !"_ZL21ssdm_global_array_ins", metadata !6, i32 41, metadata !31, i32 1, i32 1, null} ; [ DW_TAG_variable ]
!41 = metadata !{i32 786484, i32 0, null, metadata !"NUM_MULT", metadata !"NUM_MULT", metadata !"_ZL8NUM_MULT", metadata !11, i32 1, metadata !38, i32 1, i32 1, null} ; [ DW_TAG_variable ]
!42 = metadata !{metadata !43, [1 x i32]* @llvm.global_ctors.0}
!43 = metadata !{metadata !44}
!44 = metadata !{i32 0, i32 31, metadata !45}
!45 = metadata !{metadata !46}
!46 = metadata !{metadata !"llvm.global_ctors.0", metadata !47, metadata !"", i32 0, i32 31}
!47 = metadata !{metadata !48}
!48 = metadata !{i32 0, i32 0, i32 1}
!49 = metadata !{i32 786689, metadata !5, metadata !"img_in", null, i32 3, metadata !14, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!50 = metadata !{i32 3, i32 37, metadata !5, null}
!51 = metadata !{i32 786689, metadata !5, metadata !"filt_in", null, i32 3, metadata !14, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!52 = metadata !{i32 3, i32 61, metadata !5, null}
!53 = metadata !{i32 786689, metadata !5, metadata !"prod_out", null, i32 4, metadata !14, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!54 = metadata !{i32 4, i32 53, metadata !5, null}
!55 = metadata !{i32 5, i32 1, metadata !56, null}
!56 = metadata !{i32 786443, metadata !5, i32 4, i32 73, metadata !6, i32 0} ; [ DW_TAG_lexical_block ]
!57 = metadata !{i32 10, i32 5, metadata !58, null}
!58 = metadata !{i32 786443, metadata !59, i32 9, i32 38, metadata !6, i32 2} ; [ DW_TAG_lexical_block ]
!59 = metadata !{i32 786443, metadata !56, i32 9, i32 3, metadata !6, i32 1} ; [ DW_TAG_lexical_block ]
!60 = metadata !{i32 17, i32 5, metadata !61, null}
!61 = metadata !{i32 786443, metadata !62, i32 16, i32 38, metadata !6, i32 4} ; [ DW_TAG_lexical_block ]
!62 = metadata !{i32 786443, metadata !56, i32 16, i32 3, metadata !6, i32 3} ; [ DW_TAG_lexical_block ]
!63 = metadata !{i32 23, i32 5, metadata !64, null}
!64 = metadata !{i32 786443, metadata !65, i32 22, i32 38, metadata !6, i32 6} ; [ DW_TAG_lexical_block ]
!65 = metadata !{i32 786443, metadata !56, i32 22, i32 3, metadata !6, i32 5} ; [ DW_TAG_lexical_block ]
!66 = metadata !{metadata !67}
!67 = metadata !{i32 0, i32 31, metadata !68}
!68 = metadata !{metadata !69}
!69 = metadata !{metadata !"img_in", metadata !70, metadata !"float", i32 0, i32 31}
!70 = metadata !{metadata !71}
!71 = metadata !{i32 0, i32 8, i32 1}
!72 = metadata !{metadata !73}
!73 = metadata !{i32 0, i32 31, metadata !74}
!74 = metadata !{metadata !75}
!75 = metadata !{metadata !"filt_in", metadata !70, metadata !"float", i32 0, i32 31}
!76 = metadata !{metadata !77}
!77 = metadata !{i32 0, i32 31, metadata !78}
!78 = metadata !{metadata !79}
!79 = metadata !{metadata !"sum_out", metadata !47, metadata !"float", i32 0, i32 31}
!80 = metadata !{metadata !81}
!81 = metadata !{i32 0, i32 31, metadata !82}
!82 = metadata !{metadata !83}
!83 = metadata !{metadata !"prod_out", metadata !70, metadata !"float", i32 0, i32 31}
!84 = metadata !{i32 786689, metadata !24, metadata !"img_in", null, i32 29, metadata !14, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!85 = metadata !{i32 29, i32 20, metadata !24, null}
!86 = metadata !{i32 786689, metadata !24, metadata !"filt_in", null, i32 29, metadata !14, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!87 = metadata !{i32 29, i32 44, metadata !24, null}
!88 = metadata !{i32 786689, metadata !24, metadata !"sum_out", metadata !6, i32 50331678, metadata !23, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!89 = metadata !{i32 30, i32 38, metadata !24, null}
!90 = metadata !{i32 786689, metadata !24, metadata !"prod_out", null, i32 30, metadata !14, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!91 = metadata !{i32 30, i32 53, metadata !24, null}
!92 = metadata !{i32 32, i32 2, metadata !93, null}
!93 = metadata !{i32 786443, metadata !24, i32 30, i32 73, metadata !6, i32 7} ; [ DW_TAG_lexical_block ]
!94 = metadata !{i32 33, i32 1, metadata !93, null}
