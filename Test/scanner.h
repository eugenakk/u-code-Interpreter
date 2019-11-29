#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define NO_KEYWORDS 7
#define ID_LENGTH 12

enum tsymbol { tnull = -1,
	tnot, tnotequ, tmod, tmodAssign, tident, tnumber,
/*   0       1       2      3           4       5    */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,		
/*    6      7        8      9          10      11   */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,		
/*   12       13         14      15     16       17  */
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,	
/*    18      19         20         21     22       23  */
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,	
/*    24      25       26        27        28       29   */
	tconst, telse, tif, tint, treturn, tvoid,				
/*    30      31    32    33    34       35    */  
	twhile, tlbrace, tor, trbrace							
/*    36       37     38    39    */				
};

char *tokenName[]={
	//0			1			2		3		4			5
	"!",		"!=",		"%",	"%=",	"%ident",	"%number",
	//6			7			8		9		10			11
	"&&",		"(",		")",	"*",	"*=",		"+",
	//12		13			14		15		16			17
	"++",		"+=",		",",	"-",	"--",		"-=",
	//18		19			20		21		22			23
	"/",		"/=",		";",	"<",	"<=",		"="
	//24		25			26		27		28			29
	"==",		">",		">=",	"[",	"]",		"eof",
	//********************** word symbols****************************
	//30		31			32		33		34			35	
	"const",	"else",		"if",	"int",	"return",	"void",
	//36		37			38		39
	"while",	"{",		"||",	"}"
};


enum tsymbol tnum[NO_KEYWORDS] = {
	tconst, telse, tif, tint, treturn, tvoid, twhile
};

char *keyword[NO_KEYWORDS] = {
	"const", "else", "if", "int", "return", "void", "while"
};




enum nodeNumber{
		ACTUAL_PARAM,	ADD,						ADD_ASSIGN,			ARRAY_VAR,			ASSIGN_OP,
		CALL,					COMPOUND_ST,		CONST_NODE,		DCL,						DCL_ITEM,
		DCL_LIST,			   DCL_SPEC,				DIV,					DIV_ASSIGN,				EQ,
		ERROR_NODE,			EXP_ST,					FORMAL_PARA,	FUNC_DEF,				FUNC_HEAD,
		GE,							GT,							IDENT,					IF_ELSE_ST,				IF_ST,
		INDEX,					INT_NODE,				LE,							LOGICAL_AND,		LOGICAL_NOT,
		LOGICAL_OR,			LT,							MOD,					MOD_ASSIGN,				MUL,
		MUL_ASSIGN,			NE,							NUMBER,				PARAM_DCL,			POST_DEC,
		POST_INC,			PRE_DEC,				PRE_INC,				PROGRAM,			RETURN_ST,
		SIMPLE_VAR,			STAT_LIST,				SUB,					SUB_ASSIGN,			UNARY_MINUS,
		VOID_NODE,			WHILE_ST
};

char *nodeName[] = {
	"ACTUAL_PARAM", "ADD",					"ADD_ASSIGN",		"ARRAY_VAR",		"ASSIGN_OP",
		"CALL",					"COMPUND_ST",	"CONST_NODE",		"DCL",					"DCL_ITEM", 
		"DCL_LIST",				"DCL_SPEC",			"DIV",						"DIV_ASSIGN",		"EQ",
		"ERROR_NODE",		"EXP_ST",			"FORMAL_PARA",		"FUNC_DEF",			"FUNC_HEAD",
		"GE",						"GT",					"IDENT",					"IF_ELSE_ST",		"IF_ST", 
		"INDEX",					"INT_NODE",			"LE",						"LOGICAL_AND", "LOGICAL_NOT",
		"LOGICAL_OR",			"LT",					"MOD",					"MOD_ASSIGN",	"MUL",
		"MUL_ASSIGN",			"NE",				"NUMBER",					"ARAM_DCL",		 "POST_DEC", 
		"POST_INC",				"PRE_DEC",		 "PRE_INC",						"PROGRAM",		"RETURN_ST", 
		"SIMPLE_VAR",		"STAT_LIST",			"SUB",                      "SUB_ASSIGN",    "UNARY_MINUS",
		"VOID_NODE",		"WHILE_ST"
};

