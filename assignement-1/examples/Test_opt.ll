; ModuleID = 'Test.ll'
source_filename = "Test.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
  %val = add i32 10, 5
  %add = add i32 3, %val
  %a = add i32 %val, 23
  %b = mul i32 %val, 12
  %mul = mul i32 %add, 15
  %div = sdiv i32 %add, 4
  %div2 = udiv i32 %add, 4
  %result = add i32 %mul, %div
  ret i32 %result
}
