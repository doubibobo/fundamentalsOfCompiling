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

// �ķ��м��˻�ǰ׺�ľ���
struct Node {
    int which;  // ֪���������ڼ���
    int number;	// ֪���������ڼ�������һ��
    int where;  // ֪����ǰ׺���ķ��е�λ��
    char next;  // ��ǰ׺������ַ��������ж�״̬ת��
    bool flag;   // �Ƿ�Ϊ�ɹ�Լ��ǰ׺��Ĭ��Ϊ0���ɹ�Լ
};

// �ķ����о�����ǰ׺����ľ���
struct Node allNode[length];

// new DFA��ʹ��set��Ŀ����ȥ��
vector<set<int> > newDFA;

// ����LR(0)�ķ�������
char LR0[length][length];

// ״̬ջ
stack<int> state;

// ����ջ
stack<char> symbol;

// ���봮
string waitForAnalysis;

// �����ķ��ĳ���
int wfLength = 0;

// ���л�ǰ׺�ķ���Ŀ
int liveLength = 0;

// DFA��״̬����Ŀ
int stateNumber = 0;

// ���ļ��ж�ȡ�ķ��������ķ�����չ
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
        VN.insert(make_pair(content[0], VN.size()));
        if (i == 1) {
            first = content[0];
        }
    }
    // �����ķ���չ
    string some = "X";
    some[0] = first;
    vector<string> value;
    value.push_back(some);
    input.insert(make_pair('X', value));
    VN.insert(make_pair('X', VN.size()));
    inf.close();
}

