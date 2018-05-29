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

// �ķ��г��ֵķ��ս����Ӧ��int��������
map<char, int> VN;

// �ķ��г��ֵ��ս����Ӧ��int��������
map<char, int> VT;

// ���ļ��ж�ȡ��û�г�ʼ�����ķ�������ǰ��ʵ���ȡ�ļ���ģ�飩
map<char, vector<string> > input;

// �����������ķ�
vector<string> wf[length];

// �ķ��������ݽṹ
struct Node {
    int which;      // ���ĸ����ս����Ӧ���ķ�
    int where;      // ��Ӧ�Ǹ��ս���ĵڼ�������ʽ
    bool flag;      // �Ҳ��Ƿ�Ϊһ����ϵͳ����ʼ��Ϊfalse��Ĭ�ϲ���
};

// �ķ���ż���Ӧ�����ݷ���
struct Node allNode[length];

// �����ҳ�����еķ�ת�б�����һ���ַ����������������ַ�
map<int, set<string> > subChar;

// �����ҳ�����еķ�ת�б�ȷ��һ���ַ������ַ�
map<int, set<char> > preorderChar;

// first���ϣ��������Զ�ȥ���ص�
map<char, set<char> > first;

// follow���ϣ��������Զ�ȥ���ص�
map<char, set<char> > follow;

// ÿһ������ʽ��first
map<int, set<char> > findFirst;

// ����LL1Ԥ�������
char LL1[length][length];

// ����ջ
stack<char> symbol;

// ���봮
string waitForAnalysis;

// �����ķ��ĳ���
int wfLength = 0;

// �ж��Ƿ��Ƿ��ս��������ǣ�����true
bool isVN(char value)
{
    return (value >= 'A' && value <= 'Z') ? true : false;
}

// �ж��Ƿ����ս��������ǣ�����true
bool isVT(char value)
{
    return (value >= 'a' && value <= 'z' || value == '(' || value == ')' || value == '+' || value == '*' || value == '#') ? true : false;
}

// �ж��Ƿ�Ϊһ����
bool isEmpty(char value)
{
    return (value == '@') ? true : false;
}

// �ж��ķ��ܷ��Ƶ���һ����
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

// �ϲ�����set�����е�Ԫ��
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

