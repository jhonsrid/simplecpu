; How quick to increment to x

;MOV $COUNTER 100000000	; Large number
MOV $COUNTER 20000000
MOV R9 0				; Load 0
:LOOP
INC R9					; INC register
CMP R9 $COUNTER        	; Compare
JNQ :LOOP				; Jump while not eq
HLT

$COUNTER
