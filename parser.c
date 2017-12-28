/* Lexer, fixed grammar */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

int gotinstruction = 0;

void removeChar(char *str, char toremove)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != toremove) dst++;
    }
    *dst = '\0';
}

/* Determine token type from token text */
int getTokenType(char *token)
{
	int i;

	if (strncmp(token, "REBASE", 6) == 0)
	{
		token = token + 6;
		return TOKTYPE_REBASE;
	}

	if (*token == '[')
	{
		removeChar(token, '[');
		removeChar(token, ']');
		return TOKTYPE_DEREF;
	}

	if (*token == ':') return (gotinstruction) ? TOKTYPE_LABEL_REF : TOKTYPE_LABEL;

	if (*token == ';')
		return TOKTYPE_COMMENT;

	if (*token == '\'')
		return TOKTYPE_CHAR;

	if (*token == '"') {
		removeChar(token, '"');
		return TOKTYPE_STRING;
	}

	if (*token == '{') {
		removeChar(token, '{');
		removeChar(token, '}');
		return TOKTYPE_SIZEOF_VARIABLE;
	}

	if (*token >= '0' && *token <= '9')
		return TOKTYPE_INTEGER;

	if ( ((strlen(token)) > 1) && (*token == 'R') && (*(token+1) >= '0') && (*(token+1) <= '9'))
		return TOKTYPE_REGISTER;

	if (*token == '$') return (gotinstruction) ? TOKTYPE_VARIABLE_REF : TOKTYPE_VARIABLE;

	for (i = 0; i < INSTRUCTION_COUNT; i++)
	{
		if (strncmp(instructions[i].name, token, 3) == 0) {
			gotinstruction=1;
			break;
		}
	}
	if (!gotinstruction) return TOKTYPE_UNKNOWN;

	return TOKTYPE_INS;
}

void addtoken(char *token)
{
	char *val;
	int tokType;

	if (!token || !*token) return;

	tokType = getTokenType(token);
	if (tokType == TOKTYPE_UNKNOWN)
	{
		printf("Got unknown token: '%s', exiting...\n", token);
		exit(-1);
	}
    val = strdup(token);
	tokenArray[tokenpos].tokType = tokType;
	tokenArray[tokenpos].tokValue = val;

	if (tokType == TOKTYPE_DEREF || tokType == TOKTYPE_SIZEOF_VARIABLE || tokType == TOKTYPE_REBASE)
	{
		tokType = getTokenType(token); // Do it again (previous call removed "[]")
		tokenArray[tokenpos].subTokType = tokType;
		val = strdup(token);
	}

	tokenpos++;
	*token = '\0';
}

int lexer(char *inputline)
{
	int tokencount=0;
	char token[1024];
	char *tokptr = token;
	int instring = 0, incomment = 0, inchar = 0;

	while(*inputline)
	{
		if (*inputline == '"') instring = !(instring);
		if (*inputline == '\'') inchar = !inchar;
		if (!instring && *inputline == ';') incomment = 1;

		if ((!instring && !incomment && !inchar) && (*inputline == ' ' || *inputline == '\t' || *inputline == ','))
		{
			*tokptr = '\0';
			addtoken(token);
			tokptr = token;
			inputline++;
		}
		else {
			if (!(*inputline == '\r') && !(*inputline == '\n')) {
				*tokptr = *inputline;
				tokptr++;
			}
			inputline++;
		}
	}

	/* Remaining token */
	*tokptr = '\0';
	addtoken(token);
	return tokencount;
}

/* Currently only supports "#include" */
int preprocessor(char *line)
{
	char locline[1024 +1];

	if (line[0] != '#') return 0;
	if (strlen(line) < 10) {
		printf("Unknown preprocessor statement found, exiting...\n");
		exit(-1);
	}
	strcpy(locline, line + strlen("#include "));
    removeChar(locline, '\r'); // Troublesome otherwise!
    removeChar(locline, '\n');
	removeChar(locline, '"');
	removeChar(locline, '\'');
	removeChar(locline, ' ');
	removeChar(locline, '\t');

	tokenize(locline);
	return 1;
}

void tokenize(char *fn)
{
	char line[1024 +1];

	FILE *fp = fopen(fn, "r");
	if (!fp) {
		printf("Failed to open input file: %s. EXITING...\n", fn);
		exit(-1);
	}

	while(fgets(line, 1024+1, fp))
	{
		if (preprocessor(line)) continue; /* Ignore rest of line if preprocessed */
		lexer(line);
		gotinstruction = 0;
	}

	fclose(fp);
	return;
}
