; ModuleID = 'Loop3.c'
source_filename = "Loop3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 10, ptr %2, align 4
  store i32 5, ptr %3, align 4
  store i32 0, ptr %4, align 4
  store i32 0, ptr %5, align 4
  store i32 0, ptr %6, align 4
  br label %10

10:                                               ; preds = %32, %0
  %11 = load i32, ptr %6, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %35

13:                                               ; preds = %10
  %14 = load i32, ptr %6, align 4
  %15 = icmp slt i32 %14, 5
  br i1 %15, label %16, label %22

16:                                               ; preds = %13
  %17 = load i32, ptr %6, align 4
  %18 = mul nsw i32 %17, 2
  store i32 %18, ptr %4, align 4
  %19 = load i32, ptr %5, align 4
  %20 = load i32, ptr %6, align 4
  %21 = add nsw i32 %19, %20
  store i32 %21, ptr %4, align 4
  br label %35

22:                                               ; preds = %13
  %23 = load i32, ptr %2, align 4
  %24 = load i32, ptr %3, align 4
  %25 = add nsw i32 %23, %24
  store i32 %25, ptr %5, align 4
  %26 = load i32, ptr %6, align 4
  %27 = icmp sgt i32 %26, 5
  br i1 %27, label %28, label %31

28:                                               ; preds = %22
  %29 = load i32, ptr %5, align 4
  %30 = add nsw i32 %29, 1
  store i32 %30, ptr %7, align 4
  br label %35

31:                                               ; preds = %22
  br label %32

32:                                               ; preds = %31
  %33 = load i32, ptr %6, align 4
  %34 = add nsw i32 %33, 1
  store i32 %34, ptr %6, align 4
  br label %10, !llvm.loop !6

35:                                               ; preds = %28, %16, %10
  store i32 12, ptr %8, align 4
  %36 = load i32, ptr %8, align 4
  %37 = load i32, ptr %5, align 4
  %38 = add nsw i32 %36, %37
  store i32 %38, ptr %9, align 4
  %39 = load i32, ptr %4, align 4
  ret i32 %39
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
