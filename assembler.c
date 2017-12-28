#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

#ifndef WIN32
#define _strdup strdup
#endif

void debugout(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if(debugprint)
		vprintf(format, args);
	va_end(args);
}

int getmop(int insloc, int tokloc)
{
	/* Get instruction and operands. b0: Instruction, b1: Addrmode left, b2: Addrmode right, b3: Not used */

	int leftmode = INS_IMMEDIATE, rightmode= INS_IMMEDIATE; /* INS_IMMEDIATE is default if not otherwise set */

	if (instructions[insloc].length == 1) return instructions[insloc].opcode; /* No change to opcode */

	if (instructions[insloc].length == 2)
	{
		if (tokenArray[tokloc +1].tokType == TOKTYPE_DEREF || tokenArray[tokloc +1].tokType == TOKTYPE_REGISTER)
			return instructions[insloc].opcode + (INS_DEREF << 8);
		else
			return instructions[insloc].opcode + (INS_IMMEDIATE << 8);
	}

	if (instructions[insloc].length == 3)
	{
		if (tokenArray[tokloc +1].tokType == TOKTYPE_REGISTER || tokenArray[tokloc +1].tokType == TOKTYPE_VARIABLE_REF)
			leftmode = INS_DEREF;
		else if (tokenArray[tokloc +1].tokType == TOKTYPE_DEREF)
			leftmode = INS_ADDRDEREF; /* Double dereference, only supported for registers currently, ie can't addrderef variables! */

		if (tokenArray[tokloc +2].tokType == TOKTYPE_REGISTER || tokenArray[tokloc +2].tokType == TOKTYPE_VARIABLE_REF)
			rightmode = INS_DEREF;
		else if (tokenArray[tokloc +2].tokType == TOKTYPE_DEREF)
			rightmode = INS_ADDRDEREF;

		return instructions[insloc].opcode + (leftmode << 8) + (rightmode << 16);
	}
	exit(-1); /* Illegal instruction length */
}

/* Assemble a token */
static void assemble_ins(int tokloc)
{
	int i, gotins=0;

	for (i=0; i < INSTRUCTION_COUNT; i++)
	{
		if (!strcmp(tokenArray[tokloc].tokValue, instructions[i].name)) {

			PCTARGET = getmop(i, tokloc); //instructions[i].opcode;
			debugout("Assembled instruction       : %s to %d\n", instructions[i].name, PCVALUE);
			gotins=1;
			break;
		}
	}
	if (!gotins) {
		debugout("Found unknown instruction '%s', exiting...\n", tokenArray[tokloc].tokValue);
		exit(-1);
	}
	PCVALUE++;
}

/* Assemble character */
static void assemble_char(int ins)
{
	char cand;
	char *ch = tokenArray[ins].tokValue;

	if (*ch == '\'') ch++;
	if (*(ch+1) == '\'') *(ch+1) = '\0';

	if (*ch == '\\')
		cand = '\n'; /* Only support \n at moment */
	else
		cand = *ch;

	debugout("Assembled char literal      : '%c' to %d\n", cand, PCVALUE);
	PCTARGET = cand;
	wordcount++;
	PCVALUE++;
}

static void assemble_int(int ins)
{
	debugout("Assembled int literal       : '%s' to %d\n", tokenArray[ins].tokValue, PCVALUE);
	PCTARGET = atoi(tokenArray[ins].tokValue);
	wordcount++;
	PCVALUE++;
}

/* Assemble string, - doesn't support control characters yet */
static void assemble_string(int ins)
{
	int i;
	char *ch = tokenArray[ins].tokValue;
	char *ch2 = tokenArray[ins].tokValue;
	int len = strlen(ch);
	int startpos = PCVALUE;

	for (i=0; i < len; i++)
	{
		PCTARGET = *ch;
		ch++;
		wordcount++;
		PCVALUE++;
	}
	PCTARGET = '\0';
	wordcount++;
	PCVALUE++;
	debugout("Assembled string            : \"%s\" from %d to %d\n", ch2, startpos, PCVALUE);
}

/* Add label to label list! */
static void assemble_label(int ins)
{
	labels[labelmax].name = tokenArray[ins].tokValue;
	labels[labelmax].position = PCVALUE;
	debugout("Added LABEL                 : '%s' to %d\n", tokenArray[ins].tokValue, PCVALUE);
	labelmax++; 	/* DON'T Increment pc! */
}

static void assemble_label_ref(char *ch)
{
	int i;
	for(i=0; i < labelmax; i++)
	{
		if (strcmp(labels[i].name, ch) ==0)
		{
			PCTARGET = labels[i].position; /* Insert reference to correct memory location */
			PCVALUE++;
			debugout("Inserted LABEL REF          : '%s' to %d\n", ch, PCVALUE);
			return;
		}
	}
	debugout("ERROR: Could not find LABEL for REF '%s'\n", ch);
	exit(-1);
}

static int assemble_label_refs(void)
{
	int i=0, j=0;
	for(i=0; i < labelrefmax; i++)
	{
		PCVALUE = labelreferences[i].position;
		assemble_label_ref(labelreferences[i].name);
		j++;
		wordcount++;
	}
	return j;
}

static void assemble_variable_ref(char *ch)
{
	int i;

	for(i=0; i < variablemax; i++)
	{
		if (strcmp(variables[i].name, ch) == 0)
		{
			/* Insert reference to correct memory location */
			PCTARGET = variables[i].position;
			PCVALUE++;
			debugout("Inserted VARIABLE REF       : '%s' to %d\n", ch, PCVALUE);
			return;
		}
	}
	debugout("ERROR: Could not find VARIABLE for REF '%s'\n", ch);
	exit(-1);
}

