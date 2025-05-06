; ModuleID = 'Loop2.ll'
source_filename = "Loop2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  br label %1

1:                                                ; preds = %13, %0
  %.03 = phi i32 [ undef, %0 ], [ %8, %13 ]
  %.02 = phi i32 [ 0, %0 ], [ %9, %13 ]
  %.01 = phi i32 [ 0, %0 ], [ %14, %13 ]
  %2 = icmp slt i32 %.01, 100
  br i1 %2, label %3, label %15

3:                                                ; preds = %1
  %4 = icmp slt i32 %.01, 50
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  br label %15

6:                                                ; preds = %3
  %7 = mul nsw i32 10, 5
  %8 = add nsw i32 %7, 42
  %9 = add nsw i32 %.02, %8
  %10 = icmp sgt i32 %.01, 80
  br i1 %10, label %11, label %12

11:                                               ; preds = %6
  br label %15

12:                                               ; preds = %6
  br label %13

13:                                               ; preds = %12
  %14 = add nsw i32 %.01, 1
  br label %1, !llvm.loop !6

15:                                               ; preds = %11, %5, %1
  %.14 = phi i32 [ %.03, %5 ], [ %8, %11 ], [ %.03, %1 ]
  %.1 = phi i32 [ %.02, %5 ], [ %9, %11 ], [ %.02, %1 ]
  %16 = add nsw i32 20, %.14
  br label %17

17:                                               ; preds = %30, %15
  %.2 = phi i32 [ %.1, %15 ], [ %.3, %30 ]
  %.0 = phi i32 [ 0, %15 ], [ %31, %30 ]
  %18 = icmp slt i32 %.0, 50
  br i1 %18, label %19, label %32

19:                                               ; preds = %17
  %20 = srem i32 %.0, 10
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %22, label %29

22:                                               ; preds = %19
  %23 = mul nsw i32 %16, 7
  %24 = sub nsw i32 %23, 15
  %25 = icmp sgt i32 %.0, 30
  br i1 %25, label %26, label %27

26:                                               ; preds = %22
  br label %32

27:                                               ; preds = %22
  %28 = add nsw i32 %.2, %24
  br label %29

29:                                               ; preds = %27, %19
  %.3 = phi i32 [ %28, %27 ], [ %.2, %19 ]
  br label %30

30:                                               ; preds = %29
  %31 = add nsw i32 %.0, 1
  br label %17, !llvm.loop !8

32:                                               ; preds = %26, %17
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
