#include "scanner.h"
#include "minic.tbl"
//#define NO_RULES 97
//#define GOAL_RULE (NO_RULES+1)
//#define NO_SYMBOLS 85
//#define NO_STATES 153
#define PS_SIZE 200

int ruleName[];
enum n_rep {terminal, nonterm};

class nodeType{
public:
	tokenType token;
	n_rep noderep;
	nodeType *son;
	nodeType *brother;
};
typedef nodeType Node;

FILE *astFile;

class ASTparser{
public:
	ASTparser();
	tokenType sc;
	int errcnt;
	int sp;
	int stateStack[PS_SIZE];
	int symbolStack[PS_SIZE];
	Node* valueStack[NO_SYMBOLS+1];

	void semantic(int n);
	Node* parser();
	void printToken(tokenType token);
	void dumpStack();
	void errorRecovery();
	int meaningfulToken(tokenType token);
	Node* buildTree(int nodeNumber, int rhsLength);
	Node* buildNode(tokenType token);
	void printNode(Node *pt, int ident);
	void printTree(Node *pt, int ident);
};

//Contructor
ASTparser::ASTparser(){
	errcnt = 0;
}

//Parser Function
Node* ASTparser::parser(){
	//Parsing table Create.
	extern int parsingTable[NO_STATES][NO_SYMBOLS+1];
	extern int leftSymbol[NO_RULES+1],rightLength[NO_RULES+1];

	int entry, ruleNumber, lhs;
	int currentState;
	tokenType token;
	Node *ptr;

	//initial state
	sp = 0; stateStack[sp] = 0;

	//scanner에서 token을 하나 가져온다.
	token = scanner();

	while(true){
		currentState = stateStack[sp];
		entry = parsingTable[currentState][token.number];
		
		//shift Action
		if(entry > 0){
			sp++;
			if(sp > PS_SIZE){
				cout << "critical complier error : parsing stack overflow";
				exit(1);
			}
			symbolStack[sp] = token.number;
			stateStack[sp] = entry;
			valueStack[sp] = meaningfulToken(token) ? buildNode(token) : NULL;
			token = scanner();
		}
		//Reduce Action
		else if(entry < 0){
			ruleNumber = -entry;
			if(ruleNumber == GOAL_RULE){
				if(errcnt == 0)
					cout << "*** valid source ***\n";
				//else
				//	cout << "*** error in source " << errcnt << endl;
				return valueStack[sp-1];
			}
//
			//semantic(ruleNumber);
			ptr = buildTree(ruleName[ruleNumber], rightLength[ruleNumber]);
			sp = sp - rightLength[ruleNumber];
			lhs = leftSymbol[ruleNumber];
			currentState = parsingTable[stateStack[sp]][lhs];
			sp++;
			symbolStack[sp] = lhs;
			stateStack[sp] = currentState;
			valueStack[sp] = ptr;
		}
		//Error Action
		else {
			cout << "=== error in source ===" << endl;
			errcnt++;
			cout << "Current Token : ";
			printToken(token);
			dumpStack();
			errorRecovery();
			token = scanner();
		}
	}	
}

//Reducing Print Message
void ASTparser::semantic(int n){
	cout<<"reduced rule number = "<<n<<endl;
}

//print Token
void ASTparser::printToken(tokenType token){
	if(token.number == tident)
		cout << token.value.id;
	else if(token.number == tnumber)
		cout << token.value.num;
	else
		cout << tokenName[token.number];
}

//Stack dump
void ASTparser::dumpStack(){
	int i, start;
	if(sp > 10)
		start = sp - 10;
	else
		start = 0;

	cout << endl <<" *** dump state stack : ";

	for(i = start; i <= sp ; ++i)
		cout << " " << stateStack[i];

	cout << endl << " *** dump symbol stack : ";

	for(i = start; i <= sp ; ++i)
		cout<< " " << symbolStack[i];
	cout << endl;
}

