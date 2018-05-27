#include<iostream>
#include<iomanip>
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

// new DFA，使用set的目的是去重
vector<set<int> > newDFA;

// 构造LR(0)文法分析表
char LR0[length][length];

// 状态栈
stack<int> state;

// 符号栈
stack<char> symbol;

// 输入串
string waitForAnalysis;

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
        // cout << "content = " << content << endl;
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
        // cout << "size of VN = " << VN.size() << endl;
        VN.insert(make_pair(content[0], VN.size()));
        if (i == 1) {
            first = content[0];
            // cout << "first = " << first << endl;
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

        // cout << "size = " << (iter -> second).size() << endl;
        wf[VN[iter -> first]] = iter -> second;
        // 初始化其活前缀文法
        for (int i = 0; i < wf[VN[iter -> first]].size(); i++)
        {
            string value = wf[VN[iter -> first]][i];
            // cout << "value = " << value << endl;
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
        //cout << "wf.size = " << (wf[VN[iter->first]]).size() << endl;
        wfLength += (iter -> second).size();
        iter++;
    }
    VT.insert(make_pair('#', VT.size()));
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

// 根据活前缀的which和where来判断是第几条文法，操作数据结构为wf
int doCountWenfa(int which, int number)
{
    int wenfaCount = 0;
    for (int i = 0; i < which; i++)
    {
        wenfaCount += wf[i].size();
    }
    return wenfaCount+number;
}

// 遍历allNode，从allNode中获取相关的元素，使用递归
set<int> getStateFromAllNode(char value, int where)
{
    set<int> temp;
    for (int i = 0; i < liveLength; i++)
    {
        if (allNode[i].which == VN[value] && allNode[i].where == 0)
        {
            temp.insert(i);
            if (allNode[i].next >= 'A' && allNode[i].next <= 'Z' && i != where)
            {
                temp = mergeToSet(temp, getStateFromAllNode(allNode[i].next, i));
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
                // cout << "allNode[*iter].next = " << allNode[*iter].next << endl;
                middle = getStateFromAllNode(allNode[*iter].next, liveLength-2);
            }
            iter++;
        }
        temp = mergeToSet(temp, middle);
        // cout << "temp.size() = " << temp.size() << "    " << countnumber << endl;
    } while (temp.size() > countnumber);
    return temp;
}

// goto函数实现
void gotoIX(set<int> firstState, int firstStateNumber)
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
            // 判断是否是新状态
            int i;
            for (i = 0; i < newDFA.size(); i++)
            {
                if (secondState == newDFA[i])
                {
                    break;
                }
            }
            if (i == newDFA.size()) {
                // 新状态
                newDFA.push_back(secondState);
            }
            if (allNode[*iter].next >= 'A' && allNode[*iter].next <= 'Z')
            {
                LR0[firstStateNumber][VN[allNode[*iter].next]+VT.size()] = i+'a';
            }

            if (allNode[*iter].next >= 'a' && allNode[*iter].next <= 'z')
            {
                LR0[firstStateNumber][VT[allNode[*iter].next]] = i+'a';
            }

            if (newDFA.size() > stateLength)
            {
                gotoIX(secondState, i);
            }
        } else
        {
            // 这个是规约项目，所有都填
            for (int i = 0; i < VT.size(); i++)
            {
                // 如果不是最后的可识别文法
                if (allNode[*iter].which == VN.size()-1 && allNode[*iter].where == 1)
                {
                    LR0[firstStateNumber][VT['#']] = '!';
                } else
                {
                    cout << "firstState = " << firstStateNumber;
                    cout << "   which = " << (allNode[*iter].which);
                    cout << "   where = " << (allNode[*iter].number) << endl;
                    LR0[firstStateNumber][i] = doCountWenfa(allNode[*iter].which, allNode[*iter].number)+'A';
                }
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

    cout << "++++++++++" << endl;

    newDFA.push_back(firstState);
    gotoIX(firstState, 0);
    cout << "newDFA size = " << newDFA.size() << endl;
    // 根据set容器的大小是否发生变化来判断是否要继续
}

// 进行分析
void analysis()
{
    cout << "pls input the string waited for analyze, end by '#': ";
    cin >> waitForAnalysis;
    // 初始化状态栈和符号栈
    symbol.push('#');
    state.push(0);
    // 字符串的扫描位置
    int where = 0, i;
    char temp;
    for (i = 0; i < waitForAnalysis.length(); i++)
    {
        char symbolTop = symbol.top();
        do {
            cout << setw(6) << state.top() << setw(6) << symbol.top() << setw(15) << waitForAnalysis.substr(i, waitForAnalysis.length()) << endl;
            // 查表得到的值
            temp = LR0[state.top()][VT[waitForAnalysis[i]]];
            // cout << "temp = " << temp << endl;
            if (temp == NULL)
            {
                break;
            }
            // 拿状态栈的栈顶和输入串的串首字符填表
            if (temp >= 'A' && temp <= 'Z')
            {
                // 待规约项目
                // temp为产生式编号，这时候需要查找产生式
                int wenfaCount = 'A', j = 0;
                for (j = 0; j < VN.size(); j++)
                {
                    wenfaCount += wf[j].size();
                    if (temp < wenfaCount)
                    {
                        wenfaCount -= wf[j].size();
                        break;
                    }
                }
                // temp - wenfaCount是对应的文法，找到其对应的文法
                string wait = wf[j][temp-wenfaCount];
                // 根据i找到其对应的非终结符
                map<char, int>::iterator something = VN.begin();
                while (something != VN.end())
                {
                    if (something->second == j) break;
                    else something++;
                }
                char theVN =something->first;

                // cout << "theVN = " << theVN << endl;

                // 符号栈出栈并且入栈
                for (int k = 0; k < wait.length(); k++)
                {
                    symbol.pop();
                    state.pop();
                }
                symbol.push(theVN);
                state.push(LR0[state.top()][VN[symbol.top()]+VT.size()]-'a');
                cout << setw(6) << state.top() << setw(6) << symbol.top() << setw(15) << waitForAnalysis.substr(i, waitForAnalysis.length()) << endl;
                continue;
            } else if (temp >= 'a' && temp <= 'z')
            {
                // 状态栈入栈
                state.push(temp-'a');
                symbol.push(waitForAnalysis[i]);
            } else
            {
                cout << "temp >= 'a' && temp <= 'z'" << endl;
                break;
            }
        } while (temp != '!' && temp <= 'Z' && temp >= 'A');
        if (temp == NULL || temp == '!')
        {
            cout << "temp == NULL || temp == '!'" << endl;
            break;
        }
    }

    cout << "++++++++++++++++++++++++++++++++++++" << endl;

    // 扫描完毕，是否是该文法的符号串的判断
    if (i == waitForAnalysis.length()-1 && temp == '!')
    {
        cout << "yes" << endl;
    } else
    {
        cout << "no" << endl;
    }
}

// 打印没有修改的文法
void printWenfaInit()
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

// 打印修改之后的文法
void printWenfa()
{
    cout << "(((((((((((((((()))))))))))))))))))" << endl;
    for (int i = 0; i < VN.size(); i++)
    {
        cout << i << "size = " << wf[i].size() << " ";
        for (int j = 0; j < wf[i].size(); j++)
        {
            cout << wf[i][j] << "   ";
        }
        cout << endl;
    }
    cout << endl << endl;
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
    for (int i = 0; i < newDFA.size(); i++)
    {
        cout << "state " << i << ":  ";
        set<int> temp = newDFA[i];
        set<int>::iterator son = temp.begin();
        while (son != temp.end())
        {
            cout << *son << "   ";
            son++;
        }
        cout << endl << endl;
    }
}

// 打印LR(0)分析表
void printLR0()
{
    cout << setw(6) << "state";
    map<char, int>::iterator iter1, iter2;
    for (int i = 0; i < VT.size(); i++)
    {
        iter1 = VT.begin();
        while (iter1 != VT.end())
        {
            if (iter1->second == i)
            {
                cout << setw(6) << iter1->first;
                break;
            }
            iter1++;
        }
    }

    for (int i = 0; i < VN.size(); i++)
    {
        iter2 = VN.begin();
        while (iter2 != VN.end())
        {
            if (iter2->second == i)
            {
                cout << setw(6) << iter2->first;
                break;
            }
            iter2++;
        }
    }
    cout << endl;
    for (int i = 0; i < newDFA.size(); i++)
    {
        cout << setw(6) << i;
        for (int j = 0; j < VT.size(); j++)
        {
            if (LR0[i][j] >= 'A' && LR0[i][j] <= 'Z')
            {
                cout << setw(6) << LR0[i][j] - 'A';
            } else if (LR0[i][j] >= 'a' && LR0[i][j] <= 'z')
            {
                cout << setw(6) << LR0[i][j] - 'a';
            } else if (LR0[i][j] == '!')
            {
                cout << setw(6) << '!';
            } else
            {
                cout << setw(6) << ' ';
            }
        }

        for (int j = VT.size(); j < VN.size()+VT.size(); j++)
        {
            if (LR0[i][j] >= 'a' && LR0[i][j] <= 'z')
            {
                cout << setw(6) << LR0[i][j] - 'a';
            } else
            {
                cout << setw(6) << ' ';
            }
        }
        cout << endl;
    }
    cout << endl << endl;
}

int main(int argc, char const *argv[])
{
    readWenfaFromFile();
    printWenfaInit();
    toSimpleWenfa();
    printWenfa();
    printVNVT();
    printLive();

    compulateProjectList();

    printLR0();
    printVector();
    printDFA();
    cout << wfLength << endl;
    cout << liveLength << endl;

    analysis();
    return 0;
}
