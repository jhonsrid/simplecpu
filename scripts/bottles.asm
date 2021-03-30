; 99 bottles of beer

MOV $NUMBOTTLES 99		   ; Setup

:MAINLOOP				         ; Main loop
CALL :PRINTNUMBER
CALL :PRINTBOTTLES
CALL :PRINTONWALL
MOV $OUTPORTC ','
MOV $OUTPORTC ' '
CALL :PRINTNUMBER
CALL :PRINTBOTTLES
MOV $OUTPORTC ','
CALL :PRINTTAKEONE
DEC $NUMBOTTLES
CALL :PRINTNUMBER
CALL :PRINTBOTTLES
CALL :PRINTONWALL
MOV $OUTPORTC '.'
MOV $OUTPORTC '\n'

CMP $NUMBOTTLES 0
JNQ :MAINLOOP
HLT

;;;;;;;;;;;;;; SUBROUTINES ;;;;;;;;;;;;;;

:PRINTNUMBER
MOV $OUTPORTI $NUMBOTTLES	; Print number
MOV $OUTPORTC ' '		      ; And a space (' ' doesn't work yet)
RET

:PRINTBOTTLES
CMP $NUMBOTTLES 1
JEQ :ONEBOTT
MOV R10 :BOTTLES
CALL :PRINTSTR
RET
:ONEBOTT
MOV R10 :BOTTLE
CALL :PRINTSTR
RET

:PRINTONWALL
MOV R10 :ONWALL
CALL :PRINTSTR
RET

:PRINTTAKEONE
MOV R10 :TAKEONE
CALL :PRINTSTR
RET

;;;;;;;;; PRINTSTR ;;;;;;;;;
:PRINTSTR
MOV $OUTPORTC [R10]	         ; Write dereference of address
INC	R10                      ; Increment address in A
CMP [R10] 0			             ; Reached end?
JNQ :PRINTSTR                  ; Jump back if not.
RET

;;;;;;;; DATA LABELS ;;;;;;;;;;;
:BOTTLES "bottles of beer"
:BOTTLE  "bottle of beer"
:ONWALL  " on the wall"
:TAKEONE " take one down, pass it around, now there's "

; VARIABLES:
$NUMBOTTLES
