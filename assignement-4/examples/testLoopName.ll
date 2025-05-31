; ModuleID = 'testLoopName.ll'
source_filename = "testLoopName.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [20 x i32], align 16
  br label %2

2:                                                ; preds = %8, %0
  %.02 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %3 = icmp slt i32 %.02, 10
  br i1 %3, label %4, label %10

4:                                                ; preds = %2
  %5 = mul nsw i32 %.02, 2
  %6 = sext i32 %.02 to i64
  %7 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %6
  store i32 %5, ptr %7, align 4
  br label %8

8:                                                ; preds = %4
  %9 = add nsw i32 %.02, 1
  br label %2, !llvm.loop !6

10:                                               ; preds = %2
  br label %11

11:                                               ; preds = %23, %10
  %.03 = phi i32 [ 0, %10 ], [ %24, %23 ]
  %12 = icmp slt i32 %.03, 10
  br i1 %12, label %13, label %25

13:                                               ; preds = %11
  %14 = sext i32 %.03 to i64
  %15 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %14
  %16 = load i32, ptr %15, align 4
  br label %17

17:                                               ; preds = %20, %13
  %.04 = phi i32 [ 0, %13 ], [ %21, %20 ]
  %18 = icmp slt i32 %.04, 10
  br i1 %18, label %19, label %22

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %19
  %21 = add nsw i32 %.04, 1
  br label %17, !llvm.loop !8

22:                                               ; preds = %17
  br label %23

23:                                               ; preds = %22
  %24 = add nsw i32 %.03, 1
  br label %11, !llvm.loop !9

25:                                               ; preds = %11
  br label %26

26:                                               ; preds = %32, %25
  %.01 = phi i32 [ 0, %25 ], [ %33, %32 ]
  %27 = icmp slt i32 %.01, 20
  br i1 %27, label %28, label %34

28:                                               ; preds = %26
  %29 = sext i32 %.01 to i64
  %30 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %29
  %31 = load i32, ptr %30, align 4
  br label %32

32:                                               ; preds = %28
  %33 = add nsw i32 %.01, 1
  br label %26, !llvm.loop !10

34:                                               ; preds = %26
  br label %35

35:                                               ; preds = %44, %34
  %.0 = phi i32 [ 0, %34 ], [ %45, %44 ]
  %36 = icmp slt i32 %.0, 5
  br i1 %36, label %37, label %46

37:                                               ; preds = %35
  %38 = add nsw i32 %.0, 1
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %39
  %41 = load i32, ptr %40, align 4
  %42 = sext i32 %.0 to i64
  %43 = getelementptr inbounds [20 x i32], ptr %1, i64 0, i64 %42
  store i32 %41, ptr %43, align 4
  br label %44

44:                                               ; preds = %37
  %45 = add nsw i32 %.0, 1
  br label %35, !llvm.loop !11

46:                                               ; preds = %35
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
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
