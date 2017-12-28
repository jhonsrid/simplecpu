; MOD test

MOV R10 0
:LOOP
INC R10
MOV R9 R10
MOD R9 10		; Destructive, better if it set ZERO flag instead?
CMP R9 0		
JNQ :LOOP
MOV $OUTPORTI R10
HLT

