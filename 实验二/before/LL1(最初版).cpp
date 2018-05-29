#include<iostream>
#include<fstream>

#include<cstring>
#include<string>

#include<cmath>

#include<algorithm>

#include<vector>
#include<map>
#include<set>

using namespace std;

// 提取所有的文法到fw中
map<char, vector<string> > fw;
// 判断是否含有“一部修”系统 todo, 当前是判断其是否为空的功能
map<char, bool> isEmpty;
// first集
map<char, vector<char> > first;
// follow集
map<char, vector<char> > follow;
// select集
map<map<char, string>, vector<char> > theselect;

// 读取文件
void readFile(string filename);

// 读取文法结构fw中的数据
void judgeEmpty();

// 填充"一部修"
void modifyIsEmpty();

// 递归求解第一个终结符
void getTheFirstWord(char word, char my);

// 求first集合
void getFirstMap();

// 递归求解follow集合
void getFllowFirstMap(char theOne, char theTwo);

// 求follow集合
void getFollowMap();

// 求select集合
void getSelectMap();

// 分析程序
void fenxi();

// 求两个vector<char>的交集
// vector<char> interSection(vector<char> x, vector<char> y);

// vector去重
vector<char> unique_element_in_vector(vector<char> x);

// 主函数
int main() {
    string filename = "./fw.txt";
    readFile(filename);
    judgeEmpty();
    modifyIsEmpty();
    getFirstMap();
    getFollowMap();
    fenxi();
    return 0;
}

void readFile(string filename) {
    string content;
    string cut;
    int temp;
    string tempString;
    ifstream inf("./wf.txt");
    if (!inf.is_open()) {
        cout << "没有成功打开文件" << endl;
    }
    while (getline(inf, content)) {
        //　截取－＞两端的字符串，　左边一定为非终结符，右边可以是终结符，　也可以为非终结符或者其组合
        bool flag = true;
        vector<string> value;
        content.erase(--content.end());
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
        fw.insert(make_pair(content[0], value));
        isEmpty.insert(make_pair(content[0], false));
    }
    inf.close();
}

