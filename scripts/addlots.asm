; Add test

MOV R10 0
:LOOP
INC R10
CMP R10 200000000
JNQ :LOOP
MOV $OUTPORTI R10
HLT

