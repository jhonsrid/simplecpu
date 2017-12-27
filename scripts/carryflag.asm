; Carry flag test (when using for "greater than")

MOV $VAL 123			
CMP $VAL 12 			; Compare value against value
JGT :GT					; Ie if $VAL > 12
JLT :LT					; ie if $VAL < 12
MOV $OUTPORTC 'E'		; Else eq
HLT

:GT
MOV $OUTPORTC 'G'
MOV $OUTPORTC ' '
HLT

:LT
MOV $OUTPORTC 'L'
MOV $OUTPORTC ' '

HLT
$VAL
