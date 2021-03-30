# simplecpu, a simple CPU architecture, emulator and assembler in C99, ~1000 LOC.

C99, no dependencies other than libc and POSIX. Doesn't use lexx/flex or yacc/bison for lexing / parsing.

## Directories
* scripts contains example .ASM files to run.

## Building
* POSIX (Linux [x64 and Raspberry Pi]), OSX, Cygwin)
* ./make.sh

* Windows (native, Visual Studio 2015 Community).
* VS\simplecpu.sln

## Running
* simplecpu asmfilename [Y]

Assembles and runs specified file. Currently no support for pre-assembled memory images.
Y being optional debug output from assembler.

## Assembly language syntax
;										Comment

:LABEL									Label

12345									Literal number

'A'										Literal ASCII character (eg 65)

:BOTTLES
"bottles of beer"						Labelled string

\#include								Include file, eg: #include "../scripts/incl.asm"

Target operand is first, eg:
    ADD REG LIT will add literal LIT to register REG.

### Instructions:

* HLT							HALT (stop CPU)
* NOP							No operation
* INC REG						Increment register, eg INC R10
* DEC REG						Decrement register, eg DEC R10
* ADD REG (LIT|REG)				Add literal or register value to register
* SUB REG (LIT|REG)             Subtract literal or register value from register
* MUL REG (LIT|REG)             Multiply register value by literal or register value
* JMP :LABEL                    Unconditional jump to :LABEL
* JEQ :LABEL					Jump to :LABEL if equal
* JNQ :LABEL					Jump to :LABEL if not equal
* JGT :LABEL					Jump to :LABEL if greater than
* JLT :LABEL					Jump to :LABEL if less than
* MOV REG (LIT|REG|PORT)        Copy value from register, literal or port to register, eg: MOV R9 $INPORTC
* MOV PORT REG					Copy value from register to port, eg: MOV $OUTPORTC R10 or MOV $OUTPORTI R10		
* CMP REG (LIT|REG)				Compare and set flags 
* PUSH (LIT|REG)				Push value or register value to stack
* POP REG						Pop value to specific register
* CALL							Call :LABELNAME (Subroutine call)
* RET							Return from subroutine
* PUSHU							Push all user registers to stack, (not SP or PC).
* POPU							Pop all user registers from stack, (not SP or PC).
* IRQ NUM						Raise IRQ
* MOD REG LIT					Modulo division, leave remainder in REG.
* MOD REG REG					Modulo division, leave remainder in REG.

### Additional Assembler instructions:
Not CPU instructions, but commands for assembler.
* REBASE						Choose different memory location to continue assembling from.

### CPU Flags:
* EQ							Equality flag
* C								Carry flag

### Supported PORTS:

* $INPORTC						Input character

* $INPORTI						Input integer

* $OUTPORTC						Print output character

* $OUTPORTI						Print output integer

* $I00 - $I15						Interrupt vectors

### VARIABLES
$XYZABC							Assign label "XYZABC" to a word of storage.

### Interrupts

$I00 to $I15 are the interrupt vectors.

IRQ0 is KEYBOARD interrupt, eg:

	; Setup interrupt handler (IRQ0)
	MOV $I00 500			; ie jump to location 500 to service IRQ0 (keyboard)

	REBASE 500				; Move assembly point to location 500
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

R2 = Flags (Not yet supported)

R3 to R9 = GP registers.

R10 = First CALL register

R11 = Second CALL register

R12 = Third CALL register

R16 = RETURN value register

### Stack
Stack starts at nearly top of memory (STACKTOP) and grows downwards. - No stack overflow checks.

### Variables
Declare word variables as:
$NUMBOTTLES

Declare static string variables as (for instance):

:TAKEONE " take one down, pass it around, now there's "


## TODO
* Terminal control is incomplete for all platforms, so monitor.asm will not work (and is incomplete itself)
* Interrupt driven I/O needs work
* Would like to simplify instruction set further, possibly make this more 6502-like.
* Not tested on Linux / MacOS / BSD
