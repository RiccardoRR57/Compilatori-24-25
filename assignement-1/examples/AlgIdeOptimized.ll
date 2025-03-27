; ModuleID = 'test/AlgIdeTest.ll'
source_filename = "test/AlgIdeTest.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
  %x = add i32 32, 12
  %result = add i32 %x, %x
  ret i32 %result
}
