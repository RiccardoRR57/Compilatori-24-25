; ModuleID = 'prova.ll'
source_filename = "prova.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [8 x i8] c"L1: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [8 x i8] c"L2: %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [18 x i8] c"Loops terminati.\0A\00", align 1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @do_something_in_loop1(i32 noundef %0) #0 {
  %2 = srem i32 %0, 100
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %1
  %5 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %0)
  br label %6

6:                                                ; preds = %4, %1
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @do_something_in_loop2(i32 noundef %0) #0 {
  %2 = srem i32 %0, 100
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %1
  %5 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %0)
  br label %6

6:                                                ; preds = %4, %1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store volatile i32 1000, ptr %1, align 4
  store volatile i32 500, ptr %2, align 4
  br label %3

3:                                                ; preds = %7, %0
  %.01 = phi i32 [ 0, %0 ], [ %8, %7 ]
  %4 = load volatile i32, ptr %1, align 4
  %5 = icmp slt i32 %.01, %4
  br i1 %5, label %6, label %9

6:                                                ; preds = %3
  call void @do_something_in_loop1(i32 noundef %.01)
  br label %7

7:                                                ; preds = %6
  %8 = add nsw i32 %.01, 1
  br label %3, !llvm.loop !6

9:                                                ; preds = %3
  br label %10

10:                                               ; preds = %14, %9
  %.0 = phi i32 [ 0, %9 ], [ %15, %14 ]
  %11 = load volatile i32, ptr %2, align 4
  %12 = icmp slt i32 %.0, %11
  br i1 %12, label %13, label %16

13:                                               ; preds = %10
  call void @do_something_in_loop2(i32 noundef %.0)
  br label %14

14:                                               ; preds = %13
  %15 = add nsw i32 %.0, 1
  br label %10, !llvm.loop !8

16:                                               ; preds = %10
  %17 = call i32 (ptr, ...) @printf(ptr noundef @.str.2)
  ret i32 0
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