// ���ļ��ж�ȡ�ķ�
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
        cout << "û�гɹ����ļ�" << endl;
    }
    while (getline(inf, content)) {
        //����ȡ�������˵��ַ����������һ��Ϊ���ս�����ұ߿������ս������Ҳ����Ϊ���ս�����������
        bool flag = true;
        i++;
        vector<string> value;
        // Linux �� C++ ������ͬ
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

// �ı�洢�ķ������ݽṹ���൱�������˱�ţ���˳��ȡ�����е��ս�����ķ��ṹ��
void toSimpleWenfa()
{
    map<char, vector<string> >::iterator iter;
    iter = input.begin();
    while (iter != input.end())
    {
        wf[VN[iter -> first]] = iter -> second;
        set<char> preorder;     // һ�����ս����first���ϳ���
        set<string> sub;          // һ�����ս����follow���ϳ���
        for (int i = 0; i < wf[VN[iter -> first]].size(); i++)
        {
            string value = wf[VN[iter -> first]][i];
            // ��ʼ���ķ����ݽṹ
            allNode[wfLength].which = VN[iter->first];
            allNode[wfLength].where = i;
            for (int j = 0; j < value.length(); j++)
            {
                // �ռ��ս��
                if (isVT(value[j]) && VT.count(value[j]) == 0)
                {
                    VT.insert(make_pair(value[j], VT.size()));
                }
                if (value.length() == 1 && isEmpty(value[0]))
                {
                    // ��һ����ϵͳ
                    allNode[wfLength].flag = true;
                } else
                {
                    // ����һ����ϵͳ
                    allNode[wfLength].flag = false;
                }
                // ��дsub
                if (isVN(value[j]) && j < value.length()-1)
                {
                    sub = subChar[VN[value[j]]];
                    // ������ؽڵ㣬��follow�������ʹ��
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
                    // ע��C++map�м���ͻ�Ļ���
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

// ���first����
set<char> getFirstMap(char value)
{
    // ����preorderChar����������
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

// ���ÿ��VN��first����
void compulateFirst()
{
    map<char, int>::iterator iter = VN.begin();
    while (iter != VN.end())
    {
        first.insert(make_pair(iter->first, getFirstMap(iter->first)));
        iter++;
    }
}

// ���ÿ������ʽ��first����
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

// ���follow����
set<char> getFollowMap(char value)
{
    // ����subChar����������
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
            // ��һ������
            result = mergeToSet(result, first[value[0]]);
            // �ڶ���������
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

// ���ÿ��VN��follow����
void compulateFollow()
{
    map<char, int>::iterator iter = VN.begin();
    while (iter != VN.end())
    {
        if (subChar.count(iter->second)) getFollowMap(iter->first);
        iter++;
    }
    // ɾ��follow�����е�һ����Ԫ��
    map<char, set<char> >::iterator iter1 = follow.begin();
    while (iter1 != follow.end())
    {
        (iter1->second).erase('@');
        iter1++;
    }
}

// �����ǵڼ�������ʽ�����������ǵڼ����ķ�
int doFindNode(int which)
{
    for (int i = 0; i < wfLength; i++)
        if (allNode[i].which == which) return i;
}

// ����map��value��ֵ��Ѱ�Ҽ�
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

// �����ǵڼ�������ʽ����������һ�����ķ�
int doFindEmpty(int which)
{
    for (int i = 0; i < wfLength; i++)
        if (allNode[i].which == which && allNode[i].flag == true) return i;
}

// ���LL1,ʹ�ó�ʼ����allNode���ݽṹ
void getLL1()
{
    map<char, int>::iterator iter = VN.begin(), iter1;
    while (iter != VN.end())
    {
        iter1 = VT.begin();
        while (iter1 != VT.end())
        {
            // first �������
            if (first[iter->first].count(iter1->first) == 1)
            {
                int where = doFindNode(VN[iter->first]);
                // ��Ҫ�������,�ж�����������ʽ
                for (int i = 0; i < wf[VN[iter->first]].size(); i++)
                {
                    where += i;
                    if (findFirst[where].count(iter1->first) == 1)
                    {
                        LL1[iter -> second][iter1 -> second] = where + 'A';
                    }
                }
            }
            // follow �������
            if (first[iter->first].count('@') == 1)
            {
                // ��Ҫ��дfollow����
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

// ��ӡû���޸ĵ��ķ�
void printWenfaInit()
{
    cout << "===================" << endl;
    cout << setw(15) << "�ķ�����" << endl;
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

// ��ӡ���е��ս�����ս��
void printVNVT()
{
    cout << "================" << endl;
    map<char, int>::iterator iter, iter1;
    iter = VN.begin();
    iter1 = VT.begin();
    cout << setw(15) << "���ս��������" << endl;
    while (iter != VN.end())
    {
        cout << iter -> first;
        cout << "   ";
        cout << iter -> second << endl;
        iter++;
    }
    cout << setw(15) << "�ս��������" << endl;
    while (iter1 != VT.end())
    {
        cout << iter1 -> first;
        cout << "   ";
        cout << iter1 -> second << endl;
        iter1++;
    }
    cout << endl << endl;
}

// �������е�subchar
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

// ���first���Ϻ�follow����
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

// ��ӡLL1������
void printLL1()
{
    cout << "===================================================================+==" << endl;
    cout << setw(36) << "LR1������" << endl;
    // ��ӡ�б���
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

// ��ӡ���е�allNode
void printAllNode()
{
    for (int i = 0; i < wfLength; i++)
        cout << allNode[i].which << "   " << allNode[i].where << "  " << endl;
}

// ���з���
void analysis()
{
    int count = 0, i = 0; // ����
    char where;   // ��LL1������
    do {
        cout << "please input the string waiting for analysis: ";
        cin >> waitForAnalysis;
    } while(waitForAnalysis[waitForAnalysis.length()-1] != '#');
    // ��ʼ������ջ
    symbol.push('#');
    symbol.push('E');
    cout << setw(10) << "����" << setw(10) << "����ջ" << setw(10) << "��С" << setw(20) << "���봮" << setw(10) << "����ʽ" << endl;
    for (i = 0; i < waitForAnalysis.length(); i++)
    {
        cout << setw(10) << count << setw(10) << symbol.top() << setw(10) << symbol.size();
        cout << setw(20) << waitForAnalysis.substr(i, waitForAnalysis.length()-1) << setw(10) << " " << endl;
        do {
            count++;
            where = LL1[VN[symbol.top()]][VT[waitForAnalysis[i]]];
            if (where == NULL) break;
            string value = wf[allNode[where-'A'].which][allNode[where-'A'].where];
            // ջ��Ԫ�س�ջ
            symbol.pop();
            // ��value������ջ
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