int ruleName[] = {
	0,	PROGRAM,	0,	0,	0,
	0,	FUNC_DEF,	FUNC_HEAD,	DCL_SPEC,	0,
	0,	0,	0,	CONST_NODE,	INT_NODE,
	VOID_NODE,	0,	FORMAL_PARA,	0,	0,
	0,	0,	PARAM_DCL,	COMPOUND_ST,	DCL_LIST,
	DCL_LIST,	0,	0,	DCL,	0,
	0,	DCL_ITEM,	DCL_ITEM,	SIMPLE_VAR,	ARRAY_VAR,
	0,	0,	STAT_LIST,	0,	0,
	0,	0,	0,	0,	0,
	0,	EXP_ST,	0,	0,	IF_ST,
	IF_ELSE_ST,	WHILE_ST,	RETURN_ST,	0,	0,
	ASSIGN_OP,	ADD_ASSIGN,	SUB_ASSIGN,	MUL_ASSIGN,	DIV_ASSIGN,
	MOD_ASSIGN,	0,	LOGICAL_OR,	0,	LOGICAL_AND,
	0,	EQ,	NE,	0,	GT,
	LT,	GE,	LE,	0,	ADD,
	SUB,	0,	MUL,	DIV,	MOD,
	0,	UNARY_MINUS,	LOGICAL_NOT,	PRE_INC,	PRE_DEC,
	0,	INDEX,	CALL,	POST_INC,	POST_DEC,
	0,	0,	 ACTUAL_PARAM,	0,	0,
	0,	0,	0
};

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");

	switch(n) {
		case 1 : printf("an identifier length must be less than 12.\n");	break;
		case 2 : printf("next character must be &.\n");						break;
		case 3 : printf("next character must be |.\n");						break;
		case 4 : printf("invalid character!!!\n");							break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch)
{
	if(isalnum(ch) || ch == '_') return 1;
	else return 0;
}

int hexValue(char ch)
{
	switch(ch)
	{
		case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9': 
				return(ch - '0');
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': 
				return(ch - 'A' + 10);
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
				return(ch - 'a' + 10);
		default: return - 1;
	}
}

int getIntNum(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;

	if(firstCharacter != '0')						//decimal
	{
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = getchar();
		} while(isdigit(ch));
	}
	
	else
	{
		ch = getchar();
		
		if((ch>= '0') && (ch <= '7'))				// octal
			do {
				num = 8*num + (int)(ch-'0');
				ch = getchar();
			} while ((ch >= '0') && (ch <= '7'));
			
			else if ((ch == 'X') || (ch == 'x'))	// hexa decimal
			{
				while ((value = hexValue(ch=getchar())) != -1)
					num = 16 * num + value;
			}
			
			else num = 0;							// zero
	}
	
	ungetc(ch, stdin);
	return num;
}

struct tokenType
{
	int number;
	union{
		char id[ID_LENGTH];
		int num;
	} value;
	char* tokenValue;
};

