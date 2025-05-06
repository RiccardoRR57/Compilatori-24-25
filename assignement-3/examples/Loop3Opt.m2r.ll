; ModuleID = 'Loop3.m2r.ll'
source_filename = "Loop3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = add nsw i32 10, 5
  br label %2

2:                                                ; preds = %14, %0
  %.01 = phi i32 [ 0, %0 ], [ %1, %14 ]
  %.0 = phi i32 [ 0, %0 ], [ %15, %14 ]
  %3 = icmp slt i32 %.0, 10
  br i1 %3, label %4, label %16

4:                                                ; preds = %2
  %5 = icmp slt i32 %.0, 5
  br i1 %5, label %6, label %9

6:                                                ; preds = %4
  %7 = mul nsw i32 %.0, 2
  %8 = add nsw i32 %.01, %.0
  br label %16

9:                                                ; preds = %4
  %10 = icmp sgt i32 %.0, 5
  br i1 %10, label %11, label %13

11:                                               ; preds = %9
  %12 = add nsw i32 %1, 1
  br label %16

13:                                               ; preds = %9
  br label %14

14:                                               ; preds = %13
  %15 = add nsw i32 %.0, 1
  br label %2, !llvm.loop !6

16:                                               ; preds = %11, %6, %2
  %.02 = phi i32 [ %8, %6 ], [ 0, %11 ], [ 0, %2 ]
  ret i32 %.02
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
