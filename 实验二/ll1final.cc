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

// 文法简易数据结构
struct Node {
    int which;      // 是哪个非终结符对应的文法
    int where;      // 对应那个终结符的第几条产生式
    bool flag;      // 右侧是否为一部修系统，初始化为false，默认不是
};

// 文法编号及对应的数据符号
struct Node allNode[length];

// 类比网页搜索中的反转列表，建立一个字符后面所跟的所有字符
map<int, set<string> > subChar;

// 类比网页搜索中的反转列表，确定一个字符的首字符
map<int, set<char> > preorderChar;

// first集合，运用其自动去重特点
map<char, set<char> > first;

// follow集合，运用其自动去重特点
map<char, set<char> > follow;

// 每一条产生式的first
map<int, set<char> > findFirst;

// 构造LL1预测分析表
char LL1[length][length];

// 符号栈
stack<char> symbol;

// 输入串
string waitForAnalysis;

// 所有文法的长度
int wfLength = 0;

// 判断是否是非终结符，如果是，返回true
bool isVN(char value)
{
    return (value >= 'A' && value <= 'Z') ? true : false;
}

// 判断是否是终结符，如果是，返回true
bool isVT(char value)
{
    return (value >= 'a' && value <= 'z' || value == '(' || value == ')' || value == '+' || value == '*' || value == '#') ? true : false;
}

// 判断是否为一部修
bool isEmpty(char value)
{
    return (value == '@') ? true : false;
}

// 判断文法能否推导出一部修
bool toEmpty(char value)
{
    vector<string>::iterator iter = input[value].begin();
    while (iter != wf[VN[value]].end())
    {
        string temp = *iter;
        if (temp[0] == '@') return true;
        iter++;
    }
    return false;
}

// 合并两个set集合中的元素
set<char> mergeToSet(set<char> first, set<char> second)
{
    set<char> result;
    set<char>::iterator firstIter = first.begin();
    set<char>::iterator secondIter = second.begin();
    while (firstIter != first.end())
    {
        result.insert(*firstIter);
        firstIter++;
    }
    while (secondIter != second.end())
    {
        result.insert(*secondIter);
        secondIter++;
    }
    return result;
}

// 从文件中读取文法
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
        cut = content.substr(3);
        temp = cut.find('|');
        if (temp < 0 || cut.length() <= temp)
        {
            value.push_back(cut);
            flag = false;
        }
        while (flag)
        {
            tempString = cut.substr(0, temp);
            cut = cut.substr(temp+1);
            value.push_back(tempString);
            temp = cut.find('|');
            if (temp >= cut.length() || temp < 0)
            {
                value.push_back(cut);
                flag = false;
            }
        }
        input.insert(make_pair(content[0], value));
        VN.insert(make_pair(content[0], VN.size()));
        if (i == 1) {
            first = content[0];
        }
    }
    inf.close();
}