void judgeEmpty() {
    map<char, vector<string> >::iterator iter;
    iter = fw.begin();
    while (iter != fw.end()) {
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

void modifyIsEmpty() {
    // 扫描所有的文法, 需要进行三次扫描
    map<char, vector<string> >::iterator iter;
    iter = fw.begin();
    while (iter != fw.end()) {
        vector<string> result = iter -> second;
        for (int i = 0; i < result.size(); i++) {
            // 判断是否含有"一部修", 如果有, 则将对应的值改为false
            if (strcmp(const_cast<char*>(result[i].c_str()), const_cast<char*>(string("^").c_str()))==0) {
                isEmpty[iter->first] = true;
            }
        }
        cout << endl;
        iter++;
    }
    // 首先堆isEmpty数据结构进行初始化
    map<char, bool>::iterator iter1;
    iter1 = isEmpty.begin();
    while (iter1 != isEmpty.end()) {
        cout << iter1 -> first << "  " << iter1 -> second << endl;
        iter1++;
    }
}

void getFirstMap() {
    map<char, vector<string> >::iterator iter;
    iter = fw.begin();
    while (iter != fw.end()) {
        vector<string> result = iter -> second;
        for (int i = 0; i < result.size(); i++) {
            getTheFirstWord(result[i][0], iter -> first);
        }
        iter++;
    }
    map<char, vector<char> >::iterator iter1;
    iter1 = first.begin();
    cout << "this is the value" << endl;
    while (iter1 != first.end()) {
        vector<char> result = iter1 -> second;
        cout << iter1 -> first << "         ";
        for (int i = 0; i < result.size(); i++) {
            cout << result[i] << "  ";
        }
        cout << endl;
        iter1++;
    }
}

void getFollowMap() {
    // 首先扫描所有的文法识别符号, 在fw的Map中
    map<char, vector<string> >::iterator iter, iter1;
    iter = fw.begin();
    iter1 = fw.begin();



    while (iter != fw.end()) {
        // TODO 首先将#加入所有非终结符号的follow集合
        follow[iter->first].push_back('#');
        vector<string> result = iter -> second;
        for (int i = 0; i < result.size(); i++) {
            // 取到某一个文法
            string detail = result[i];
            // 首先根据规则2的first集合, 初始化follow集合(这时候要看第一个字符到倒数第二个字符)
            for (int j = 0; j < detail.length()-1; j++) {
                if (detail[j] >= 'A' && detail[j] <= 'Z') {
                    if (detail[j+1] >= 'a' && detail[j+1] <= 'z' || detail[j+1] == '*' || detail[j+1] == '(' || detail[j+1] == ')' || detail[j+1] == '+' || detail[j+1] == '^') {
                        follow[detail[j]].push_back(detail[j+1]);
                    } else if (detail[j+1] >= 'A' && detail[j+1] <= 'Z') {
                        for (int k = 0; k < first[detail[j+1]].size(); k++) {
                            follow[detail[j]].push_back(first[detail[j+1]][k]);
                        }
                    }
                } else {
                    continue;
                }
            }
        }
        iter++;
    }

    cout << "==================" << endl;

    cout << "size =" << follow.size() << endl;

    cout << "==================" << endl;

    while (iter1 != fw.end()) {
        cout << "++++++++++++++++++++" << endl;
        cout << "first = " << iter1 -> first << endl;
        vector<string> result = iter1 -> second;
        for (int i = 0; i < result.size(); i++) {
            string detail = result[i];
            // 拿到该字符串的最后一个字符
            // cout << "detail = " << detail << endl;
            // cout << "length of detail = " << detail.length() << endl;
            // 从最后一个字符开始查询
            char temp = detail[detail.length()-1];
            cout << "temp = " << temp << endl;
            if (temp >= 'A' && temp <= 'Z') {
                // 规则3(1)情形: follow(A) 属于follow(B)的情形
                vector<char> itertmp = follow[temp];

                cout << itertmp.size() << endl;

                cout << "itertmp = " << itertmp.size() << endl;

                for (int j = 0; j < itertmp.size(); j++) {
                    follow[iter1->first].push_back(itertmp[j]);
                }

                // 规则3(2)情形
                vector<string> some = fw[temp];
                for (int k = 0; k < some.size(); k++) {
                    if (some[k] == "^") {
                        itertmp = follow[temp];
                        for (int j = 0; j < itertmp.size(); j++) {
                            follow[iter1->first].push_back(itertmp[j]);
                        }
                    } else {
                        continue;
                    }
                }
            }
        }
        iter1++;
    }

    // 输出follow集合相关的信息
    cout << "follow size is " << follow.size() << endl;

    map<char, vector<char> >::iterator iter2;
    iter2 = follow.begin();
    while (iter2 != follow.end()) {
        // 清空重复的元素
        sort(iter2 -> second.begin(), iter2 -> second.end());
        iter2 -> second.erase(unique(iter2 -> second.begin(), iter2 -> second.end()), iter2 -> second.end());
        // 输出follow信息
        vector<char> result = iter2 -> second;
        cout << "follow [" << iter2->first << "] = ";
        for (int i = 0; i < result.size(); i++) {
            cout << "   " << result[i];
        }
        cout << endl;
        iter2++;
    }
}

void getSelectMap() {

}

void getTheFirstWord(char simple, char my) {
    if ((simple >= 'a' && simple <= 'z') || simple == '^' || simple == '(' || simple == '+' || simple == '*' || simple == ')') {
        // 已经是终结符, 将其插入其中
        vector<char> value = first[my];
        value.push_back(simple);
        first[my] = value;
    }
    if (simple >= 'A' && simple <= 'Z') {
        // cout << endl << endl << endl;
        vector<string> temp = fw[simple];
        for (int i = 0; i < temp.size(); i++) {
            simple = temp[i][0];
            getTheFirstWord(simple, my);
        }
    }
}

void getFllowFirstMap(char theOne, char theTwo) {

}

vector<char> unique_element_in_vector(vector<char> x) {
    sort(x.begin(), x.end());
    x.erase(unique(x.begin(), x.end()), x.end());
    return x;
}

void fenxi() {

}
