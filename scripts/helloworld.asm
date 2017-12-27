; Hello world test program
;
MOV R3 :BEGINTEXT		; Load address of begintext
:PRINTSTART
MOV $OUTPORTC [R3]		; Write dereference of address from R3 // JCB: TODO!
INC R3					; Increment address in R3
CMP R3 :ENDTEXT 		; Reached end?
JNQ :PRINTSTART			; Jump if not.
HLT

:BEGINTEXT
"Hello World!" 			; String literal
10						; int literal 10 (\n)
:ENDTEXT