// 改变存储文法的数据结构（相当于增加了编号），顺便取得所有的终结符和文法结构体
void toSimpleWenfa()
{
    map<char, vector<string> >::iterator iter;
    iter = input.begin();
    while (iter != input.end())
    {
        wf[VN[iter -> first]] = iter -> second;
        set<char> preorder;     // 一个非终结符的first集合初稿
        set<string> sub;          // 一个非终结符的follow集合初稿
        for (int i = 0; i < wf[VN[iter -> first]].size(); i++)
        {
            string value = wf[VN[iter -> first]][i];
            // 初始化文法数据结构
            allNode[wfLength].which = VN[iter->first];
            allNode[wfLength].where = i;
            for (int j = 0; j < value.length(); j++)
            {
                // 收集终结符
                if (isVT(value[j]) && VT.count(value[j]) == 0)
                {
                    VT.insert(make_pair(value[j], VT.size()));
                }
                if (value.length() == 1 && isEmpty(value[0]))
                {
                    // 是一部修系统
                    allNode[wfLength].flag = true;
                } else
                {
                    // 不是一部修系统
                    allNode[wfLength].flag = false;
                }
                // 填写sub
                if (isVN(value[j]) && j < value.length()-1)
                {
                    sub = subChar[VN[value[j]]];
                    // 构造相关节点，供follow集合求解使用
                    string node = "abcd";
                    node[0] = value[j+1];
                    if (isVN(value[j+1]))
                    {
                        if (j == value.length()-2) node[1] = '1';
                        else node[1] = '0';
                        if (toEmpty(value[j+1])) node[2] = '1';
                        else node[2] = '0';
                    } else
                    {
                        node[1] = '0';
                        node[2] = '0';
                    }

                    node[3] = iter -> first;
                    sub.insert(node);
                    // 注意C++map中键冲突的机制
                    subChar.erase(VN[value[j]]);
                    subChar.insert(make_pair(VN[value[j]], sub));
                }
            }
            wfLength++;
            preorder.insert(value[0]);
        }
        preorderChar.insert(make_pair(VN[iter->first], preorder));
        iter++;
    }
    VT.insert(make_pair('#', VT.size()));
}

// 求解first集合
set<char> getFirstMap(char value)
{
    // 根据preorderChar来进行运算
    set<char> firstValue = preorderChar[VN[value]];
    set<char>::iterator iter = firstValue.begin();
    set<char> result;
    while (iter != firstValue.end())
    {
        if (isVT(*iter) || *iter == '@') result.insert(*iter);
        else result = mergeToSet(result, getFirstMap(*iter));
        iter++;
    }
    return result;
}

// 求解每个VN的first集合
void compulateFirst()
{
    map<char, int>::iterator iter = VN.begin();
    while (iter != VN.end())
    {
        first.insert(make_pair(iter->first, getFirstMap(iter->first)));
        iter++;
    }
}

// 求解每个产生式的first集合
void compulateEvery()
{
    for (int i = 0; i < wfLength; i++)
    {
        string value = wf[allNode[i].which][allNode[i].where];
        set<char> temp;
        if (isVT(value[0]))
        {
            temp.insert(value[0]);
            findFirst.insert(make_pair(i, temp));
        } else
        {
            findFirst.insert(make_pair(i, first[value[0]]));
        }
    }
}

// 求解follow集合
set<char> getFollowMap(char value)
{
    // 根据subChar来进行运算
    set<string> temp = subChar[VN[value]];
    set<string>::iterator iter = temp.begin();
    set<char> result;
    result.insert('#');
    while (iter != temp.end())
    {
        string value = *iter;
        if (isVT(value[0]))
        {
            result.insert(value[0]);
        }
        else if (value[1] == '1' && value[2] == '1' || value[1] == '1' && value[2] == '0')
        {
            // 第一条规则
            result = mergeToSet(result, first[value[0]]);
            // 第二三条规则
            set<char> beforeValue = getFollowMap(value[3]);
            set<char> oldBeforeResult = follow[value[0]];
            follow.erase(value[0]);
            follow.insert(make_pair(value[0], mergeToSet(oldBeforeResult, beforeValue)));
            result = mergeToSet(result, beforeValue);
        }
        iter++;
    }
    set<char> oldResult = follow[value];
    follow.erase(value);
    follow.insert(make_pair(value, mergeToSet(oldResult, result)));
    return result;
}

// 求解每个VN的follow集合
void compulateFollow()
{
    map<char, int>::iterator iter = VN.begin();
    while (iter != VN.end())
    {
        if (subChar.count(iter->second)) getFollowMap(iter->first);
        iter++;
    }
    // 删除follow集合中的一部修元素
    map<char, set<char> >::iterator iter1 = follow.begin();
    while (iter1 != follow.end())
    {
        (iter1->second).erase('@');
        iter1++;
    }
}

