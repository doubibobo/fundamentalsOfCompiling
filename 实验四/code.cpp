#include<iostream>
#include<fstream>

#include<string>

#include<cstring>
#include<cmath>

#include<algorithm>

#include<vector>
#include<stack>
#include<map>
#include<set>

using namespace std;

int const length = 100;

// 文法中出现的非终结符对应的int数据类型
map<char, int> VN;

// 文法中出现的终结符对应的int数据类型
map<char, int> VT;

// 从文件中读取的没有初始化的文法（利用前面实验读取文件的模块）
map<char, vector<string> > input;

// 经过处理后的文法
vector<string> wf[length];

// 文法中加了活前缀的句型
struct Node {
    int which;  // 知道是上述第几条
    int number;	// 知道是上述第几条的哪一个
    int where;  // 知道活前缀在文法中的位置
    char next;  // 活前缀后面的字符，用来判断状态转移
    bool flag;   // 是否为可规约活前缀，默认为0不可规约
};

// 文法所有经过活前缀处理的句型
struct Node allNode[length];

// 状态栈
stack<int> state;

// 符号栈
stack<char> symbol;

// 输入串
string waitForAnalysis;

// 表示DFA中每一个状态的每一个元素
struct Linked {
    Node* own;
    Linked* theNext;
};

// 表示DFA中的每一个状态
struct Simple {
    char word;      // 识别的符号
    Linked* first;  // 第一个活前缀项目
};

// DFA
struct Simple DFA[length];   //N 表示状态集合编号

// new DFA，使用set的目的是去重
set<set<int> > newDFA;

// 构造LR(0)文法分析表
char LR0[length][length];

// 所有文法的长度
int wfLength = 0;

// 所有活前缀文法数目
int liveLength = 0;

// DFA中状态的数目
int stateNumber = 0;

// 从文件中读取文法并进行文法的拓展
void readWenfaFromFile()
{
    string content;
    string cut;
    int temp;
    char first;
    int i = 0;
    string tempString;
    ifstream inf("D:\\wf.txt");
    if (!inf.is_open()) {
        cout << "没有成功打开文件" << endl;
    }
    while (getline(inf, content)) {
        //　截取－＞两端的字符串，　左边一定为非终结符，右边可以是终结符，　也可以为非终结符或者其组合
        bool flag = true;
        i++;
        vector<string> value;
        // Linux 和 C++ 有所不同
        // content.erase(--content.end());
        cout << "content = " << content << endl;
        cut = content.substr(3);
        temp = cut.find('|');
        if (temp < 0 || cut.length() <= temp) {
            value.push_back(cut);
            flag = false;
        }
        while (flag) {
            tempString = cut.substr(0, temp);
            cut = cut.substr(temp+1);
            value.push_back(tempString);
            temp = cut.find('|');
            if (temp >= cut.length() || temp < 0) {
                value.push_back(cut);
                flag = false;
            }
        }
        input.insert(make_pair(content[0], value));
        cout << "size of VN = " << VN.size() << endl;
        VN.insert(make_pair(content[0], VN.size()));
        if (i == 1) {
            first = content[0];
            cout << "first = " << first << endl;
        }
    }
    // 进行文法拓展
    string some = "X";
    some[0] = first;
    vector<string> value;
    value.push_back(some);
    input.insert(make_pair('X', value));
    VN.insert(make_pair('X', VN.size()));
    inf.close();
}

// 构造活前缀文法同时改变存储文法的数据结构（相当于增加了编号），顺便取得所有的终结符
void toSimpleWenfa()
{
    map<char, vector<string> >::iterator iter;
    iter = input.begin();
    while (iter != input.end())
    {

        cout << "size = " << (iter -> second).size() << endl;
        wf[VN[iter -> first]] = iter -> second;
        // 初始化其活前缀文法
        for (int i = 0; i < wf[VN[iter -> first]].size(); i++)
        {
            string value = wf[VN[iter -> first]][i];
            cout << "value = " << value << endl;
            for (int j = 0; j < value.length(); j++)
            {
                allNode[liveLength+j].which = VN[iter -> first];
                allNode[liveLength+j].number = i;
                allNode[liveLength+j].where = j;
                allNode[liveLength+j].next = value[j];
                allNode[liveLength+j].flag = false;
                // 收集终结符
                if (value[j] >= 'a' && value[j] <= 'z' && VT.count(value[j]) == 0)
                {
                    VT.insert(make_pair(value[j], VT.size()));
                }
            }
            allNode[liveLength+value.length()].which = VN[iter -> first];
            allNode[liveLength+value.length()].number = i;
            allNode[liveLength+value.length()].where = value.length();
            allNode[liveLength+value.length()].next = '$';
            allNode[liveLength+value.length()].flag = true;
            liveLength += value.length()+1;
        }
     //    vector<string> value = iter -> second;
        cout << "wf.size = " << (wf[VN[iter->first]]).size() << endl;
        wfLength += (iter -> second).size();
        iter++;
    }
}

// 合并两个set集合中的元素
set<int> mergeToSet(set<int> first, set<int> second)
{
    set<int> result;
    set<int>::iterator firstIter = first.begin();
    set<int>::iterator secondIter = second.begin();
    while (firstIter != first.end())
    {
        // cout << "firstIter = " << *firstIter << endl;
        result.insert(*firstIter);
        firstIter++;
    }
    while (secondIter != second.end())
    {
        // cout << "secondIter = " << *secondIter << endl;
        result.insert(*secondIter);
        secondIter++;
    }
    // cout << result.size() << endl;
    return result;
}

