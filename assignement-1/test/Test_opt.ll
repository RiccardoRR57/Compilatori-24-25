; ModuleID = 'Test.ll'
source_filename = "Test.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
  %val = add i32 10, 5
  %0 = shl i32 %val, 4
  %1 = sub i32 %0, %val
  %div = sdiv i32 %val, 4
  %div2 = udiv i32 %val, 4
  %result = add i32 %1, %div
  ret i32 %result
}
