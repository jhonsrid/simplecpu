#include <stdint.h>

#define PCVALUE memory[0]
#define PCTARGET memory[memory[0]]
#define PCTARGETPLUS1 memory[memory[0] +1]
#define PCTARGETPLUS2 memory[memory[0] +2]
#define PCDEREF memory[memory[memory[0]]
#define PCDEREFPLUS1 memory[memory[memory[0] +1]]
#define PCDEREFPLUS2 memory[memory[memory[0] +2]]

#define PCADDRESSDEREF memory[memory[memory[memory[0]]]]
#define PCADDRESSDEREFPLUS1 memory[memory[memory[memory[0] +1]]]
#define PCADDRESSDEREFPLUS2 memory[memory[memory[memory[0] +2]]]

#define SPVALUE memory[1]

#define MEMWORDS (1024 + 16 + 2) /* Word addressed machine, "16" is to leave room for interrupt vectors on top, plus gap plus INPORTC above that! */
#define STACKTOP 1023			 /* Start stack at (0 based) 1023, (ie last word in memory) */
#define STACKELEMENTS 1000		 /* Maximum number of elements in stack */
#define STARTPOS 16				 /* Start at memory location 16, (0 based), to avoid scribbing on registers! */
#define VECTOR_BASE	1024		 /* 0 based, ie 1025th element */
#define KBDPOLL_INTERVAL 1000	/* Number of cycles between keyboard polls */


/* R0 to R15 are first 16 words of memory, no need for pretend register addresses
 * R0 is program counter.
 * R1 is stack pointer.
 */

/* Hardcoded memory addresses (outside of physical memory range) */
#define OUTPORTC_LOCATION (1 << 23) + 1
#define OUTPORTI_LOCATION (1 << 23) + 2
#define INPORTC_LOCATION (MEMWORDS - 1) /* no INPORTI */

#define pc 0

#ifdef MAIN_GLOBALS

int memory[MEMWORDS]; /* 16777216 bytes (4194304 words). ((22bit addressing) */

/* Flags aren't part of a register yet */
uint8_t eq; /* Equality flag */
uint8_t c;  /* Carry flag */

int stoprun=0;
int absolute=0; /* ie don't post-fiddle with pc */
int wordcount=0; /* Assembly word count */
#else
extern int memory[MEMWORDS]; /* 16777216 bytes (4194304 words). ((22bit addressing) */
extern uint8_t eq; /* Equality flag */
extern uint8_t c;  /* Carry flag */
extern int stoprun;
extern int absolute; /* ie don't post-fiddle with pc */
extern int wordcount; /* Assembly word count */
#endif

#define MAX_LINE_LEN 1024

typedef void (*Handler)(int leftaddressmode, int rightaddressmode);

typedef struct instruction {
	char *name;
	uint32_t opcode; /* Opcode is actually position in opcode table */
	uint8_t length;
	Handler handler;
} t_instruction;

/* Instruction prototypes */
void nop(int lam, int ram);		/* No operation */
void hlt(int lam, int ram);		/* Halt */
void inc(int lam, int ram);		/* Increment */
void dec(int lam, int ram);		/* Decrement */
void add(int lam, int ram);		/* Add */
void sub(int lam, int ram);		/* Sub */
void mul(int lam, int ram);		/* Multiply */
void jmp(int lam, int ram);		/* Jump */
void jeq(int lam, int ram);		/* Jump if eq */
void jnq(int lam, int ram);		/* Jump if not eq */
void jgt(int lam, int ram);		/* Jump if carry set */
void jlt(int lam, int ram);		/* Jump if carry NOT set */
void mov(int lam, int ram);		/* Move */
void cmp(int lam, int ram);		/* Compare, sets/clears "eq" and "c" flags */
void call(int lam, int ram);	/* Function call */
void ret(int lam, int ram);		/* Return from function call */
void push(int lam, int ram);	/* Push to stack */
void pop(int lam, int ram);		/* Pop from stack */
void pushu(int lam, int ram);	/* Push user registers to stack */
void popu(int lam, int ram);	/* Pop user registers from stack */
void irq(int lam, int ram);		/* Interrupt call */
void mod(int lam, int ram);		/* Modulo division call */

