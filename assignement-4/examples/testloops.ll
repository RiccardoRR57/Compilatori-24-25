; filepath: test_loops.ll
define void @test_adjacent_loops() {
entry:
  br label %loop1.preheader

loop1.preheader:
  ; Blocco che precede l'header del primo loop
  br label %loop1.header

loop1.header:
  %i = phi i32 [ 0, %loop1.preheader ], [ %i.next, %loop1.latch ]
  %cond1 = icmp slt i32 %i, 10
  br i1 %cond1, label %loop1.body, label %loop1.exit

loop1.body:
  ; Corpo del primo loop - fai qualcosa qui
  call void @do_something_in_loop1(i32 %i)
  br label %loop1.latch

loop1.latch:
  %i.next = add nsw i32 %i, 1
  br label %loop1.header, !llvm.loop !1

loop1.exit:
  ; Questo è il blocco di uscita di loop1.
  ; Per l'adiacenza, questo blocco funge da preheader per loop2.
  ; Quindi, brancha direttamente all'header di loop2.
  br label %loop2.header

loop2.header:
  ; L'ingresso a questo header da fuori loop2 avviene da loop1.exit
  %j = phi i32 [ 0, %loop1.exit ], [ %j.next, %loop2.latch ]
  %cond2 = icmp slt i32 %j, 5
  br i1 %cond2, label %loop2.body, label %loop2.exit

loop2.body:
  ; Corpo del secondo loop - fai qualcosa qui
  call void @do_something_in_loop2(i32 %j)
  br label %loop2.latch

loop2.latch:
  %j.next = add nsw i32 %j, 1
  br label %loop2.header, !llvm.loop !2

loop2.exit:
  ret void
}

; Dichiarazioni di funzioni fittizie per evitare che i loop siano vuoti
declare void @do_something_in_loop1(i32)
declare void @do_something_in_loop2(i32)


!1 = distinct !{!1} ; ID per loop1
!2 = distinct !{!2} ; ID per loop2