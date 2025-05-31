; ModuleID = 'iternum.ll'
source_filename = "iternum.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [12 x i32], align 16
  %2 = icmp sgt i32 12, -12
  br i1 %2, label %3, label %12

3:                                                ; preds = %0
  br label %4

4:                                                ; preds = %9, %3
  %.0 = phi i32 [ 0, %3 ], [ %8, %9 ]
  %5 = mul nsw i32 %.0, 4
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [12 x i32], ptr %1, i64 0, i64 %6
  store i32 %5, ptr %7, align 4
  %8 = add nsw i32 %.0, 1
  br label %9

9:                                                ; preds = %4
  %10 = icmp slt i32 %8, 12
  br i1 %10, label %4, label %11, !llvm.loop !6

11:                                               ; preds = %9
  br label %12

12:                                               ; preds = %11, %0
  %13 = icmp sgt i32 12, -12
  br i1 %13, label %14, label %23

14:                                               ; preds = %12
  br label %15

15:                                               ; preds = %20, %14
  %.01 = phi i32 [ 0, %14 ], [ %19, %20 ]
  %16 = sext i32 %.01 to i64
  %17 = getelementptr inbounds [12 x i32], ptr %1, i64 0, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %.01, 1
  br label %20

20:                                               ; preds = %15
  %21 = icmp slt i32 %19, 12
  br i1 %21, label %15, label %22, !llvm.loop !8

22:                                               ; preds = %20
  br label %23

23:                                               ; preds = %22, %12
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
