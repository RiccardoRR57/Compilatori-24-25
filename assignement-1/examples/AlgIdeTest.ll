; ModuleID = 'test_algebraic_identities.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
    %x = add i32 32, 12      ; Initialize %x = 42
    ; Initialize value
    %val = add i32 %x, 0    ; Should optimize to just 42
    
    ; Division by 1 (should optimize to same number)
    %div1 = sdiv i32 %val, 1   ; Should optimize to %val
    
    ; Subtraction by 0 (should optimize to same number)  
    %sub0 = sub i32 %val, 0    ; Should optimize to %val
    
    ; Addition by 0 (should optimize to same number)
    %add0 = add i32 %val, 0    ; Should optimize to %val
    
    ; Multiplication by 1 (should optimize to same number)
    %mul1 = mul i32 %val, 1    ; Should optimize to %val
    
    ; Combine results
    %result = add i32 %div1, %sub0
    
    ret i32 %result
}