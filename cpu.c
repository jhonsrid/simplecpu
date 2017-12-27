#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "main.h"

#ifdef WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

static int *lv, *rv;
static int lvvirtual, rvvirtual;

static void push2(int reg)
{
	if (SPVALUE < (STACKTOP - STACKELEMENTS))
	{
		printf("RUNTIME ERROR, STACK OVERFLOW\n");
		exit(-1);
	}

	memory[SPVALUE] = memory[reg];
	SPVALUE--; /* Decrement */
}

unsigned char prevchar = 0; /* Bodge for windows, if prevchar was CR, send LF this time. */

int dokbd(void)
{
#ifdef WIN32
	if (prevchar == 13) {
		memory[INPORTC_LOCATION] = 10;
		prevchar = 10;
		return 1;
	}
	if (_kbhit()) {
		memory[INPORTC_LOCATION] = _getch();
		prevchar = memory[INPORTC_LOCATION];
		return 1;
	}
#else
	if (kbhit())	
	{
		int test = getchar();
		printf("echo: %c\n", test);
		memory[INPORTC_LOCATION] = test;
		return 1;
	}
#endif
return 0;
}

/* Interrupt request, basically same as CALL, but to location specified in vector table based on interrupt number */
void inner_irq(int lam, int ram, int irqnum)
{
	pushu(lam, ram);		/* Push all user registers, (not PC, SP or R15!) */
	push2(pc);				/* Push PC */

	if (irqnum < 0 || irqnum > 15) {
		printf("RUNTIME ERROR: Illegal IRQ number %d\n", irqnum);
		exit(-1);
	}
	PCVALUE = memory[VECTOR_BASE + irqnum]; /* Set PC to dereference of vector table */
	printf("PCVALUE = %d\n", PCVALUE);
	if (PCVALUE < 16 || PCVALUE > MEMWORDS) {
		printf("RUNTIME ERROR: Illegal PC value (%d) from IRQ (%d) deref (%d) in vector table\n", PCVALUE, irqnum, memory[VECTOR_BASE + irqnum]);
	}
}

int runtime(void)
{
	int cycles = 0;
	int opcode;
	int lam, ram;
	PCVALUE = STARTPOS;
	SPVALUE = STACKTOP; /* Empty stack */

	//printf("\nRunning...\n\n");

	while(!stoprun)
	{
		if ((cycles % 1000) == 0 && dokbd()) /* Raise an interrupt if keyboard hit */
		{
			printf("cycle: %d\n", cycles);
			PCVALUE = PCVALUE - 2;	/* Pre-decrement PC twice, (because RET and then MAIN CODE is going to increment it!) */
			inner_irq(0, 0, KEYPRESS_INTERRUPT);
		}

		opcode = PCTARGET & 0x000000FF;			/*  0-7  (instruction code) */
		lam = (PCTARGET & 0x0000FF00) >> 8;		/*  8-15 (left hand addressing mode) */
		ram = (PCTARGET & 0x00FF0000) >> 16;	/* 16-23 (right hand addressing mode) */

		if (opcode < INSTRUCTION_COUNT) {

			setlvrv(lam, ram, opcode);
			instructions[opcode].handler(lam, ram);
			if (!absolute)
				PCVALUE += (instructions[opcode].length);
			absolute = 0;
		}
		else {
			printf("Unknown operand at memory location %d\nExiting...", PCVALUE);
			exit(-1);
		}

		cycles++;
	}
	return cycles;
}

/* TODO: Break into subroutine and caller with lv/rv and offset (1/2) */
void setlvrv(int lam, int ram, int opcode)
{
	lvvirtual = 0;
	rvvirtual = 0;
	lv = (int *)0;
	rv = (int *)0;

	switch (lam)
	{
		case INS_DEREF:
			if (PCTARGETPLUS1 > MEMWORDS) lvvirtual = PCTARGETPLUS1;
			lv = &PCDEREFPLUS1;
			break;
		case INS_ADDRDEREF:
			lv = &PCADDRESSDEREFPLUS1;
			break;
		case INS_IMMEDIATE:
			lv = &PCTARGETPLUS1;
			break;
	}

	if (instructions[opcode].length <= 2) return; /* Can't check RHS for 2-operand instructions! */

	switch (ram)
	{
		case INS_DEREF:
			if (PCTARGETPLUS2 > MEMWORDS) rvvirtual = PCTARGETPLUS2;
			rv = &PCDEREFPLUS2;
			break;
		case INS_ADDRDEREF:
			rv = &PCADDRESSDEREFPLUS2;
			break;
		case INS_IMMEDIATE:
			rv = &PCTARGETPLUS2;
			break;
	}
}

