; ModuleID = 'TestOptimizations.ll'
source_filename = "TestOptimizations.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
  %0 = shl i32 42, 3
  %1 = shl i32 42, 3
  %2 = sub i32 %1, 42
  %3 = shl i32 42, 3
  %4 = add i32 %3, 42
  %div1 = sdiv i32 %0, 4
  %add1 = add i32 %div1, 5
  %add2 = add i32 %div1, 10
  %result = add i32 %2, %4
  %final = add i32 %result, %div1
  ret i32 %final
}
