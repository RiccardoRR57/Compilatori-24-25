; ModuleID = 'Loop2.c'
source_filename = "Loop2.c"
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
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 10, ptr %2, align 4
  store i32 5, ptr %3, align 4
  store i32 0, ptr %4, align 4
  store i32 0, ptr %5, align 4
  store i32 0, ptr %7, align 4
  br label %13

13:                                               ; preds = %33, %0
  %14 = load i32, ptr %7, align 4
  %15 = icmp slt i32 %14, 100
  br i1 %15, label %16, label %36

16:                                               ; preds = %13
  %17 = load i32, ptr %7, align 4
  %18 = icmp slt i32 %17, 50
  br i1 %18, label %19, label %20

19:                                               ; preds = %16
  br label %36

20:                                               ; preds = %16
  %21 = load i32, ptr %2, align 4
  %22 = load i32, ptr %3, align 4
  %23 = mul nsw i32 %21, %22
  %24 = add nsw i32 %23, 42
  store i32 %24, ptr %6, align 4
  %25 = load i32, ptr %6, align 4
  %26 = load i32, ptr %4, align 4
  %27 = add nsw i32 %26, %25
  store i32 %27, ptr %4, align 4
  %28 = load i32, ptr %7, align 4
  %29 = icmp sgt i32 %28, 80
  br i1 %29, label %30, label %32

30:                                               ; preds = %20
  %31 = load i32, ptr %6, align 4
  store i32 %31, ptr %5, align 4
  br label %36

32:                                               ; preds = %20
  br label %33

33:                                               ; preds = %32
  %34 = load i32, ptr %7, align 4
  %35 = add nsw i32 %34, 1
  store i32 %35, ptr %7, align 4
  br label %13, !llvm.loop !6

36:                                               ; preds = %30, %19, %13
  %37 = load i32, ptr %6, align 4
  %38 = add nsw i32 20, %37
  store i32 %38, ptr %8, align 4
  store i32 7, ptr %9, align 4
  store i32 0, ptr %10, align 4
  store i32 0, ptr %11, align 4
  br label %39

39:                                               ; preds = %60, %36
  %40 = load i32, ptr %11, align 4
  %41 = icmp slt i32 %40, 50
  br i1 %41, label %42, label %63

42:                                               ; preds = %39
  %43 = load i32, ptr %11, align 4
  %44 = srem i32 %43, 10
  %45 = icmp eq i32 %44, 0
  br i1 %45, label %46, label %59

46:                                               ; preds = %42
  %47 = load i32, ptr %8, align 4
  %48 = load i32, ptr %9, align 4
  %49 = mul nsw i32 %47, %48
  %50 = sub nsw i32 %49, 15
  store i32 %50, ptr %12, align 4
  %51 = load i32, ptr %11, align 4
  %52 = icmp sgt i32 %51, 30
  br i1 %52, label %53, label %55

53:                                               ; preds = %46
  %54 = load i32, ptr %12, align 4
  store i32 %54, ptr %10, align 4
  br label %63

55:                                               ; preds = %46
  %56 = load i32, ptr %12, align 4
  %57 = load i32, ptr %4, align 4
  %58 = add nsw i32 %57, %56
  store i32 %58, ptr %4, align 4
  br label %59

59:                                               ; preds = %55, %42
  br label %60

60:                                               ; preds = %59
  %61 = load i32, ptr %11, align 4
  %62 = add nsw i32 %61, 1
  store i32 %62, ptr %11, align 4
  br label %39, !llvm.loop !8

63:                                               ; preds = %53, %39
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