static int assemble_variable_refs(void)
{
	int i=0, j=0;
	for(i=0; i < variablerefmax; i++)
	{
		PCVALUE = variablereferences[i].position;
		assemble_variable_ref(variablereferences[i].name);
		j++;
	}
	return j;
}

static void assemble_variable(int *ins)
{
	int varlen=1;
	if (tokenArray[(*ins)+1].tokType == TOKTYPE_SIZEOF_VARIABLE) /* Multi-element variable */
	{
		varlen = atoi(tokenArray[(*ins)+1].tokValue);
		*ins = *ins + 1; /* Skip next token! */
	}

	variables[variablemax].name = _strdup(tokenArray[*ins].tokValue);
	variables[variablemax].position = PCVALUE;
	variables[variablemax].size = varlen;

	debugout("Added VARIABLE '%s' of size %d to %d\n", tokenArray[*ins].tokValue, variables[variablemax].size, PCVALUE);
	PCVALUE+=varlen; /* DO increment pc, (based on variable size)! */
	variablemax++;
}

static int GetReg(char *tok)
{
	if (strlen(tok) == 2 || strlen(tok) == 3) {
		tok++;
		return atoi(tok);
	}
	debugout("Could not get register number (exiting)...\n");
	exit(-1);
}

static void assemble_rebase(int *ins)
{
	static int previous_rebase = STARTPOS;
	int val = atoi(tokenArray[(*ins)+1].tokValue);

	if (val < previous_rebase || val < PCVALUE)
	{
		debugout("Cannot rebase to PREVIOUS location (Previous rebase: %d, PC: %d, new location: %d)\n", previous_rebase, PCVALUE, val);
		exit(-1);
	}
	debugout("REBASE moved from           : %d to %d\n", PCVALUE, val);
	previous_rebase = val;
	PCVALUE = val;
	*ins = *ins + 1; /* Skip next instruction */
}

static void assemble_register(int ins)
{
	PCTARGET = GetReg(tokenArray[ins].tokValue);
	debugout("Assembled REGISTER          : '%s' to %d\n", tokenArray[ins].tokValue, PCVALUE);
	wordcount++;
	PCVALUE++;
}

/* When we find label reference, add it to list (and then process list after assembly) */
static void pospone_label_ref(int ins)
{
	labelreferences[labelrefmax].name = _strdup(tokenArray[ins].tokValue);
	labelreferences[labelrefmax].position = PCVALUE;
	labelrefmax++;
	PCVALUE++; /* Leave hole to stuff label address in later */
}

/* When we find variable reference, add it to list (and then process list after assembly) */
static void pospone_variable_ref(int ins)
{
	variablereferences[variablerefmax].name = _strdup(tokenArray[ins].tokValue);
	variablereferences[variablerefmax].position = PCVALUE;
	variablerefmax++;
	PCVALUE++; /* Leave hole to stuff variable address in */
}

void createvariable(char *name, int location)
{
	variables[variablemax].name = _strdup(name);
	variables[variablemax].position = location; /* Memory location for outportc */
	debugout("Created VARIABLE            : '%s' to %d\n", name, location);
	variablemax++;
}

void createvariables()
{
	int i;
	char varname[20];
	createvariable("$OUTPORTC", OUTPORTC_LOCATION);
	createvariable("$OUTPORTI", OUTPORTI_LOCATION);
	createvariable("$INPORTC", INPORTC_LOCATION);

	for(i=0; i < 16; i++) /* Create I01 to I15 for interrupt vectors */
	{
		snprintf(varname, sizeof(varname), "$I%02d", i);
		createvariable(varname, VECTOR_BASE + i);
	}
}

/* Assemble from token stream */
void assemble()
{
	int i, j;

	PCVALUE = STARTPOS;
	memory[STARTPOS] = HLT; /* Just in case nothing is assembled */
	createvariables();

	for(i=0; i < tokenpos; i++)
	{
		switch (tokenArray[i].tokType)
		{
		case TOKTYPE_INS:			assemble_ins(i);			break;
		case TOKTYPE_CHAR:			assemble_char(i);			break;
		case TOKTYPE_INTEGER:		assemble_int(i);			break;
		case TOKTYPE_STRING:		assemble_string(i);			break;
		case TOKTYPE_LABEL:			assemble_label(i);			break;
		case TOKTYPE_LABEL_REF:		pospone_label_ref(i);		break;
		case TOKTYPE_VARIABLE:		assemble_variable(&i);		break;
		case TOKTYPE_VARIABLE_REF:	pospone_variable_ref(i);	break;
		case TOKTYPE_REGISTER:		assemble_register(i);		break;
		case TOKTYPE_DEREF:			assemble_register(i);		break; /* Only support deref for registers currently! */
		case TOKTYPE_REBASE:		assemble_rebase(&i);		break;
		}
	}

	debugout("\nAssembled %d tokens, (used %d memory words)\n", tokenpos, PCVALUE - STARTPOS);

	/* Insert label positions! */
	debugout("Inserting label references...\n");
	j = assemble_label_refs();
	debugout("Inserted %d label references.\n\n", j);

	/* Insert variable positions */
	debugout("Inserting variable references...\n");
	j = assemble_variable_refs();
	debugout("Inserted %d variable references.\n", j);
	debugout("Assembly complete\n");
}
