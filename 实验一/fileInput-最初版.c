#include<stdio.h>
#include<ctype.h>
#include<math.h>
#include<string.h>

#define LETTER  0       // 定义表示字母
#define DIGIT   1       // 定义表示数字
#define POINT   2       // 定义点
#define OTHER   3       // 定义其它字符
#define POWER   4       // 定义E或者e表示指数
#define PLUS    5       // 定义加号
#define MINUS   6       // 定义减号
#define UNDERLINE 7     // 定义下划线
#define OPERATOR 8      // 定义运算符
#define ROLLBACK 9      // 定义；和，表示的界限符
#define BORDER 10       // 定义其它的界限符

#define CLASSNUMBER 100     //Suppose the class of number is 100
#define CLASSOTHER 200      //Suppose the class of other is 200
#define CLASSBORDER 300     //Suppose the class of border is 300
#define CLASSOPERATOR 400   //Suppose the class of operator is 400
#define CLASSVAR 500        //Suppose the class of var is 500
#define CLASSDEFAULT 600    //Suppose the class of used var is 600
#define EndState -1

char usedWord[10][10] = {
    "unsigned", "break", "return", "void", "case", "float", "char", "for", "while", "continue", "if", "default", "do", "int", "switch", "double", "long", "else"
};

int w, n, p, e, d, i = 0, j = 0;
char varName[100];
char currentChar;
char otherOpertar[2];
int Class;            //Used to indicate class of the word
int ICON;
float FCON;
static int currentState;  //Used to present current state, the initial value:0

int GetChar(char input);
int EXCUTE(int, int);
int LEX(void);

// 判断是否为数字
int isDigit(char word) {
    if (word >= '0' && word <= '9') {
        return 1;
    }
    return 0;
}

// 判断是否为字母
int isWord(char word) {
    if ((word >= 'A' && word <= 'Z') || (word >= 'a' && word <= 'z')) {
        return 1;
    }
    return 0;
}

// 判断是否为除；或者，之外的界限符
int isBorder(char word) {
    if (word == '{' || word == '}' || word == '(' || word == ')') {
        return 1;
    }
    return 0;
}

// 判断是否为;, ，如果是，则跳转到初始状态
int isRollBack(char word) {
    if (word == ';' || word == ',' || word == ' ') {
        return 1;
    }
    return 0;
}

// 判断是否为运算符
int isOperator(char word) {
    if (word == '+' || word == '-' || word == '*' || word == '/' || word == '>' || word == '<' || word == '|'
        || word == '&' || word == '!' || word == '=') {
            return 1;
        }
    return 0;
}

// HandleWord
int HandleWord(void) {
    return CLASSOTHER;
}

// 错误处理
int HandleError(void) {
    printf("Error!\n");
    return 0;
}

// 提取一个字符并且返回其类别
int GetChar(char input) {
    if(input == 'E'|| input == 'e') {
      currentChar = input;
      return POWER;
    }
    if(isWord(input)) {
      currentChar = input;
      return LETTER;
    }
    if(isDigit(input)) {
      d=input-'0';
      return DIGIT;
    }
    if(input == '.') return POINT;
    if(input == '+') return PLUS;
    if(input == '-') return MINUS;
    if(input == '_') {
      currentChar = input;
      return UNDERLINE;
    }
    if(isOperator(input)) {
      currentChar=input;
      return OPERATOR;
    }
    if(isRollBack(input)) {
      currentChar=input;
      return ROLLBACK;
    }
    if(isBorder(input)) {
      currentChar=input;
      return BORDER;
    }
    return OTHER;
}

