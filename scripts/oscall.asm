; Interrupt driven I/O example

; Setup interrupt handler (IRQ1)
MOV $I01 500

; Main routine (interrupt based system calls)
:MAIN
MOV R8 102
IRQ 1
HLT
	
; Example IRQ service for "input character"
REBASE 500
MOV $OUTPORTI R8
RET					; Return
