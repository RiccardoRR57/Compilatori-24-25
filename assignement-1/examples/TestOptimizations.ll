; ModuleID = 'test_optimizations.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() {
entry:
    ; Test per Algebraic Identity
    %val1 = add i32 0, 42    ; Dovrebbe essere ottimizzato a %val1 = 42
    %val2 = mul i32 1, %val1 ; Dovrebbe essere ottimizzato a %val2 = %val1
    %val3 = sdiv i32 %val2, 1 ; Dovrebbe essere ottimizzato a %val3 = %val2
    %val4 = sub i32 %val3, 0 ; Dovrebbe essere ottimizzato a %val4 = %val3

    ; Test per Strength Reduction
    %mul1 = mul i32 %val4, 8    ; Dovrebbe essere ottimizzato a shift left di 3
    %mul2 = mul i32 %val4, 7    ; Dovrebbe essere ottimizzato a (val4 << 3) - val4
    %mul3 = mul i32 %val4, 9    ; Dovrebbe essere ottimizzato a (val4 << 3) + val4
    %div1 = sdiv i32 %mul1, 4   ; Dovrebbe essere ottimizzato a shift right di 2

    ; Test per Multi Instruction Optimization
    %add1 = add i32 %div1, 5
    %sub1 = sub i32 %add1, 5    ; Dovrebbe essere ottimizzato rimuovendo add e sub
    %add2 = add i32 %sub1, 10
    %sub2 = sub i32 %add2, 10   ; Dovrebbe essere ottimizzato rimuovendo add e sub

    ; Combine results
    %result = add i32 %mul2, %mul3
    %final = add i32 %result, %sub2

    ret i32 %final
} 