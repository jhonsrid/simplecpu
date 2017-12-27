# simplecpu, a simple CPU emulator and assembler in C99, ~1000 LOC.

## Directories
* scripts contains example .ASM files to run.

## Building
* ./make.sh

## Running
* simplecpu asmfilename
(Assembles and runs specified file). Currently no support for pre-assembled
memory images.
* simplecpu asmfilename 1
(Same, but print more information about assembly process)

## Assembly language syntax
;										Comment

:LABEL							Label

12345								Literal number

'A'									Literal ASCII character (eg 65)

:BOTTLES
"bottles of beer"		Labelled string

\#include						Include file, eg: #include "../scripts/incl.asm"

Target operand is first, eg:
    ADD REG LIT will add literal LIT to register REG.

### Instructions:

* HLT					HALT (stop CPU)
* NOP					No operation
* INC	REG				Increment register, eg INC R10
* DEC	REG				Decrement register, eg DEC R10
* ADD	REG LIT			Add literal to register
* ADD REG REG     Add register v	ADD REG LIT, or ADD REG REG
* SUB	REG LIT
* SUB REG REG
* MUL REG LIT
* MUL REG REG
* JMP :LABEL
* JEQ :LABEL			Jump to :LABEL if equal
* JNQ	:LABEL			Jump to :LABEL if not equal
* JGT	:LABEL			Jump to :LABEL if greater than
* JLT	:LABEL			Jump to :LABEL if less than
* MOV	REG REG			
* MOV REG LIT
* MOV REG PORT		MOV R9 $INPORTC or MOV R9 $INPORTI
* MOV PORT REG		MOV $OUTPORTC R10 or MOV $OUTPORTI R10		
* CMP REG LIT			Compare and set flags
* CMP REG REG			Compare and set flags
* PUSH LIT
* PUSH REG
* POP REG					Pop to specific register
* CALL						Call :LABELNAME (Subroutine call)
* RET							Return from subroutine
* PUSHU						Push all user registers to stack, (not SP or PC).
* POPU						Pop all user registers from stack, (not SP or PC).
* IRQ	NUM					Raise IRQ
* REBASE					Choose different memory location to continue assembling from.

### Flags:
* EQ							Equality flag
* C								Carry flag

### PORTS:
$INPORTC	Input character

$INPORTI	Input integer

$OUTPORTC	Print output character

$OUTPORTI	Print output integer

$I00 - $I?? Interrupt vectors, eg:
			MOV $I00 500 (set int 0 to jump to location 500 when called)

### VARIABLES
$XYZABC		Assign label "XYZABC" to a word of storage.

### Interrupts

$I00 to $I15 are the interrupt vectors.

IRQ0 is KEYBOARD interrupt, eg:

	; Setup interrupt handler (IRQ0)
	MOV $I00 500			; ie jump to location 500 to service IRQ0 (keyboard)

	REBASE 500
	MOV R9 $INPORTC			; Get character
	CMP R9 'X'				; See if it's eXit
	JEQ :INPEND				; Jump to "end of input"
	MOV $OUTPORTC R9		; Print value
	:INPEND					; "End of input" label
	RET						; Return

### Register use
16 registers.

R0 = PC

R1 = SP

R2 = Flags (Not yet used)

R3 to R9 = GP registers.

R10 = First CALL register

R11 = Second CALL register

R12 = Third CALL register

R16 = RETURN value register

### Stack
Stack starts at nearly top of memory (STACKTOP) and grows downwards. - No
checks (can overrun program code!)

### Variables
(Check this)
VARIABLE DECLARATION
$VAR {10} ok.