// �����ǰ׺�ķ�ͬʱ�ı�洢�ķ������ݽṹ���൱�������˱�ţ���˳��ȡ�����е��ս��
void toSimpleWenfa()
{
    map<char, vector<string> >::iterator iter;
    iter = input.begin();
    while (iter != input.end())
    {
        wf[VN[iter -> first]] = iter -> second;
        // ��ʼ�����ǰ׺�ķ�
        for (int i = 0; i < wf[VN[iter -> first]].size(); i++)
        {
            string value = wf[VN[iter -> first]][i];
            for (int j = 0; j < value.length(); j++)
            {
                allNode[liveLength+j].which = VN[iter -> first];
                allNode[liveLength+j].number = i;
                allNode[liveLength+j].where = j;
                allNode[liveLength+j].next = value[j];
                allNode[liveLength+j].flag = false;
                // �ռ��ս��
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
        wfLength += (iter -> second).size();
        iter++;
    }
    VT.insert(make_pair('#', VT.size()));
}

// �ϲ�����set�����е�Ԫ��
set<int> mergeToSet(set<int> first, set<int> second)
{
    set<int> result;
    set<int>::iterator firstIter = first.begin();
    set<int>::iterator secondIter = second.begin();
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

// Ϊgoto�ҵ���Ӧ��һ����ǰ׺
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

// ���ݻ�ǰ׺��which��where���ж��ǵڼ����ķ����������ݽṹΪwf
int doCountWenfa(int which, int number)
{
    int wenfaCount = 0;
    for (int i = 0; i < which; i++)
    {
        wenfaCount += wf[i].size();
    }
    return wenfaCount+number;
}

// ����allNode����allNode�л�ȡ��ص�Ԫ�أ�ʹ�õݹ�
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

    // ��֤closure���ϵ����
    set<int>::iterator iter = temp.begin();
    while (iter != temp.end())
    {
        iter++;
    }
    return temp;
}

// ���closure����
set<int> colsureIX(set<int> temp)
{
    int countnumber;
    set<int>::iterator iter;
    do {
        countnumber = temp.size();
        // ���ȱ���temp�е�����Ԫ�أ���ÿһ����ǰ׺�ķ�������չ
        set<int> middle;
        iter = temp.begin();
        while (iter != temp.end())
        {
            if (allNode[*iter].next >= 'A' && allNode[*iter].next <= 'Z')
            {
                middle = getStateFromAllNode(allNode[*iter].next, liveLength-2);
            }
            iter++;
        }
        temp = mergeToSet(temp, middle);
    } while (temp.size() > countnumber);
    return temp;
}

// goto����ʵ��
void gotoIX(set<int> firstState, int firstStateNumber)
{
    // ����ԭ��״̬��ÿһ��Ԫ�أ�����״̬ת��
    set<int>::iterator iter = firstState.begin();
    int stateLength = newDFA.size();
    while (iter != firstState.end())
    {
        if (!allNode[*iter].flag)
        {
            set<int> secondState;
            // Ѱ��ÿ�������Ԫ�ض�Ӧ��״̬
            secondState.insert(findOneState(allNode[*iter].which, allNode[*iter].number, allNode[*iter].where));
            secondState = colsureIX(secondState);
            // �ж��Ƿ�����״̬
            int i;
            for (i = 0; i < newDFA.size(); i++)
            {
                if (secondState == newDFA[i])
                {
                    break;
                }
            }
            if (i == newDFA.size()) {
                // ��״̬
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
            // ����ǹ�Լ��Ŀ�����ж���
            for (int i = 0; i < VT.size(); i++)
            {
                // ����������Ŀ�ʶ���ķ�
                if (allNode[*iter].which == VN.size()-1 && allNode[*iter].where == 1)
                {
                    LR0[firstStateNumber][VT['#']] = '!';
                } else
                {
                    LR0[firstStateNumber][i] = doCountWenfa(allNode[*iter].which, allNode[*iter].number)+'A';
                }
            }
        }
        iter++;
    }
}

// ���DFA
void compulateProjectList()
{
    // ���Ƚ�����DFA�ĳ�ʼ����������I0״̬���׸���ǰ׺�ķ����뽫��Ŀ���Ϲ淶����
    set<int> firstState;
    firstState.insert(liveLength-2);
    firstState = colsureIX(firstState);
    newDFA.push_back(firstState);
    gotoIX(firstState, 0);
}

// ���з���
void analysis()
{
    cout << "===========================================================" << endl;
    do {
        cout << "pls input the string waited for analyze, end by '#': ";
        cin >> waitForAnalysis;
    } while (waitForAnalysis[waitForAnalysis.length()-1] != '#');
    // ��ʼ��״̬ջ�ͷ���ջ
    symbol.push('#');
    state.push(0);
    // �ַ�����ɨ��λ��
    int where = 0, i;
    char temp;
    cout << setw(20) << "����ִ�й���" << endl;
    for (i = 0; i < waitForAnalysis.length(); i++)
    {
        char symbolTop = symbol.top();
        do {
            cout << setw(6) << state.top() << setw(6) << symbol.top() << setw(15) << waitForAnalysis.substr(i, waitForAnalysis.length()) << endl;
            // ���õ���ֵ
            temp = LR0[state.top()][VT[waitForAnalysis[i]]];
            // cout << "temp = " << temp << endl;
            if (temp == NULL)
            {
                break;
            }
            // ��״̬ջ��ջ�������봮�Ĵ����ַ����
            if (temp >= 'A' && temp <= 'Z')
            {
                // ����Լ��Ŀ
                // tempΪ����ʽ��ţ���ʱ����Ҫ���Ҳ���ʽ
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
                // temp - wenfaCount�Ƕ�Ӧ���ķ����ҵ����Ӧ���ķ�
                string wait = wf[j][temp-wenfaCount];
                // ����i�ҵ����Ӧ�ķ��ս��
                map<char, int>::iterator something = VN.begin();
                while (something != VN.end())
                {
                    if (something->second == j) break;
                    else something++;
                }
                char theVN =something->first;
                // ����ջ��ջ������ջ
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
                // ״̬ջ��ջ
                state.push(temp-'a');
                symbol.push(waitForAnalysis[i]);
            } else
            {
                break;
            }
        } while (temp != '!' && temp <= 'Z' && temp >= 'A');
        if (temp == NULL || temp == '!')
        {
            break;
        }
    }
    cout << endl << "ʶ������";
    // ɨ����ϣ��Ƿ��Ǹ��ķ��ķ��Ŵ����ж�
    if (i == waitForAnalysis.length()-1 && temp == '!')
    {
        cout << "yes" << endl;
    } else
    {
        cout << "no" << endl;
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

// ��ӡ����Ļ�ǰ׺
void printLive()
{
    cout << "===================================================================" << endl;
    cout << setw(40) <<"��ǰ׺�ķ�����" << endl;
    cout << setw(10) << "��ǰ׺���";
    cout << setw(10) << "�ķ����1";
    cout << setw(10) << "�ķ����2";
    cout << setw(12) << "��ǰ׺λ��";
    cout << setw(12) << "��ʶ���ַ�";
    cout << setw(12) << "��Լ��ǰ׺" << endl;
    for (int i = 0; i < liveLength; i++)
    {
        cout << setw(10) << i;
        cout << setw(10) << allNode[i].which;
        cout << setw(10) << allNode[i].number;
        cout << setw(12) << allNode[i].where;
        string flag = allNode[i].flag == true ? "true" : "false";
        cout << setw(12) << allNode[i].next;
        cout << setw(12) << flag;
        cout << endl;
    }
    cout << endl << "��ǰ׺�ķ���Ŀ��" << liveLength << endl;
    cout << "�ķ�����" << wfLength << endl;
    cout << endl << endl;
}

// ��ӡDFA
void printDFA()
{
    cout << "=======================" << endl;
    cout << setw(15) <<"DFA״̬����" << endl;
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
        cout << endl;
    }
    cout << endl << endl;
}

// ��ӡLR(0)������
void printLR0()
{
    cout << "=====================================================================" << endl;
    cout << setw(36) << "LR0������" << endl;
    cout << setw(6) << "state";
    map<char, int>::iterator iter1, iter2;
    for (int i = 0; i < VT.size(); i++)
    {
        iter1 = VT.begin();
        while (iter1 != VT.end())
        {
            if (iter1->second == i)
            {
                cout << setw(7) << iter1->first;
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
                cout << setw(7) << iter2->first;
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
                cout << setw(6) << LR0[i][j] - 'A' << setw(1) << 'r';
            } else if (LR0[i][j] >= 'a' && LR0[i][j] <= 'z')
            {
                cout << setw(6) << LR0[i][j] - 'a' << setw(1) << 's';
            } else if (LR0[i][j] == '!')
            {
                cout << setw(7) << '!';
            } else
            {
                cout << setw(7) << ' ';
            }
        }

        for (int j = VT.size(); j < VN.size()+VT.size(); j++)
        {
            if (LR0[i][j] >= 'a' && LR0[i][j] <= 'z')
            {
                cout << setw(7) << LR0[i][j] - 'a';
            } else
            {
                cout << setw(7) << ' ';
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
    printVNVT();
    printLive();
    compulateProjectList();
    printDFA();
    printLR0();
    analysis();
    return 0;
}