//Error Recovery Function
void ASTparser::errorRecovery(){
	tokenType tok;
	int parenthesisCount, braceCount;
	int i;


	//step1 : skip to the semicolon
	parenthesisCount = braceCount = 0;
	while(true){
		tok = scanner();
		if(tok.number == teof)
			exit(1);
		
		if(tok.number == tlparen)
			parenthesisCount++;
		else if(tok.number == trparen) 
			parenthesisCount--;

		if(tok.number == tlbrace)
			braceCount++;
		else if(tok.number == trbrace)
			braceCount--;

		if((tok.number == tsemicolon) && (parenthesisCount <= 0) && (braceCount <= 0))
			break;
	}
	
	//step2 : adjust state stack
	for(i=sp;i>=0;i--){
		//statement_list -> statement_list. statement
		if(stateStack[i]==36) break; //second statement part

		//statement_list -> . statement
		//statement_list -> . statement_list statement
		if(stateStack[i]==24) break; //first statement part

		//declaration_list -> declaration_list . declaration
		if(stateStack[i]==25) break; //second internal dcl

		//declaration_list -> . declaration
		//declaration_list -> . declaration_list declaration
		if(stateStack[i]==17) break; //internal declaration

		//external declaration
		//external_dcl -> . declaration
		if(stateStack[i]==2) break;  //after first external dcl
		if(stateStack[i]==0) break;  //first external declaration
	}
	sp=i;
}

//의미 있는 토큰인지 체크
int ASTparser::meaningfulToken(tokenType token){
	if((token.number == tident) || (token.number == tnumber))
		return 1;
	else
		return 0;
}

//노드를 만드는 메소드
Node* ASTparser::buildNode(tokenType token){
	Node* ptr;
	ptr = (Node *)malloc(sizeof(Node));
	if(!ptr){
		printf("malloc error in buildNode()\n");
		exit(1);
	}
	ptr->token = token;
	ptr->noderep = terminal;
	ptr->son = ptr->brother = NULL;
	return ptr;
}

//트리를 만드는 메소드
Node* ASTparser::buildTree(int nodeNumber, int rhsLength){
	int i, j, start;
	Node *first, *ptr;

	i = sp - rhsLength + 1;
	//step 1 : find a first index with node in value stack
	while(i <= sp && valueStack[i] == NULL)
		i++;
	if(!nodeNumber && i > sp)
		return NULL;
	start = i;

	//step 2 : linking brothers
	while( i <= sp -1){
		j = i + 1;
		while(j <= sp && valueStack[j] == NULL)
			j++;
		if(j <= sp){
			ptr = (Node *)(valueStack[i]);
			while(ptr->brother)
				ptr = ptr->brother;
			ptr->brother = valueStack[j];
		}
		i = j;
	}

	first = (start > sp) ? NULL : valueStack[start];

	//step 3 : making subtree root and linking son
	if(nodeNumber){
		ptr = (Node *) malloc(sizeof(Node));
		if(!ptr){
			printf("malloc eerror in buildTree()\n");
			exit(1);
		}

		ptr->token.number = nodeNumber;
		ptr->token.tokenValue = NULL;
		ptr->noderep = nonterm;
		ptr->son = first;
		ptr->brother = NULL;
		
		return ptr;
	}
	else
		return first;
}

//노드를 프린트하는 메소드
void ASTparser::printNode(Node *pt, int indent){
	extern FILE *astFile; int i;
	
	for(i = 1; i <= indent; i++)
		fprintf(astFile, " ");
	if(pt->noderep == terminal){
		if(pt->token.number == tident)
			fprintf(astFile, " Terminal : %s", pt->token.value.id);
		else if(pt->token.number == tnumber)
			fprintf(astFile, " Terminal : %d", pt->token.value.num);
	}
	else{
		int i;
		i = (int)(pt->token.number);
		fprintf(astFile, " Nonterminal : %s", nodeName[i]);
	}
	fprintf(astFile, "\n");
}

//트리를 프린하는 메소드
void ASTparser::printTree(Node *pt, int indent){
	Node *p = pt;
	while(p != NULL){
		printNode(p, indent);
		if(p->noderep == nonterm)
			printTree(p->son, indent+5);
		p = p->brother;
	}
}
	

void main(){
	astFile= fopen("perfect.ast", "w");	
	//create LRParser class
	ASTparser* lp = new ASTparser();
	
	//call the parser function
	lp->printTree(lp->parser(), 0);
	fclose(astFile);
}