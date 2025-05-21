; ModuleID = 'loopfusion.ll'
source_filename = "loopfusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = dso_local constant i32 100, align 4
@i1 = dso_local global i32 1, align 4
@i2 = dso_local global i32 1, align 4
@guard = dso_local global i32 5, align 4
@a = dso_local global [100 x i32] zeroinitializer, align 16
@b = dso_local global [100 x i32] zeroinitializer, align 16
@c = dso_local global [100 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = load i32, ptr @guard, align 4
  %2 = icmp slt i32 %1, 4
  br i1 %2, label %3, label %11

3:                                                ; preds = %0
  br label %4

4:                                                ; preds = %7, %3
  %5 = load i32, ptr @i1, align 4
  %6 = add nsw i32 %5, 1
  store i32 %6, ptr @i1, align 4
  br label %7

7:                                                ; preds = %4
  %8 = load i32, ptr @i1, align 4
  %9 = icmp slt i32 %8, 100
  br i1 %9, label %4, label %10, !llvm.loop !6

10:                                               ; preds = %7
  br label %11

11:                                               ; preds = %10, %0
  %12 = load i32, ptr @guard, align 4
  %13 = icmp slt i32 %12, 4
  br i1 %13, label %14, label %22

14:                                               ; preds = %11
  br label %15

15:                                               ; preds = %18, %14
  %16 = load i32, ptr @i2, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, ptr @i2, align 4
  br label %18

18:                                               ; preds = %15
  %19 = load i32, ptr @i2, align 4
  %20 = icmp slt i32 %19, 100
  br i1 %20, label %15, label %21, !llvm.loop !8

21:                                               ; preds = %18
  br label %22

22:                                               ; preds = %21, %11
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