// 根据是第几条产生式，快速求解出是第几条文法
int doFindNode(int which)
{
    for (int i = 0; i < wfLength; i++)
        if (allNode[i].which == which) return i;
}

// 根据map中value的值来寻找键
char getCharByValue(int value, bool flag)
{
    map<char, int>::iterator iter;
    if (flag)
    {
        iter = VN.begin();
        while (iter != VN.end())
        {
            if (iter -> second == value) return iter -> first;
            iter++;
        }
    } else
    {
        iter = VT.begin();
        while (iter != VT.end())
        {
            if (iter -> second == value)
            {
                return iter -> first;
            }
            iter++;
        }
    }
    return NULL;
}

// 根据是第几条产生式，快速求解出一部修文法
int doFindEmpty(int which)
{
    for (int i = 0; i < wfLength; i++)
        if (allNode[i].which == which && allNode[i].flag == true) return i;
}

// 求解LL1,使用初始化的allNode数据结构
void getLL1()
{
    map<char, int>::iterator iter = VN.begin(), iter1;
    while (iter != VN.end())
    {
        iter1 = VT.begin();
        while (iter1 != VT.end())
        {
            // first 集合填表
            if (first[iter->first].count(iter1->first) == 1)
            {
                int where = doFindNode(VN[iter->first]);
                // 需要进行填表,判断是那条产生式
                for (int i = 0; i < wf[VN[iter->first]].size(); i++)
                {
                    where += i;
                    if (findFirst[where].count(iter1->first) == 1)
                    {
                        LL1[iter -> second][iter1 -> second] = where + 'A';
                    }
                }
            }
            // follow 集合填表
            if (first[iter->first].count('@') == 1)
            {
                // 需要填写follow集合
                int where = doFindEmpty(VN[iter->first]);
                set<char>::iterator iter2 = follow[iter->first].begin();
                while (iter2 != follow[iter->first].end())
                {
                    LL1[iter -> second][VT[*iter2]] = where + 'A';
                    iter2++;
                }
            }
            iter1++;
        }
        iter++;
    }
}

// 打印没有修改的文法
void printWenfaInit()
{
    cout << "===================" << endl;
    cout << setw(15) << "文法集合" << endl;
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
    cout << endl << endl;
}

// 打印所有的终结符和终结符
void printVNVT()
{
    cout << "================" << endl;
    map<char, int>::iterator iter, iter1;
    iter = VN.begin();
    iter1 = VT.begin();
    cout << setw(15) << "非终结符及其编号" << endl;
    while (iter != VN.end())
    {
        cout << iter -> first;
        cout << "   ";
        cout << iter -> second << endl;
        iter++;
    }
    cout << setw(15) << "终结符及其编号" << endl;
    while (iter1 != VT.end())
    {
        cout << iter1 -> first;
        cout << "   ";
        cout << iter1 -> second << endl;
        iter1++;
    }
    cout << endl << endl;
}

// 遍历所有的subchar
void printSubchar()
{
    cout << "============" << endl;
    map<int, set<string> >::iterator iter = subChar.begin();
    while (iter != subChar.end())
    {
        cout << iter->first << "    ";
        set<string> value = iter->second;
        set<string>::iterator iterTemp = value.begin();
        while (iterTemp != value.end())
        {
            cout << *iterTemp << "  ";
            iterTemp++;
        }
        cout << endl;
        iter++;
    }
    cout << endl << endl;
}

// 输出first集合和follow集合
void printFirstAndFollow()
{
    cout << setw(20) << "first" << endl;
    map<char, set<char> >::iterator iter1 = first.begin(), iter2 = follow.begin();
    while (iter1 != first.end())
    {
        cout << setw(4) << iter1->first;
        set<char> value = iter1->second;
        set<char>::iterator itertemp = value.begin();
        while (itertemp != value.end())
        {
            cout << setw(4) << *itertemp;
            itertemp++;
        }
        cout << endl;
        iter1++;
    }
    cout << endl;
    cout << setw(20) << "follow" << endl;
    while (iter2 != follow.end())
    {
        cout << setw(4) << iter2->first;
        set<char> value = iter2->second;
        set<char>::iterator itertemp = value.begin();
        while (itertemp != value.end())
        {
            cout << setw(4) << *itertemp;
            itertemp++;
        }
        cout << endl;
        iter2++;
    }
    cout << endl;
}