#define HLT 0 /* Halt instruction */

#define INSTRUCTION_COUNT 22

#ifdef MAIN_GLOBALS
t_instruction instructions[INSTRUCTION_COUNT] = {
	{ "HLT",	 0,	 1, hlt },
	{ "NOP",	 1,	 1, nop },
	{ "INC",	 2,	 2, inc },
	{ "DEC",	 3,  2, dec },
	{ "ADD",	 4,  3, add },
	{ "SUB",	 5,  3, sub },
	{ "MUL",	 6,  3, mul },
	{ "JMP",	 7,  2, jmp },
	{ "JEQ",	 8,  2, jeq },
	{ "JNQ",	 9,  2, jnq },
	{ "JGT",	 10, 2, jgt },
	{ "JLT",	 11, 2, jlt },
	{ "MOV",	 12, 3, mov },
	{ "CMP",	 13, 3, cmp },
	{ "PUSH",	 14, 2, push },
	{ "POP",	 15, 2, pop },
	{ "CALL",	 16, 2, call },
	{ "RET",	 17, 1, ret },
	{ "PUSHU",   18, 1, pushu },
	{ "POPU",	 19, 1, popu },
	{ "IRQ",     20, 2, irq },
	{ "MOD",	 21, 3, mod }
};
int debugprint=0;
#else
extern t_instruction instructions[INSTRUCTION_COUNT];
extern int debugprint;
#endif

#define INS_IMMEDIATE			0 /* Actually "immediate" or "don't care" */
#define INS_DEREF				1 /* Dereference value */
#define INS_ADDRDEREF			2 /* Double dereference value */

#define TOKTYPE_UNKNOWN 		0
#define TOKTYPE_DEREF 			101
#define TOKTYPE_STRING 			102
#define TOKTYPE_INTEGER 		103
#define TOKTYPE_LABEL			104
#define TOKTYPE_LABEL_REF		105
#define TOKTYPE_COMMENT			106
#define TOKTYPE_INS				107
#define TOKTYPE_REGISTER		108
#define TOKTYPE_VARIABLE		109
#define TOKTYPE_VARIABLE_REF	110
#define TOKTYPE_SIZEOF_VARIABLE 111
#define TOKTYPE_CHAR			112
#define TOKTYPE_REBASE			113

typedef struct _lToken
{
	int tokType;
	char *tokValue;
	int subTokType;	/* Only single level of subtokens, so this value embedded */
} lToken;

#ifdef MAIN_GLOBALS
lToken tokenArray[1000];
int tokenpos=0;
#else
extern lToken tokenArray[1000];
extern int tokenpos;
#endif

/* LABELS (interim) */
struct namepos {
	char *name;
	int position;
	int size; /* for variables */
};

typedef struct namepos LABEL;
typedef struct namepos VARIABLE;

/* All hardcoded at moment! */
#ifdef MAIN_GLOBALS
LABEL labels[1000];
LABEL labelreferences[1000];
int labelmax=0;
int labelrefmax=0;
VARIABLE variables[1000];
VARIABLE variablereferences[1000];
int variablemax=0;
int variablerefmax=0;
#else
extern LABEL labels[1000];
extern LABEL labelreferences[1000];
extern int labelmax;
extern int labelrefmax;
extern VARIABLE variables[1000];
extern VARIABLE variablereferences[1000];
extern int variablemax;
extern int variablerefmax;
#endif

/* Function prototypes */
int runtime(void);
void tokenize(char *fn);
void assemble(void);
void setlvrv(int lam, int ram, int opcode);

/* platform.c */
int posix_kbhit(void);

#define KEYPRESS_INTERRUPT 0 /* IRQ0 is the keypress interrupt */
#define OS_INTERRUPT 1 /* Operating system interrupt for system calls */
