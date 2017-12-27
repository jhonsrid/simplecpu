; Get input from user, store it, print it back out.
MOV R12 :TEXT
:MAINLOOP
CALL :PRINTPROMPT
MOV R9 $INPORTC		; Get input
CMP R9 13 			; lf
JEQ :END
CMP R9 10			; cr
JEQ :END
MOV $OUTPORTC R9	; Echo as we go
CALL :CRLF
MOV [R12] R9 		; Save character in variable pointed to by R10.
INC R12
JMP :MAINLOOP

:END
CALL :CRLF

CALL :PRINTPROMPT2

MOV R10 :TEXT		; Reset to beginning of ADDR
CALL :PRINTF		; Print the whole thing
HLT

:PRINTPROMPT
MOV R10 :PROMPT
CALL :PRINTF
RET

:PRINTPROMPT2
CALL :CRLF
CALL :CRLF
MOV R10 :PROMPT2
CALL :PRINTF
RET

;;;;;;;;; Simple PRINTF routine ;;;;;;;;;
:PRINTF
MOV $OUTPORTC [R10]	; Write dereference of address
INC	R10				; Increment address in A
CMP [R10] 0			; Reached end?
JNQ :PRINTF			; Jump if not.
RET

:CRLF
MOV $OUTPORTC 10
MOV $OUTPORTC 13
RET

:PROMPT
"INPUT CHARACTER:  "
0

:PROMPT2
"Your text is : "
0

$VALVAL
:TEXT ; Don't have syntax for allocating more than one word of storage yet other than this! (Or allocating string)
0
0
0
0
0
0
0
0


$ADDR
