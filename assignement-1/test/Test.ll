; ModuleID = 'test_strength_reduction.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Main function with simple strength reduction examples
define i32 @main() {
entry:
    ; Value to operate on
    %val = add i32 10, 5    ; Initialize %val = 15
    
    ; Multiplication by power of 2 (can be replaced with shift left)
    %mul = mul i32 %val, 15   ; Can be replaced with %val << 3
    
    ; Division by power of 2 (can be replaced with shift right)
    %div = sdiv i32 %val, 4  ; Can be replaced with %val >> 2
    %div2 = udiv i32 %val, 4 ; Can be replaced with %val >> 2
    
    ; Combine results
    %result = add i32 %mul, %div
    
    ret i32 %result
}