// 为goto找到对应的一个活前缀
int findOneState(int which, int number, int where)
{
    for (int i = 0; i < liveLength; i++)
    {
        if (allNode[i].which == which && allNode[i].number == number && allNode[i].where == where+1)
        {
            return i;
        }
    }
}

// 遍历allNode，从allNode中获取相关的元素，使用递归
set<int> getStateFromAllNode(char value)
{
    set<int> temp;
    for (int i = 0; i < liveLength; i++)
    {
        if (allNode[i].which == VN[value] && allNode[i].where == 0)
        {
            temp.insert(i);
            if (allNode[i].next >= 'A' && allNode[i].next <= 'Z')
            {
                temp = mergeToSet(temp, getStateFromAllNode(allNode[i].next));
            }
        }
    }

    // 验证closure集合的求解
    set<int>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
        // cout << "iter = " << *iter << endl;
        iter++;
    }
    // cout << endl;
    return temp;
}

// 求出closure集合
set<int> colsureIX(set<int> temp)
{
    int countnumber;
    set<int>::iterator iter;
    do {
        countnumber = temp.size();
        // 首先遍历temp中的所有元素，对每一个活前缀文法进行拓展
        set<int> middle;
        iter = temp.begin();
        while (iter != temp.end())
        {
            if (allNode[*iter].next >= 'A' && allNode[*iter].next <= 'Z')
            {
                middle = getStateFromAllNode(allNode[*iter].next);
            }
            iter++;
        }
        temp = mergeToSet(temp, middle);
        // cout << "temp.size() = " << temp.size() << "    " << countnumber << endl;
    } while (temp.size() > countnumber);
    return temp;
}

// goto函数实现
void gotoIX(set<int> firstState)
{
    // 遍历原来状态的每一个元素，进行状态转移
    set<int>::iterator iter = firstState.begin();
    int stateLength = newDFA.size();
    while (iter != firstState.end())
    {
        if (!allNode[*iter].flag)
        {
            set<int> secondState;
            // 寻找每个后面的元素对应的状态
            secondState.insert(findOneState(allNode[*iter].which, allNode[*iter].number, allNode[*iter].where));
            secondState = colsureIX(secondState);
            newDFA.insert(secondState);
            if (newDFA.size() > stateLength)
            {
                gotoIX(secondState);
            }
        }
        iter++;
    }
}

// 求解DFA
void compulateProjectList()
{
    // 首先进行新DFA的初始化操作，将I0状态的首个活前缀文法加入将项目集合规范族中
    set<int> firstState;
    firstState.insert(liveLength-2);
    firstState = colsureIX(firstState);
    newDFA.insert(firstState);
    gotoIX(firstState);
    cout << "newDFA size = " << newDFA.size() << endl;
    // 根据set容器的大小是否发生变化来判断是否要继续
}

// 求解LR(0)分析表
void compulateLR0()
{
    set<set<int> >::iterator father;

}

// 打印文法
void printWenfa()
{
    map<char, vector<string> >::iterator iter;
    iter = input.begin();
    while (iter != input.end()) {
        cout << iter -> first;
        cout << "       ";
        vector<string> result = iter -> second;
        for (int i = 0; i < result.size(); i++) {
            cout << result[i] << "      ";
        }
        cout << endl;
        iter++;
    }
}

// 打印所有的终结符和终结符
void printVNVT()
{
    map<char, int>::iterator iter, iter1;
    iter = VN.begin();
    iter1 = VT.begin();

    cout << "++++++++++++++++VN+++++++++++++++++++" << endl;

    while (iter != VN.end())
    {
        cout << iter -> first;
        cout << "   ";
        cout << iter -> second << endl;
        iter++;
    }

    cout << "+++++++++++++++++VT++++++++++++++++++" << endl;
    while (iter1 != VT.end())
    {
        cout << iter1 -> first;
        cout << "   ";
        cout << iter1 -> second << endl;
        iter1++;
    }
    cout << "+++++++++++++++++end+++++++++++++++++" << endl;
}

// 打印容器内容
void printVector()
{
    for (int i = 0; i < VN.size(); i++)
    {
        vector<string> value = wf[i];
        cout << i << "  ";
        for (int j = 0; j < value.size(); j++)
        {
            cout << value[j] << "   ";
        }
        cout << endl;
    }
}

// 打印构造的活前缀
void printLive()
{
    for (int i = 0; i < liveLength; i++)
    {
        cout << allNode[i].which << "   ";
        cout << allNode[i].number << "  ";
        cout << allNode[i].where << "   ";
        int flag = allNode[i].flag == true ? 1 : 0;
        cout << flag<< "    ";
        cout << allNode[i].next << "    ";
        cout << endl;
    }
}

// 打印DFA
void printDFA()
{
    set<set<int> >::iterator father = newDFA.begin();
    while (father != newDFA.end())
    {
        set<int> temp = *father;
        set<int>::iterator son = temp.begin();
        while (son != temp.end())
        {
            cout << *son << "   ";
            son++;
        }
        cout << endl << endl;
        father++;
    }
}

int main(int argc, char const *argv[])
{
    readWenfaFromFile();
    printWenfa();
    toSimpleWenfa();
    printVNVT();
    printLive();
    compulateProjectList();
    //getStateFromAllNode('E');
    printVector();
    printDFA();
    cout << wfLength << endl;
    cout << liveLength << endl;
    return 0;
}
