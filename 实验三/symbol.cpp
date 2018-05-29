#include<iostream>

#include<iomanip>

#include<vector>
#include<stack>
#include<map>
#define N 100

using namespace std;

// 算符和数字映射表
map<char, int> theMap;

// 算符优先表
char table[N][N];

// 符号栈
stack<char> symbol;

// 最左素短语
map<int, vector<char> > sentence;

// 输入串
string input;

// 算符优先表的初始化
void tableInit();

// 映射初始化
void initMap();

// 分析程序
void analytical();

// 打印stack中的元素
void printVector();

int main() {
    initMap();
    tableInit();
    cout << "please input the string: " << endl;
    cin >> input;
    analytical();
    return 0;
}

void tableInit() {
    table[1][1] = '>';
    table[1][2] = '<';
    table[1][3] = '<';
    table[1][4] = '<';
    table[1][5] = '>';   

    table[2][1] = '>';
    table[2][2] = '>';
    table[2][3] = '<';
    table[2][4] = '@';
    table[2][5] = '>';  

    table[3][1] = '>';
    table[3][2] = '>';
    table[3][3] = '@';
    table[3][4] = '@';
    table[3][5] = '>';  

    table[4][1] = '<';
    table[4][2] = '<';
    table[4][3] = '<';
    table[4][4] = '@';
    table[4][5] = '=';  

    table[5][1] = '>';
    table[5][2] = '>';
    table[5][3] = '@';
    table[5][4] = '@';
    table[5][5] = '>';  
}

void initMap() {
    theMap['+'] = 1;
    theMap['*'] = 2;
    theMap['i'] = 3;
    theMap['('] = 4;
    theMap[')'] = 5;
    map<char, int>::iterator it;
    it = theMap.begin();
    while (it != theMap.end()) {
        cout << it->first << "  " << it->second << endl;
        it++;
    }
}

void analytical() {
    // 符号栈'#' 首先入栈
    symbol.push('#');
    cout << "input = " << input << endl;
    cout << "size = " << input.size() << endl;
    for (int j = 0; j < input.size(); j++) {
        cout << input[j] << "   ";
    }
    cout << endl;
    input = input + '#';
    int i;
    for (i = 0; i < input.size(); ) {
        cout << "size = " << symbol.size() << " " << "top = " << symbol.top();
        cout << "   " << setw(10) << input.substr(i, input.length());
        if (symbol.top() == '#') {
            symbol.push(input[i]);
            cout << endl;
            i++;
        } else {
            char simple = table[theMap[symbol.top()]][theMap[input[i]]];
            cout << "       " << symbol.top() << "   " << simple  << "   "<< input[i] << endl;
            if (simple == '@') {
                break;
            }
            if (simple == '<' || simple == '=') {
                symbol.push(input[i]);
                i++;
            } else {
                char temp = symbol.top();
                do {
                    symbol.pop();
                } while (symbol.top() >= 'A' && symbol.top() <= 'Z' && symbol.top() != '#' || table[theMap[symbol.top()]][theMap[temp]] == '>' || table[theMap[symbol.top()]][theMap[temp]] == '=');
                // symbol.push('V');
                while (table[theMap[symbol.top()]][theMap[input[i]]] == '>') {
                    symbol.pop();
                }
            }
        }
    }

    cout << symbol.top() << endl;
    cout << symbol.size() << endl;

    if (i == input.size() && symbol.top() == '#') {
        cout << "识别成功!" << endl;   
        printVector();     
    } else {
        cout << "error!" << endl;
    }
}

void printVector() {
    map<int, vector<char> >::iterator it;
    it = sentence.begin();
    while (it != sentence.end()) {
        vector<char> result = it -> second;
        for (int i = 0; i < result.size(); i++) {
            cout << result[i];
        }
        cout << endl;
        it++;
    }
}