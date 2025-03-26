; ModuleID = 'Test.ll'
source_filename = "Test.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
  %val = add i32 10, 5
  %0 = shl i32 %val, 3
  %1 = lshr i32 %val, 2
  %2 = lshr i32 %val, 2
  %result = add i32 %0, %1
  ret i32 %result
}
