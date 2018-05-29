#include <iostream>
#include <fstream>
#include <math.h>
#include <ctype.h>
#include <cstdlib>
#include <cstring>

using namespace std;

#define Max 655  //最大代码长度
#define WordMaxNum 256   //变量最大个数
#define DigitNum 256    //常量最大个数
#define MaxKeyWord 18  //关键字数量
#define MaxOptANum 9    //运算符最大个数
#define MaxOptBNum 6  //运算符最大个数
#define MaxEndNum 10    //界符最大个数

// 定义变量类型
enum finaltype {keyWord = 1, varWord = 2, intWord = 3, folatWord = 4, operatorAWord = 5, operatorBWord = 6, borderWord = 7, otherWord = 8};

enum errorType {VarExceed = 1, PointError = 2, ConExceed = 3};

typedef struct DisplayTable {
    int type;   //标识符的类型
    char symbol[20];    //标识符
}Table;

int TableNum = 0; 
bool errorFlag = 0;

// 定义关键字
const char* const KeyWord[MaxKeyWord] ={
    "unsigned", "break", "return", "void", "case", "float", "char", "for", "while", "continue", "if", "default", "do", "int", "switch", "double", "long", "else"
};     

// 定义单个符号运算
const char OptA[] = {
    '+', '-', '*', '/', '=', '#', '<', '>', '!'
}; 

// 定义双符号运算
const char* OptB[] = {
    "<=", ">=", "==", "!=", "&&", "||"
};

// 定义边界符号  
const char End[] = {
    '(', ')' , ',' , ';', '{' , '}', '"', '"', '[', ']'
};

/**
 * 判断是否以字母或者下划线开头
 */
bool beginWithPha(char index) {
    return (isalpha(index) || index == '_' ) ? true : false;
}

/**
 * 判断是否以数字开头
 */
bool beginWithDigit(char index) {
    return isdigit(index) ? true : false;
}

void error(char str[20], int nLine, int errorType);

void Scanner(char ch[], int chLen, Table table[Max], int nLine);

int main() {
    ifstream in;
    ofstream out;
    // 读入文件和写入文件的名称
    char in_file_name[26], out_file_name[26]; 
    // 存放输入代码的缓冲区
    char ch[Max];   
    // 初始化行数
    int nLine = 1;  
    Table* table = new Table[Max];
    int choice;

    cout << "请选择:" << endl; 
    cout << "1：从文件中读, " << endl;
    cout << "2：从键盘读(输入结束标志位#)" << endl;
    cin >> choice;

    switch(choice) {
        int i;
        /*从文件读取*/
        case 1:
            cout<<"Enter the input file name:\n";
            cin>>in_file_name;
            in.open(in_file_name);
            if(in.fail()) {
                cout<<"Inputput file opening failed.\n";
                exit(1);
            }
            cout<<"Enter the output file name:\n";
            cin>>out_file_name;
            out.open(out_file_name);
            if(out.fail()) {
                cout<<"Output file opening failed.\n";
                exit(1);
            }
            in.getline(ch, Max, '#');
            Scanner(ch, strlen(ch), table, nLine); //调用扫描函数
            if(errorFlag == 1) //出错处理
                return 0;
            // 把结果打印到各个txt文档中
            out <<"类型"<<"      "<<"下标"  <<endl;
            //打印display, 在文件testout.txt中输出
            for(i = 0; i < TableNum; i++) {
                out << "(";
                // enum finaltype {keyWord = 1, varWord = 2, intWord = 3, folatWord = 4, operatorAWord = 5, operatorBWord = 6, borderWord = 7, otherWord = 8};
                switch (table[i].type) {
                    case keyWord:
                        out << "关键字";
                        break;
                    case varWord:
                        out << "标识符";
                        break;
                    case intWord:
                        out << "整数";
                        break;
                    case folatWord:
                        out << "浮点数";
                        break;
                    case operatorAWord:
                        out << "操作符";
                        break;
                    case operatorBWord:
                        out << "操作符";
                        break;
                    case borderWord:
                        out << "边界符";
                        break;
                    default:
                        out << "其它";
                }
                out << "    ,   "<< table[i].symbol << ")" << endl;       
            }
                out << "(" <<  table[i].type << "    ,   "<< table[i].symbol << ")" << endl;
            break;
        // 从键盘输入的方式，输出到屏幕
        case 2:
            cin.getline(ch, Max, '#');
            cout << "信息输入完毕" << endl;
            Scanner(ch, strlen(ch), table, nLine); //调用扫描函数
            if(errorFlag == 1)
                return 0;
            cout << "\n 识别表： \n";
            cout << "类型" << "      " << "下标"  << endl; //dos界面下
            for(i = 0; i < TableNum; i++) {
                cout << "(";
                // enum finaltype {keyWord = 1, varWord = 2, intWord = 3, folatWord = 4, operatorAWord = 5, operatorBWord = 6, borderWord = 7, otherWord = 8};
                switch (table[i].type) {
                    case keyWord:
                        cout << "关键字";
                        break;
                    case varWord:
                        cout << "标识符";
                        break;
                    case intWord:
                        cout << "整数";
                        break;
                    case folatWord:
                        cout << "浮点数";
                        break;
                    case operatorAWord:
                        cout << "操作符";
                        break;
                    case operatorBWord:
                        cout << "操作符";
                        break;
                    case borderWord:
                        cout << "边界符";
                        break;
                    default:
                        cout << "其它";
                }
                cout << "    ,   "<< table[i].symbol << ")" << endl;                
            }
            break;
        default:
            cout << "error" << endl;
    }
    in.close();//关闭文件
    out.close();
    return 0;
}


/**
 * 定义错误类型
 **/
