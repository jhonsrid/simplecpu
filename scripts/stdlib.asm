; Some simple useful functions.

;;;;;;;;; STRLEN routine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine, so can only be included after program code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:STRLEN
MOV R16 0           ; 
:STRLENLOOP
CMP [R10] 0	        ; Does R10 pointer point to 0?
JEQ :STRLENEND      ; Have we hit end of string?
INC R16             ; Increment length
INC R10             ; Increment pointer
JMP :STRLENLOOP     ; Continue
:STRLENEND
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;; Simple PRINT routine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine, so can only be included after program code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:PRINT
MOV $OUTPORTC [R10]	; Write dereference of address
INC	R10				; Increment address in A
CMP [R10] 0			; Reached end?
JNQ :PRINT			; Jump if not.
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;; PRINT CRLF routine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine, so can only be included after program code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:PRINTCRLF
MOV $OUTPORTC 13
MOV $OUTPORTC 10
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;; Simple ATOI routine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Subroutine, so can only be included after program code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:ATOI
MOV R6 R10			; Copy of input address (strlen kills it)
CALL :STRLEN		; Get length of input length
MOV R10 R6			; Reset input address
MOV R8 0			; R8, stores built up number
MOV R7 0			; R7, stores temp character

:ATOILOOP
MOV R7 [R10] 		; Copy a character
CMP R7 0			; Is it 0?
JEQ	:ATOIEND		; End if so...

SUB R7 '0'			; Remove 48 ('0') to get numeric
ADD R7 R8			; Add numbers
INC R10				; Move to next character in string
CMP [R10] 0			; If last character, no need to bump
JEQ :ATOINOBUMP
MUL R7 10			; Bump number
:ATOINOBUMP
MOV R8 R7			; Store in built-up-number-location
JMP :ATOILOOP		; Repeat
:ATOIEND
MOV R16 R8			; Move temp variable to return register
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