/* INSTRUCTIONS */

void mov(int lam, int ram)
{
	if (lvvirtual)
	{
		if (lvvirtual == OUTPORTC_LOCATION)
			printf("%c", *rv);
		else if (lvvirtual == OUTPORTI_LOCATION)
			printf("%d", *rv);
	}
	else if (rvvirtual && rvvirtual == INPORTC_LOCATION)
	{
		*lv = memory[INPORTC_LOCATION];
	}
	else
		*lv = *rv;
	return;
}

void irq(int lam, int ram)
{
	inner_irq(lam, ram, PCTARGETPLUS1);
	absolute = 1; /* Required! */
}

void cmp(int lam, int ram)
{
	eq = (*lv == *rv);
	c = (*lv >= *rv) ? 1 : 0;
	return;
}

void jmp2(int lam, int ram, int flag)
{
	if (flag) {
		PCVALUE = PCTARGETPLUS1;
		absolute=1;
	}
}

static void pop_inner(int reg)
{
	if (SPVALUE == STACKTOP)
	{
		printf("\n\n Can't POP without PUSH!\n\n");
		exit(-1);
	}

	SPVALUE++; /* Increment */
	memory[reg]  = memory[SPVALUE]; /* Dereference SP */
}

void jeq(int lam, int ram)  { jmp2(lam, ram, eq); }
void jnq(int lam, int ram)  { jmp2(lam, ram, !eq); }
void jgt(int lam, int ram)  { jmp2(lam, ram, (c && !eq)); }
void jlt(int lam, int ram)  { jmp2(lam, ram, (!c && !eq)); }
void jmp(int lam, int ram)  { jmp2(lam, ram, 1); }
void push(int lam, int ram) { push2(PCTARGETPLUS1); }
void pop(int lam, int ram)  { pop_inner(PCTARGETPLUS1); }

void pushu(int lam, int ram) /* Push all user registers, (not PC, SP or R15 (return register)) */
{
	int reg;
	for (reg=2; reg < 15; reg++) {
		push2(reg);
	}
}

void popu(int lam, int ram) /* Pop all user registers, (not PC, SP or R15 (return register)) */
{
	int reg;
	for(reg=14; reg >= 2; reg--) {
		pop_inner(reg);
	}
}

void call(int lam, int ram)
{
	pushu(lam, ram);	/* Push all user registers, (not PC, SP or R15!) */
	push2(pc);
	PCVALUE = PCTARGETPLUS1;
	absolute = 1;
}

void ret(int lam, int ram)
{
	pop_inner(pc);		/* Let runtime increment pc for us! */
	popu(lam, ram);	/* Pop all user registers (not PC, SP or R15!) */
	PCVALUE = PCVALUE + 1;	/* increment PC, runtime will increment again. (Ie skip *2* bytes, ie "CALL <loc>") */
}

void inc(int lam, int ram)  { PCDEREFPLUS1 = PCDEREFPLUS1 + 1; }
void dec(int lam, int ram)  { PCDEREFPLUS1 = PCDEREFPLUS1 - 1; }
void mul(int lam, int ram)  { *lv = (*lv) * (*rv); } /* No overflow check! */
void add(int lam, int ram)  { *lv = (*lv) + (*rv); } /* No overflow check! */
void sub(int lam, int ram)  { *lv = (*lv) - (*rv); } /* No overflow check! */
void nop(int lam, int ram)  {}
void hlt(int lam, int ram)
{
	printf("\nGot HALT at location %d (%d)\n", pc, memory[pc]);
	stoprun=1;
}
