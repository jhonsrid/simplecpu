; ATOI TEST
MOV R10 :STRINGNUM
CALL :ATOI
MOV $OUTPORTI R16

; PRINTCRLF TEST
CALL :PRINTCRLF

; PRINT TEST
MOV R10 :STRINGLENIS
CALL :PRINT

; STRLEN TEST
MOV R10 :STRINGVAL
CALL :STRLEN
MOV $OUTPORTI R16

HLT

:STRINGNUM
"5432123"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:STRINGVAL
"ABCDEFGHIJKL"		; 12 characters

:STRINGLENIS
"String length is: "

#include stdlib.asm
