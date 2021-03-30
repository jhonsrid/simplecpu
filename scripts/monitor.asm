; Monitor O/S

; R10 
; R12 used for input buffer pointer

; Setup interrupt handler for keypress (IRQ0)
MOV $I00 500            ; Interrupt vector

; Init stuff
:INIT 
MOV R12 $INPUT
CALL :PRINTPROMPT

; Main routine
:MAIN
NOP
JMP :MAIN

; Main IRQ processing
REBASE 500
MOV R9 $INPORTC         ; Get character
MOV $OUTPORTC R9        ; Echo input character

CMP R9 10               ; Is char LF?
JNQ :NOTLF              ; if not
CMP $CMDCHAR 'X'        ; Is char X (exit)?
JEQ :X
CMP $CMDCHAR 'H'        ; Is char H (help)?
JEQ :H
CMP $CMDCHAR 'D'        ; Is char D (dump memory)?
JEQ :D
RET

:NOTLF                  ; Wasn't LF
MOV $CMDCHAR R9         ; Save CMDCHAR
RET

:PRINTPROMPT
MOV R10 :PROMPT
CALL :PRINT
RET                     ; Finished

:D                      ; Dump memory, takes decimal address
MOV R10 $STRINGNUM
CALL :ATOI
MOV $OUTPORTI R16
RET

:H
MOV R10 :HELPTEXT
CALL :PRINTLINE
CALL :PRINTCRLF
CALL :PRINTPROMPT
RET

:X
HLT                        ; HALT on 'X'

:PRINTLINE
CALL :PRINTCRLF
CALL :PRINT
CALL :PRINTCRLF
RET

:PRINTCRLF
MOV $OUTPORTC 13
MOV $OUTPORTC 10
RET

:PRINT
MOV $OUTPORTC [R10]       ; Write dereference of address
INC    R10                ; Increment address in A
CMP [R10] 0               ; Reached end?
JNQ :PRINT                ; Jump if not.
RET

#include "stdlib.asm"

:PROMPT
"$ "                      ; Prompt

:HELPTEXT
" H:Help, X:Exit, D<decimaladdr>:Dump memory, (not yet implemented)"

$CMDCHAR                  ; Variable

$INPUT
"          "              ; Keyboard input buffer

$STRINGNUM
"         "