void error(char str[20], int nLine, int errorType) {
    cout <<"\nError :";
    switch(errorType) {
        case VarExceed:
            cout << "第" << nLine-1 << "行" << str << "变量的长度超过限制!\n";
            errorFlag = 1;
            break;
        case PointError:
            cout << "第" << nLine-1 <<"行" << str << "小数点错误!\n";
            errorFlag = 1;
            break;
        case ConExceed:
            cout << "第" << nLine-1 <<"行" << str << "常量的长度超过限制!\n";
            errorFlag = 1;
            break;
    }
}

/**
 * 词法扫描器
 * char ch[] 输入的字符串
 * clen chLen 输入的字符串的长度
 * nLine 默认的行数
 **/
void Scanner(char ch[], int chLen, Table table[Max], int nLine) {
    int chIndex = 0;
    cout << "chLen = " << chLen << endl;
    while(chIndex < chLen) {
        cout << "now is chIndex " << chIndex << endl;
        cout << "now char is " << ch[chIndex] << endl;
        // 忽略空格和tab
        while(ch[chIndex] == ' ' || ch[chIndex] == 9) { 
            chIndex++; 
        }
        // 遇到换行符，行数加1
        while(ch[chIndex] == 10) {   
            nLine++;
            chIndex++;
        }
        if(beginWithPha(ch[chIndex])) {
            char str[256];
            int strLen = 0;
            // 是字母、下划线            
            while(beginWithPha(ch[chIndex])) {
                cout << "----------------------------" << endl;
                str[strLen++] = ch[chIndex];
                chIndex++;
                // 不是第一位，可以为数字
                while(beginWithDigit(ch[chIndex])) {
                    str[strLen++] = ch[chIndex];
                    chIndex++;
                }
            }
            // 字符串结束符
            str[strLen] = 0;
            // 标识符超过规定长度，报错处理 
            if(strlen(str) > 20) {
                error(str, nLine, 1);
            } else {   
                int i;      
                //与关键字匹配, 如果是关键字，写入table表中
                for(i = 0; i < MaxKeyWord; i++) {
                    if(strcmp(str, KeyWord[i]) == 0) {
                        strcpy(table[TableNum].symbol, str);
                        table[TableNum].type = 1;  //关键字
                        TableNum++;
                        break;
                    }
                }
                //不是关键字
                if(i >= MaxKeyWord) {
                    table[TableNum].type = 2; //变量标识符
                    strcpy(table[TableNum].symbol, str);
                    TableNum++;
                }
            }
        }
        // 如果是以数字开头
        //else if(isdigit(ch[chIndex])&&ch[chIndex]!='0') //遇到数字
        else if(isdigit(ch[chIndex])) {
            int flag = 0;
            char str[256];
            int strLen = 0;
            //数字和小数点
            while(isdigit(ch[chIndex]) || ch[chIndex] == '.') {
                 // flag表记小数点的个数，0时为整数，1时为小数，2时出错
                if(ch[chIndex] == '.')  flag++;
                str[strLen++] = ch[chIndex];
                chIndex++;
            }
            str[strLen] = 0;
            //常量标识符超过规定长度20，报错处理
            if(strlen(str) > 20) {
                error(str, nLine, 3);
            }
            if(flag == 0) {
                table[TableNum].type = 3; //整数
                cout << "table[TableNum] = " << table[TableNum].symbol << endl;
            }
            if(flag == 1) {
                table[TableNum].type = 4; //小数
                cout << "table[TableNum] = " << table[TableNum].symbol << endl;
            }
            if(flag > 1) {
                error(str, nLine, 2);
            }
            strcpy(table[TableNum].symbol, str);
            cout << "table[TableNum] = " << table[TableNum].symbol << endl;
            TableNum++;
        } else {
            int errorFlag; //用来区分是不是无法识别的标识符，0为运算符，1为界符
            char str[3];
            str[0] = ch[chIndex];
            str[1] = ch[chIndex + 1];
            str[2] = 0;
            int i;
            for(i = 0; i < MaxOptBNum; i++)//MaxOptBNum)
                if(strcmp(str, OptB[i]) == 0) {
                    errorFlag = 0;
                    table[TableNum].type = 6;
                    strcpy(table[TableNum].symbol, str);
                    TableNum++;
                    chIndex = chIndex + 2;
                    break;
                }
            if(i >= MaxOptBNum) {
                for(int k = 0; k < MaxOptANum; k++)
                    if(OptA[k] == ch[chIndex]) {
                        errorFlag = 0;
                        table[TableNum].type = 5;
                        table[TableNum].symbol[0] = ch[chIndex];
                        table[TableNum].symbol[1] = 0;
                        TableNum++;
                        chIndex++;
                        break;
                    }
                // 界符
                for(int j = 0;j < MaxEndNum; j++)
                    if(End[j] == ch[chIndex]) {
                        errorFlag = 1;
                        table[TableNum].symbol[0] = ch[chIndex];
                        table[TableNum].symbol[1] = 0;
                        table[TableNum].type = 7;
                        TableNum++;
                        chIndex++;
                    }
                // 其他无法识别字符, 开头的不是字母、数字、运算符、界符
                if(errorFlag != 0 && errorFlag != 1) {
                    char str[256];
                    int strLen = -1;
                    str[strLen++] = ch[chIndex];
                    chIndex++;
                    while(*ch != ' ' || *ch != 9 || ch[chIndex] != 10) {
                        str[strLen++] = ch[chIndex];
                        chIndex++;
                    }
                    str[strLen] = 0;
                    table[TableNum].type = 8;
                    strcpy(table[TableNum].symbol, str);
                    TableNum++;
                }
            }
        }
    }
}