struct tokenType scanner()
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];

	token.number = tnull;

	do {
		
		while(isspace(ch = getchar()));
		
		if(superLetter(ch))
		{
			i=0;
			
			do {
				if(i<ID_LENGTH) id[i++] = ch;
				ch = getchar();
			} while(superLetterOrDigit(ch));
			
			if(i>=ID_LENGTH) lexicalError(1);

			id[i] = '\0';
			ungetc(ch,stdin);
			
			for(index=0; index < NO_KEYWORDS; index++)
				if(!strcmp(id, keyword[index])) break;
				
				if(index < NO_KEYWORDS)
				{
					token.number = tnum[index];
					strcpy(token.value.id, keyword[index]);
				}
				
				else
				{
					token.number = tident;
					strcpy(token.value.id, id);
				}
		}
		
		else if(isdigit(ch))
		{
			token.number = tnumber;
			token.value.num = getIntNum(ch);
		}
		
		else 
		{
			switch(ch)
			{
				case '/': 
				ch = getchar(); 
	
				if(ch == '*')
					do{
						while(ch != '*') ch = getchar();
						ch = getchar();
					} while (ch != '/');
				
				else if(ch =='/')
					while(getchar() != '\n');
		
				else if(ch == '=') 
				{
					token.number = tdivAssign;
					strcpy(token.value.id, "/=");
				}
					
				else
				{ 
					token.number = tdiv;
					strcpy(token.value.id, "/");
					ungetc(ch, stdin); // retract
				}
				break;
				
			case '!':
				ch = getchar();

				if(ch == '=') 
				{
					token.number = tnotequ;
					strcpy(token.value.id, "!=");
				}
			
				else
				{   
					token.number = tnot;
					strcpy(token.value.id, "!");
					ungetc(ch, stdin); // retract
				}
				break;
	
			case '%':
				ch = getchar();
			
				if(ch == '=') 
				{
					token.number = tmodAssign;
					strcpy(token.value.id, "%=");
				}
	
				else
				{   
					token.number = tmod;
					strcpy(token.value.id, "%");
					ungetc(ch, stdin); // retract
				}
				break;

			case '&':
				ch = getchar();
			
				if(ch == '&') 
				{
					token.number = tand;
					strcpy(token.value.id, "&&");
				}
			
				else
				{   
					lexicalError(2);
					ungetc(ch, stdin); // retract
				}
				break;
			
			case '*':
				ch = getchar();
			
				if(ch == '=') 
				{
					token.number = tmulAssign;
					strcpy(token.value.id, "*=");
				}
			
				else
				{   
					token.number = tmul;
					strcpy(token.value.id, "*");
					ungetc(ch, stdin); // retract
				}
				break;

			case '+':
				ch = getchar();
			
				if(ch == '+') 
				{
					token.number = tinc;
					strcpy(token.value.id, "++");
				}
			
				else if (ch == '=') 
				{
					token.number = taddAssign;
					strcpy(token.value.id, "+=");
				}
			
				else
				{   
					token.number = tplus;
					strcpy(token.value.id, "+");
					ungetc(ch, stdin); // retract
				}
				break;
			
			case '-':
				ch = getchar();
			
				if(ch == '-') 
				{
					token.number = tdec;
					strcpy(token.value.id, "--");
				}
			
				else if(ch == '=') 
				{
					token.number = tsubAssign;
					strcpy(token.value.id, "-=");
				}
			
				else
				{   
					token.number = tminus;
					strcpy(token.value.id, "-");
					ungetc(ch, stdin); // retract
				}
				break;
		
			case '<':
				ch = getchar();
			
				if(ch == '=') 
				{
					token.number = tlesse;
					strcpy(token.value.id, "<=");
				}
			
				else
				{   
					token.number = tless;
					strcpy(token.value.id, "<");
					ungetc(ch, stdin); // retract
				}
				break;

			case '=':
				ch = getchar();
			
				if(ch == '=') 
				{
					token.number = tequal;
					strcpy(token.value.id, "==");
				}
			
				else
				{   
					token.number = tassign;
					strcpy(token.value.id, "=");
					ungetc(ch, stdin); // retract
				}
				break;
		
			case '>':
				ch = getchar();

				if(ch == '=') 
				{
					token.number = tgreate;
					strcpy(token.value.id, ">=");
				}
			
				else
				{   
					token.number = tgreat;
					strcpy(token.value.id, ">");
					ungetc(ch, stdin); // retract
				}
				break;
			
			case '|':
				ch = getchar();
			
				if(ch == '|') 
				{
					token.number = tor;
					strcpy(token.value.id, "||");
				}
			
				else
				{   
					lexicalError(3);
					ungetc(ch, stdin); // retract
				}
				break;
			
			case '(' : token.number = tlparen; 
				strcpy(token.value.id, "(");
				break;

			case ')' : token.number = trparen; 
				strcpy(token.value.id, ")");
				break;
			
			case ',' : token.number = tcomma; 
				strcpy(token.value.id, ",");
				break;
			
			case ';' : token.number = tsemicolon; 
				strcpy(token.value.id, ";");
				break;
			
			case '[' : token.number = tlbracket; 
				strcpy(token.value.id, "[");  
				break;
			
			case ']' : token.number = trbracket; 
				strcpy(token.value.id, "]");  
				break;

			case '{' : token.number = tlbrace;
				strcpy(token.value.id, "{");  
				break;
			
			case '}' : token.number = trbrace;
				strcpy(token.value.id, "}");  
				break;
			
			case EOF : token.number = teof; 
				break;
			
			default:
				{
				printf("Current character : %c", ch);
				lexicalError(4);
				break;
				}
			} // switch end 
		}
	} while (token.number == tnull);

	return token;
} // end of scanner