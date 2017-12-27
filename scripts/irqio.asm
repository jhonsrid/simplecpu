; Interrupt driven I/O example

; Setup interrupt handler (IRQ0)
MOV $I00 500

; Main routine (doesn't do anything)
:MAIN
NOP
JMP :MAIN

; Example IRQ service for "input character"
REBASE 500			; Continue assembling at location 500dec
MOV R9 $INPORTC		; Get character
CMP R9 'X'			; See if it's eXit
JEQ :INPEND			; 
MOV $OUTPORTC R9	; Print value
RET					; Return

:INPEND				
HLT					; Finish if 'X'