// 打印LL1分析表
void printLL1()
{
    cout << "===================================================================+==" << endl;
    cout << setw(36) << "LR1分析表" << endl;
    // 打印列标题
    cout << setw(10) << "   ";
    for (int i = 0; i < VT.size(); i++)
    {
        cout << setw(10) << getCharByValue(i, false);
    }
    cout << endl;
    for (int i = 0; i < VN.size(); i++)
    {
        cout << setw(10) << getCharByValue(i, true);
        for (int j = 0; j < VT.size(); j++)
        {
            char where = LL1[i][j];
            if (where != NULL) cout << setw(7) << wf[allNode[where-'A'].which][allNode[where-'A'].where] << setw(2) << "<-" << setw(1) << getCharByValue(i, true);
            else cout << setw(10) <<"   ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

// 打印所有的allNode
void printAllNode()
{
    for (int i = 0; i < wfLength; i++)
        cout << allNode[i].which << "   " << allNode[i].where << "  " << endl;
}

// 进行分析
void analysis()
{
    int count = 0, i = 0; // 步数
    char where;   // 查LL1分析表
    do {
        cout << "please input the string waiting for analysis: ";
        cin >> waitForAnalysis;
    } while(waitForAnalysis[waitForAnalysis.length()-1] != '#');
    // 初始化符号栈
    symbol.push('#');
    symbol.push('E');
    cout << setw(10) << "步骤" << setw(10) << "符号栈" << setw(10) << "大小" << setw(20) << "输入串" << setw(10) << "产生式" << endl;
    for (i = 0; i < waitForAnalysis.length(); i++)
    {
        cout << setw(10) << count << setw(10) << symbol.top() << setw(10) << symbol.size();
        cout << setw(20) << waitForAnalysis.substr(i, waitForAnalysis.length()-1) << setw(10) << " " << endl;
        do {
            count++;
            where = LL1[VN[symbol.top()]][VT[waitForAnalysis[i]]];
            if (where == NULL) break;
            string value = wf[allNode[where-'A'].which][allNode[where-'A'].where];
            // 栈顶元素出栈
            symbol.pop();
            // 将value逆序入栈
            for (int j = value.length()-1; j >= 0; j--)
                symbol.push(value[j]);
            if (value[0] == '@')
            {
                symbol.pop();
                cout << setw(10) << count << setw(10) << symbol.top() << setw(10) << symbol.size();
                cout << setw(20) << waitForAnalysis.substr(i, waitForAnalysis.length()-1) << setw(10) << value << endl;
                if (symbol.top()!= '#') continue;
                else break;
            }
            cout << setw(10) << count << setw(10) << symbol.top() << setw(10) << symbol.size();
            cout << setw(20) << waitForAnalysis.substr(i, waitForAnalysis.length()-1) << setw(10) << value << endl;
        } while (isVN(symbol.top()));
        if (symbol.top() == '#') break;
        symbol.pop();
        count++;
        if (symbol.empty()) break;
    }
    if (where != NULL && !symbol.empty() && symbol.top() == '#' && i == waitForAnalysis.length()-1)
        cout << "yes" << endl;
    else cout << "no" << endl;
}

int main()
{
    readWenfaFromFile();
    printWenfaInit();
    toSimpleWenfa();
    printVNVT();
    compulateFirst();
    printAllNode();
    compulateEvery();
    printSubchar();
    compulateFollow();
    getLL1();
    printLL1();
    printFirstAndFollow();
    analysis();
    return 0;
}
