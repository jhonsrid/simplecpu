; Test CALL and RET.
MOV $OUTPORTC '1'
CALL :CRLF
MOV $OUTPORTC '2'
CALL :CRLF
MOV R10 :STUFF
CALL :PRINTF
CALL :CRLF
MOV R10 123
CALL :PRINTSTUFF
CALL :CRLF
HLT

:PRINTSTUFF
MOV R10 :STUFF
CALL :PRINTF
RET

;;;;;;;;; Simple PRINTF routine ;;;;;;;;;
:PRINTF
MOV $OUTPORTC [R10]	; Write dereference of address
INC	R10				; Increment address in A
CMP [R10] 0			; Reached end?
JNQ :PRINTF			; Jump if not.
CALL :CRLF
CALL :CRLF
RET

:CRLF
MOV $OUTPORTC 13
MOV $OUTPORTC 10
RET

:STUFF
"STUFF "
0
