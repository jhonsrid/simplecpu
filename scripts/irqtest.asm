MOV $I10 30	; Interrupt 10 will call location 30
IRQ 10		; Call interrupt 10
MOV $OUTPORTC 'F'
HLT



; INTERRUPT 10 SERVICE ROUTINE
REBASE 30
MOV $OUTPORTI 11223344
RET
