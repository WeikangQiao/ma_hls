; ModuleID = '/home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/a.g.1.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%class.processing_element.1 = type { [9 x float], float }

@llvm.global_ctors = appending global [1 x { i32, void ()* }] [{ i32, void ()* } { i32 65535, void ()* @_GLOBAL__I_a }] ; [#uses=0 type=[1 x { i32, void ()* }]*]
@do_macc.str = internal unnamed_addr constant [8 x i8] c"do_macc\00" ; [#uses=1 type=[8 x i8]*]
@.str3 = private unnamed_addr constant [9 x i8] c"L_assign\00", align 1 ; [#uses=1 type=[9 x i8]*]
@.str2 = private unnamed_addr constant [13 x i8] c"L_accumulate\00", align 1 ; [#uses=1 type=[13 x i8]*]
@.str1 = private unnamed_addr constant [11 x i8] c"L_multiply\00", align 1 ; [#uses=1 type=[11 x i8]*]
@.str = private unnamed_addr constant [1 x i8] zeroinitializer, align 1 ; [#uses=1 type=[1 x i8]*]

; [#uses=1]
define internal fastcc void @"processing_element::macc"(%class.processing_element.1* %this, float* %img_in, float* %filt_in, float* %sum_out, float* %prod_out) nounwind uwtable align 2 {
  call void @llvm.dbg.value(metadata !{%class.processing_element.1* %this}, i64 0, metadata !42), !dbg !44 ; [debug line = 3:26] [debug variable = this]
  call void @llvm.dbg.value(metadata !{float* %img_in}, i64 0, metadata !45), !dbg !46 ; [debug line = 3:37] [debug variable = img_in]
  call void @llvm.dbg.value(metadata !{float* %filt_in}, i64 0, metadata !47), !dbg !48 ; [debug line = 3:61] [debug variable = filt_in]
  call void @llvm.dbg.value(metadata !{float* %sum_out}, i64 0, metadata !49), !dbg !50 ; [debug line = 4:38] [debug variable = sum_out]
  call void @llvm.dbg.value(metadata !{float* %prod_out}, i64 0, metadata !51), !dbg !52 ; [debug line = 4:53] [debug variable = prod_out]
  call void (...)* @_ssdm_SpecArrayDimSize(float* %prod_out, i32 9) nounwind, !dbg !53 ; [debug line = 4:74]
  call void (...)* @_ssdm_SpecArrayDimSize(float* %filt_in, i32 9) nounwind, !dbg !55 ; [debug line = 4:116]
  call void (...)* @_ssdm_SpecArrayDimSize(float* %img_in, i32 9) nounwind, !dbg !56 ; [debug line = 4:157]
  call void (...)* @_ssdm_op_SpecPipeline(i32 1, i32 1, i32 1, i32 0, i8* getelementptr inbounds ([1 x i8]* @.str, i64 0, i64 0)) nounwind, !dbg !57 ; [debug line = 5:1]
  call void (...)* @_ssdm_Unroll(i32 0, i32 0, i32 0, i8* getelementptr inbounds ([1 x i8]* @.str, i64 0, i64 0)) nounwind, !dbg !58 ; [debug line = 7:1]
  br label %1, !dbg !59                           ; [debug line = 9:17]

; <label>:1                                       ; preds = %2, %0
  %i = phi i32 [ 0, %0 ], [ %i.1, %2 ]            ; [#uses=3 type=i32]
  %exitcond2 = icmp eq i32 %i, 9, !dbg !59        ; [#uses=1 type=i1] [debug line = 9:17]
  br i1 %exitcond2, label %3, label %2, !dbg !59  ; [debug line = 9:17]

; <label>:2                                       ; preds = %1
  call void (...)* @_ssdm_op_SpecLoopName(i8* getelementptr inbounds ([11 x i8]* @.str1, i64 0, i64 0)) nounwind, !dbg !61 ; [debug line = 9:39]
  %rbegin5 = call i32 (...)* @_ssdm_op_SpecRegionBegin(i8* getelementptr inbounds ([11 x i8]* @.str1, i64 0, i64 0)) nounwind, !dbg !61 ; [#uses=1 type=i32] [debug line = 9:39]
  %tmp = sext i32 %i to i64, !dbg !63             ; [#uses=3 type=i64] [debug line = 10:5]
  %img_in.addr = getelementptr inbounds float* %img_in, i64 %tmp, !dbg !63 ; [#uses=1 type=float*] [debug line = 10:5]
  %img_in.load = load float* %img_in.addr, align 4, !dbg !63 ; [#uses=2 type=float] [debug line = 10:5]
  call void (...)* @_ssdm_SpecKeepArrayLoad(float %img_in.load) nounwind
  %filt_in.addr = getelementptr inbounds float* %filt_in, i64 %tmp, !dbg !63 ; [#uses=1 type=float*] [debug line = 10:5]
  %filt_in.load = load float* %filt_in.addr, align 4, !dbg !63 ; [#uses=2 type=float] [debug line = 10:5]
  call void (...)* @_ssdm_SpecKeepArrayLoad(float %filt_in.load) nounwind
  %tmp.1 = fmul float %img_in.load, %filt_in.load, !dbg !63 ; [#uses=1 type=float] [debug line = 10:5]
  %this.addr.1 = getelementptr inbounds %class.processing_element.1* %this, i64 0, i32 0, i64 %tmp, !dbg !63 ; [#uses=1 type=float*] [debug line = 10:5]
  store float %tmp.1, float* %this.addr.1, align 4, !dbg !63 ; [debug line = 10:5]
  %rend6 = call i32 (...)* @_ssdm_op_SpecRegionEnd(i8* getelementptr inbounds ([11 x i8]* @.str1, i64 0, i64 0), i32 %rbegin5) nounwind, !dbg !64 ; [#uses=0 type=i32] [debug line = 11:3]
  %i.1 = add nsw i32 %i, 1, !dbg !65              ; [#uses=1 type=i32] [debug line = 9:33]
  call void @llvm.dbg.value(metadata !{i32 %i.1}, i64 0, metadata !66), !dbg !65 ; [debug line = 9:33] [debug variable = i]
  br label %1, !dbg !65                           ; [debug line = 9:33]

; <label>:3                                       ; preds = %1
  %this.addr = getelementptr inbounds %class.processing_element.1* %this, i64 0, i32 1, !dbg !67 ; [#uses=3 type=float*] [debug line = 13:3]
  store float 0.000000e+00, float* %this.addr, align 4, !dbg !67 ; [debug line = 13:3]
  call void (...)* @_ssdm_Unroll(i32 0, i32 0, i32 0, i8* getelementptr inbounds ([1 x i8]* @.str, i64 0, i64 0)) nounwind, !dbg !68 ; [debug line = 14:1]
  br label %4, !dbg !69                           ; [debug line = 16:17]

; <label>:4                                       ; preds = %5, %3
  %tmp.3 = phi float [ 0.000000e+00, %3 ], [ %tmp.5, %5 ] ; [#uses=2 type=float]
  %i1 = phi i32 [ 0, %3 ], [ %i.2, %5 ]           ; [#uses=3 type=i32]
  %exitcond1 = icmp eq i32 %i1, 9, !dbg !69       ; [#uses=1 type=i1] [debug line = 16:17]
  br i1 %exitcond1, label %6, label %5, !dbg !69  ; [debug line = 16:17]

; <label>:5                                       ; preds = %4
  call void (...)* @_ssdm_op_SpecLoopName(i8* getelementptr inbounds ([13 x i8]* @.str2, i64 0, i64 0)) nounwind, !dbg !71 ; [debug line = 16:39]
  %rbegin = call i32 (...)* @_ssdm_op_SpecRegionBegin(i8* getelementptr inbounds ([13 x i8]* @.str2, i64 0, i64 0)) nounwind, !dbg !71 ; [#uses=1 type=i32] [debug line = 16:39]
  %tmp.4 = sext i32 %i1 to i64, !dbg !73          ; [#uses=1 type=i64] [debug line = 17:5]
  %this.addr.2 = getelementptr inbounds %class.processing_element.1* %this, i64 0, i32 0, i64 %tmp.4, !dbg !73 ; [#uses=1 type=float*] [debug line = 17:5]
  %this.load = load float* %this.addr.2, align 4, !dbg !73 ; [#uses=2 type=float] [debug line = 17:5]
  call void (...)* @_ssdm_SpecKeepArrayLoad(float %this.load) nounwind
  call void (...)* @_ssdm_SpecKeepArrayLoad(float %tmp.3) nounwind
  %tmp.5 = fadd float %tmp.3, %this.load, !dbg !73 ; [#uses=2 type=float] [debug line = 17:5]
  store float %tmp.5, float* %this.addr, align 4, !dbg !73 ; [debug line = 17:5]
  %rend = call i32 (...)* @_ssdm_op_SpecRegionEnd(i8* getelementptr inbounds ([13 x i8]* @.str2, i64 0, i64 0), i32 %rbegin) nounwind, !dbg !74 ; [#uses=0 type=i32] [debug line = 18:3]
  %i.2 = add nsw i32 %i1, 1, !dbg !75             ; [#uses=1 type=i32] [debug line = 16:33]
  call void @llvm.dbg.value(metadata !{i32 %i.2}, i64 0, metadata !76), !dbg !75 ; [debug line = 16:33] [debug variable = i]
  br label %4, !dbg !75                           ; [debug line = 16:33]

; <label>:6                                       ; preds = %4
  call void (...)* @_ssdm_Unroll(i32 0, i32 0, i32 0, i8* getelementptr inbounds ([1 x i8]* @.str, i64 0, i64 0)) nounwind, !dbg !77 ; [debug line = 20:1]
  br label %7, !dbg !78                           ; [debug line = 22:17]

; <label>:7                                       ; preds = %8, %6
  %i2 = phi i32 [ 0, %6 ], [ %i.3, %8 ]           ; [#uses=3 type=i32]
  %exitcond = icmp eq i32 %i2, 9, !dbg !78        ; [#uses=1 type=i1] [debug line = 22:17]
  br i1 %exitcond, label %9, label %8, !dbg !78   ; [debug line = 22:17]

; <label>:8                                       ; preds = %7
  call void (...)* @_ssdm_op_SpecLoopName(i8* getelementptr inbounds ([9 x i8]* @.str3, i64 0, i64 0)) nounwind, !dbg !80 ; [debug line = 22:39]
  %rbegin3 = call i32 (...)* @_ssdm_op_SpecRegionBegin(i8* getelementptr inbounds ([9 x i8]* @.str3, i64 0, i64 0)) nounwind, !dbg !80 ; [#uses=1 type=i32] [debug line = 22:39]
  %tmp.7 = sext i32 %i2 to i64, !dbg !82          ; [#uses=2 type=i64] [debug line = 23:5]
  %this.addr.3 = getelementptr inbounds %class.processing_element.1* %this, i64 0, i32 0, i64 %tmp.7, !dbg !82 ; [#uses=1 type=float*] [debug line = 23:5]
  %this.load.2 = load float* %this.addr.3, align 4, !dbg !82 ; [#uses=2 type=float] [debug line = 23:5]
  call void (...)* @_ssdm_SpecKeepArrayLoad(float %this.load.2) nounwind
  %prod_out.addr = getelementptr inbounds float* %prod_out, i64 %tmp.7, !dbg !82 ; [#uses=1 type=float*] [debug line = 23:5]
  store float %this.load.2, float* %prod_out.addr, align 4, !dbg !82 ; [debug line = 23:5]
  %rend4 = call i32 (...)* @_ssdm_op_SpecRegionEnd(i8* getelementptr inbounds ([9 x i8]* @.str3, i64 0, i64 0), i32 %rbegin3) nounwind, !dbg !83 ; [#uses=0 type=i32] [debug line = 24:3]
  %i.3 = add nsw i32 %i2, 1, !dbg !84             ; [#uses=1 type=i32] [debug line = 22:33]
  call void @llvm.dbg.value(metadata !{i32 %i.3}, i64 0, metadata !85), !dbg !84 ; [debug line = 22:33] [debug variable = i]
  br label %7, !dbg !84                           ; [debug line = 22:33]

; <label>:9                                       ; preds = %7
  %this.load.1 = load float* %this.addr, align 4, !dbg !86 ; [#uses=2 type=float] [debug line = 25:3]
  call void (...)* @_ssdm_SpecKeepArrayLoad(float %this.load.1) nounwind
  store float %this.load.1, float* %sum_out, align 4, !dbg !86 ; [debug line = 25:3]
  ret void, !dbg !87                              ; [debug line = 26:1]
}

; [#uses=12]
declare void @llvm.dbg.value(metadata, i64, metadata) nounwind readnone

; [#uses=1]
declare void @llvm.dbg.declare(metadata, metadata) nounwind readnone

; [#uses=0]
define void @do_macc(float* %img_in, float* %filt_in, float* %sum_out, float* %prod_out) nounwind uwtable {
  call void (...)* @_ssdm_op_SpecTopModule([8 x i8]* @do_macc.str) nounwind
  %PE = alloca %class.processing_element.1, align 4 ; [#uses=1 type=%class.processing_element.1*]
  call void @llvm.dbg.value(metadata !{float* %img_in}, i64 0, metadata !88), !dbg !89 ; [debug line = 29:20] [debug variable = img_in]
  call void @llvm.dbg.value(metadata !{float* %filt_in}, i64 0, metadata !90), !dbg !91 ; [debug line = 29:44] [debug variable = filt_in]
  call void @llvm.dbg.value(metadata !{float* %sum_out}, i64 0, metadata !92), !dbg !93 ; [debug line = 30:38] [debug variable = sum_out]
  call void @llvm.dbg.value(metadata !{float* %prod_out}, i64 0, metadata !94), !dbg !95 ; [debug line = 30:53] [debug variable = prod_out]
  call void (...)* @_ssdm_SpecArrayDimSize(float* %prod_out, i32 9) nounwind, !dbg !96 ; [debug line = 30:74]
  call void (...)* @_ssdm_SpecArrayDimSize(float* %filt_in, i32 9) nounwind, !dbg !98 ; [debug line = 30:116]
  call void (...)* @_ssdm_SpecArrayDimSize(float* %img_in, i32 9) nounwind, !dbg !99 ; [debug line = 30:157]
  call void @llvm.dbg.declare(metadata !{%class.processing_element.1* %PE}, metadata !100), !dbg !101 ; [debug line = 31:21] [debug variable = PE]
  call fastcc void @"processing_element::macc"(%class.processing_element.1* %PE, float* %img_in, float* %filt_in, float* %sum_out, float* %prod_out), !dbg !102 ; [debug line = 32:2]
  ret void, !dbg !103                             ; [debug line = 33:1]
}

; [#uses=1]
declare void @_ssdm_op_SpecTopModule(...)

; [#uses=3]
declare i32 @_ssdm_op_SpecRegionEnd(...)

; [#uses=3]
declare i32 @_ssdm_op_SpecRegionBegin(...)

; [#uses=1]
declare void @_ssdm_op_SpecPipeline(...) nounwind

; [#uses=3]
declare void @_ssdm_op_SpecLoopName(...) nounwind

; [#uses=3]
declare void @_ssdm_Unroll(...) nounwind

; [#uses=6]
declare void @_ssdm_SpecKeepArrayLoad(...)

; [#uses=6]
declare void @_ssdm_SpecArrayDimSize(...) nounwind

; [#uses=1]
declare void @_GLOBAL__I_a() nounwind section ".text.startup"

!llvm.dbg.cu = !{!0}
!hls.encrypted.func = !{}

!0 = metadata !{i32 786449, i32 0, i32 4, metadata !"/home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/processing_element.pragma.2.cpp", metadata !"/home/sem14h17/MA/ma_hls", metadata !"clang version 3.1 ", i1 true, i1 false, metadata !"", i32 0, metadata !1, metadata !1, metadata !3, metadata !35} ; [ DW_TAG_compile_unit ]
!1 = metadata !{metadata !2}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4}
!4 = metadata !{metadata !5, metadata !24, metadata !27, metadata !34}
!5 = metadata !{i32 786478, i32 0, null, metadata !"macc", metadata !"macc", metadata !"_ZN18processing_element4maccEPfS0_RfS0_", metadata !6, i32 3, metadata !7, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, void (%class.processing_element.1*, float*, float*, float*, float*)* @"processing_element::macc", null, metadata !19, metadata !20, i32 4} ; [ DW_TAG_subprogram ]
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
!24 = metadata !{i32 786478, i32 0, metadata !6, metadata !"do_macc", metadata !"do_macc", metadata !"_Z7do_maccPfS_RfS_", metadata !6, i32 29, metadata !25, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, void (float*, float*, float*, float*)* @do_macc, null, null, metadata !20, i32 30} ; [ DW_TAG_subprogram ]
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
!42 = metadata !{i32 786689, metadata !5, metadata !"this", metadata !6, i32 16777219, metadata !43, i32 64, i32 0} ; [ DW_TAG_arg_variable ]
!43 = metadata !{i32 786447, null, metadata !"", null, i32 0, i64 64, i64 64, i64 0, i32 0, metadata !10} ; [ DW_TAG_pointer_type ]
!44 = metadata !{i32 3, i32 26, metadata !5, null}
!45 = metadata !{i32 786689, metadata !5, metadata !"img_in", metadata !6, i32 33554435, metadata !22, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!46 = metadata !{i32 3, i32 37, metadata !5, null}
!47 = metadata !{i32 786689, metadata !5, metadata !"filt_in", metadata !6, i32 50331651, metadata !22, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!48 = metadata !{i32 3, i32 61, metadata !5, null}
!49 = metadata !{i32 786689, metadata !5, metadata !"sum_out", metadata !6, i32 67108868, metadata !23, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!50 = metadata !{i32 4, i32 38, metadata !5, null}
!51 = metadata !{i32 786689, metadata !5, metadata !"prod_out", metadata !6, i32 83886084, metadata !22, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!52 = metadata !{i32 4, i32 53, metadata !5, null}
!53 = metadata !{i32 4, i32 74, metadata !54, null}
!54 = metadata !{i32 786443, metadata !5, i32 4, i32 73, metadata !6, i32 0} ; [ DW_TAG_lexical_block ]
!55 = metadata !{i32 4, i32 116, metadata !54, null}
!56 = metadata !{i32 4, i32 157, metadata !54, null}
!57 = metadata !{i32 5, i32 1, metadata !54, null}
!58 = metadata !{i32 7, i32 1, metadata !54, null}
!59 = metadata !{i32 9, i32 17, metadata !60, null}
!60 = metadata !{i32 786443, metadata !54, i32 9, i32 3, metadata !6, i32 1} ; [ DW_TAG_lexical_block ]
!61 = metadata !{i32 9, i32 39, metadata !62, null}
!62 = metadata !{i32 786443, metadata !60, i32 9, i32 38, metadata !6, i32 2} ; [ DW_TAG_lexical_block ]
!63 = metadata !{i32 10, i32 5, metadata !62, null}
!64 = metadata !{i32 11, i32 3, metadata !62, null}
!65 = metadata !{i32 9, i32 33, metadata !60, null}
!66 = metadata !{i32 786688, metadata !60, metadata !"i", metadata !6, i32 9, metadata !39, i32 0, i32 0} ; [ DW_TAG_auto_variable ]
!67 = metadata !{i32 13, i32 3, metadata !54, null}
!68 = metadata !{i32 14, i32 1, metadata !54, null}
!69 = metadata !{i32 16, i32 17, metadata !70, null}
!70 = metadata !{i32 786443, metadata !54, i32 16, i32 3, metadata !6, i32 3} ; [ DW_TAG_lexical_block ]
!71 = metadata !{i32 16, i32 39, metadata !72, null}
!72 = metadata !{i32 786443, metadata !70, i32 16, i32 38, metadata !6, i32 4} ; [ DW_TAG_lexical_block ]
!73 = metadata !{i32 17, i32 5, metadata !72, null}
!74 = metadata !{i32 18, i32 3, metadata !72, null}
!75 = metadata !{i32 16, i32 33, metadata !70, null}
!76 = metadata !{i32 786688, metadata !70, metadata !"i", metadata !6, i32 16, metadata !39, i32 0, i32 0} ; [ DW_TAG_auto_variable ]
!77 = metadata !{i32 20, i32 1, metadata !54, null}
!78 = metadata !{i32 22, i32 17, metadata !79, null}
!79 = metadata !{i32 786443, metadata !54, i32 22, i32 3, metadata !6, i32 5} ; [ DW_TAG_lexical_block ]
!80 = metadata !{i32 22, i32 39, metadata !81, null}
!81 = metadata !{i32 786443, metadata !79, i32 22, i32 38, metadata !6, i32 6} ; [ DW_TAG_lexical_block ]
!82 = metadata !{i32 23, i32 5, metadata !81, null}
!83 = metadata !{i32 24, i32 3, metadata !81, null}
!84 = metadata !{i32 22, i32 33, metadata !79, null}
!85 = metadata !{i32 786688, metadata !79, metadata !"i", metadata !6, i32 22, metadata !39, i32 0, i32 0} ; [ DW_TAG_auto_variable ]
!86 = metadata !{i32 25, i32 3, metadata !54, null}
!87 = metadata !{i32 26, i32 1, metadata !54, null}
!88 = metadata !{i32 786689, metadata !24, metadata !"img_in", metadata !6, i32 16777245, metadata !22, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!89 = metadata !{i32 29, i32 20, metadata !24, null}
!90 = metadata !{i32 786689, metadata !24, metadata !"filt_in", metadata !6, i32 33554461, metadata !22, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!91 = metadata !{i32 29, i32 44, metadata !24, null}
!92 = metadata !{i32 786689, metadata !24, metadata !"sum_out", metadata !6, i32 50331678, metadata !23, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!93 = metadata !{i32 30, i32 38, metadata !24, null}
!94 = metadata !{i32 786689, metadata !24, metadata !"prod_out", metadata !6, i32 67108894, metadata !22, i32 0, i32 0} ; [ DW_TAG_arg_variable ]
!95 = metadata !{i32 30, i32 53, metadata !24, null}
!96 = metadata !{i32 30, i32 74, metadata !97, null}
!97 = metadata !{i32 786443, metadata !24, i32 30, i32 73, metadata !6, i32 7} ; [ DW_TAG_lexical_block ]
!98 = metadata !{i32 30, i32 116, metadata !97, null}
!99 = metadata !{i32 30, i32 157, metadata !97, null}
!100 = metadata !{i32 786688, metadata !97, metadata !"PE", metadata !6, i32 31, metadata !10, i32 0, i32 0} ; [ DW_TAG_auto_variable ]
!101 = metadata !{i32 31, i32 21, metadata !97, null}
!102 = metadata !{i32 32, i32 2, metadata !97, null}
!103 = metadata !{i32 33, i32 1, metadata !97, null}
