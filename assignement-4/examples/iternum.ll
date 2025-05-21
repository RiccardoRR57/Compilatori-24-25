; ModuleID = 'iternum.ll'
source_filename = "iternum.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  br label %2

2:                                                ; preds = %7, %0
  %.01 = phi i32 [ 0, %0 ], [ %6, %7 ]
  %3 = mul nsw i32 2, %.01
  %4 = sext i32 %.01 to i64
  %5 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %4
  store i32 %3, ptr %5, align 4
  %6 = add nsw i32 %.01, 1
  br label %7

7:                                                ; preds = %2
  %8 = icmp slt i32 %6, 10
  br i1 %8, label %2, label %9, !llvm.loop !6

9:                                                ; preds = %7
  br label %10

10:                                               ; preds = %15, %9
  %.0 = phi i32 [ 0, %9 ], [ %14, %15 ]
  %11 = sext i32 %.0 to i64
  %12 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %11
  %13 = load i32, ptr %12, align 4
  %14 = add nsw i32 %.0, 1
  br label %15

15:                                               ; preds = %10
  %16 = icmp slt i32 %14, 10
  br i1 %16, label %10, label %17, !llvm.loop !8

17:                                               ; preds = %15
  ret i32 0
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
!8 = distinct !{!8, !7}
