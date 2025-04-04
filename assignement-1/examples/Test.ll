; ModuleID = 'test_strength_reduction.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Main function with simple strength reduction examples
define i32 @main() {
entry:
    ; Value to operate on
    %val = add i32 10, 5    ; Initialize %val = 15

    %add = add i32 3, %val
    %a = add i32 %val, 23
    %sub = sub i32 %add, 3
    %b = mul i32 %sub, 12
    %sub2 = sub i32 %add, 3
    
    ; Multiplication by power of 2 (can be replaced with shift left)
    %mul = mul i32 %add, 15   ; Can be replaced with %val << 3
    
    ; Division by power of 2 (can be replaced with shift right)
    %div = sdiv i32 %add, 4  ; Can be replaced with %val >> 2
    %div2 = udiv i32 %add, 4 ; Can be replaced with %val >> 2
    
    ; Combine results
    %result = add i32 %mul, %div
    
    ret i32 %result
}