int EXCUTE(int state, int symbol) {
    switch(state) {
        case 0:
            switch(symbol){
                case DIGIT:
                    n=0;
                    p=0;
                    e=1;
                    w=d;
                    currentState=1;
                    Class=CLASSNUMBER;
                    break;
                case POINT:
                    w=0;
                    n=0;
                    p=0;
                    e=1;
                    currentState=3;
                    Class=CLASSNUMBER;
                    break;
                case POWER:
                    currentState=9;
                    varName[i]=currentChar;
                    i++;
                    Class=CLASSVAR;
                    break;
                case LETTER:
                    currentState=9;
                    varName[i]=currentChar;
                    i++;
                    Class=CLASSVAR;
                    break;
                case UNDERLINE:
                    currentState=9;
                    varName[i]=currentChar;
                    i++;
                    Class=CLASSVAR;
                    break;
                case OPERATOR:
                    currentState=8;
                    otherOpertar[j] = currentChar;
                    j++;
                    Class=CLASSOPERATOR;
                    break;
                case BORDER:
                    currentState=7;
                    Class=CLASSBORDER;
                    break;
                case ROLLBACK:
                    currentState=0;
                    Class=CLASSBORDER;
                    break;
                default:
                    HandleError();
                    Class=CLASSOTHER;
                    currentState=EndState;
            }
            break;
        case 1:
            switch (symbol){
                case DIGIT:
                    currentState=1;
                    w=w*10+d;
                    break; //CurrentState=1
                case POINT:
                    currentState=2;
                    break;
                case POWER:
                    currentState=4;
                    break;
                case UNDERLINE:
                case LETTER:
                    currentState=9;
                    printf("<%d, %d>", w, Class);
                    w = 0;
                    n = 0;
                    p = 0;
                    e = 0;
                    break;
                default:
                    ICON=w;
                    currentState=EndState;
            }
            break;
        case 2:
            switch (symbol){
                case DIGIT:
                    currentState=2;
                    n++;
                    w=w*10+d;
                    break;
                case POWER:
                    currentState=4;
                    break;
                default:
                    FCON=w*pow(10, e*p-n);
                    currentState=EndState;
            }
            break;
        case 3:
            switch (symbol){
            case DIGIT:
                n++;
                w=w*10+d;
                currentState=2;
                break;
            default :
                HandleError();
                Class=CLASSOTHER;
                currentState=EndState;
            }
            break;
        case 4:
            switch (symbol) {
                case DIGIT:
                    p=p*10+d;
                    currentState=6;
                    break;
                case MINUS:
                    e=-1;
                    currentState=5;
                    break;
                case PLUS:
                    currentState=5;
                    break;
                default:
                    HandleError();
                    Class=CLASSOTHER;
                    currentState=EndState;
            }
            break;
        case 5:
            switch(symbol){
                case DIGIT:
                    p=p*10+d;
                    currentState=6;
                    break;
                default:
                    HandleError();
                    Class=CLASSOTHER;
                    currentState=EndState;
            }
            break;
        case 6:
            switch(symbol){
                case DIGIT:
                    currentState=6;
                    p=p*10+d;
                    break;
                default:
                    FCON=w*pow(10,e*p-n);
                    currentState = EndState;
            }
            break;
        case 7:
            switch(symbol) {
                case POWER:
                    currentState=9;
                    break;
                case LETTER:
                    currentState=9;
                    break;
                case UNDERLINE:
                    currentState=9;
                    break;
                case PLUS:
                    currentState=8;
                    break;
                case MINUS:
                    currentState=8;
                    break;
                case OPERATOR:
                    currentState=8;
                    break;
                case DIGIT:
                    n=0;
                    p=0;
                    e=1;
                    w=d;
                    currentState=1;
                    Class=CLASSNUMBER;
                    break;
                case POINT:
                    w=0;
                    n=0;
                    p=0;
                    e=1;
                    currentState=3;
                    Class=CLASSNUMBER;
                    break;
                case BORDER:
                    currentState=7;
                    break;
                case ROLLBACK:
                    currentState=0;
                    break;
                default:
                    HandleError();
                    Class=CLASSOTHER;
                    currentState=EndState;
            }
            break;
        case 8:
            switch(symbol) {
                case POWER:
                    currentState=9;
                    break;
                case LETTER:
                    currentState=9;
                    break;
                case UNDERLINE:
                    currentState=9;
                    break;
                case PLUS:
                    currentState=8;
                    break;
                case MINUS:
                    currentState=8;
                    break;
                case OPERATOR:
                    currentState=8;
                    break;
                case DIGIT:
                    n=0;
                    p=0;
                    e=1;
                    w=d;
                    currentState=1;
                    Class=CLASSNUMBER;
                    break;
                case POINT:
                    w=0;
                    n=0;
                    p=0;
                    e=1;
                    currentState=3;
                    Class=CLASSNUMBER;
                    break;
                case BORDER:
                    currentState=7;
                    break;
                case ROLLBACK:
                    currentState=0;
                    break;
                default:
                    HandleError();
                    Class=CLASSOTHER;
                    currentState=EndState;
            }
            break;
        case 9:
            switch(symbol) {
                case POWER:
                    currentState=9;
                    break;
                case LETTER:
                    currentState=9;
                    break;
                case UNDERLINE:
                    currentState=9;
                    break;
                case PLUS:
                    currentState=8;
                    break;
                case MINUS:
                    currentState=8;
                    break;
                case OPERATOR:
                    currentState=8;
                    break;
                case DIGIT:
                    n=0;
                    p=0;
                    e=1;
                    w=d;
                    currentState=1;
                    Class=CLASSNUMBER;
                    break;
                case POINT:
                    w=0;
                    n=0;
                    p=0;
                    e=1;
                    currentState=3;
                    Class=CLASSNUMBER;
                    break;
                case BORDER:
                    currentState=7;
                    break;
                case ROLLBACK:
                    currentState=0;
                    break;
                default:
                    HandleError();
                    Class=CLASSOTHER;
                    currentState=EndState;
            }
            break;
    }
    return currentState;
}

int Lex(void){
    i = 0;
    int ch;
    char input;
    currentState=0;
    while(currentState != EndState){
        ch=GetChar(input);
        EXCUTE(currentState, ch);
    }
    return Class;
}

int main() {
    FILE* fp;
    char ch, filename[50];
    printf("please input file's name");
    scanf("%s", filename);
    fp = fopen(filename, "r");
    ch = fgetc(fp);
    while(ch != EOF) {
        putchar(ch);
        ch = fgetc(fp);
    }
    fclose(fp);
    return 0;
}
