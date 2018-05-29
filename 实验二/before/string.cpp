#include <iostream>
#include<fstream>

#include <string>
using namespace std;

void readFile(string filename) {
    string content;
    string cut;
    int temp;
    string tempString;
    ifstream inf("./string.txt");
    if (!inf.is_open()) {
        cout << "没有成功打开文件" << endl;
    }
    while (getline(inf, content)) {
	cout << content.length() << endl;
    }
    inf.close();
}

int main() {
    string filename = "./string.txt";
    readFile(filename);
    // modifyIsEmpty();
    return 0;
}
