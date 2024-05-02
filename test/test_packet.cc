
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Packet.h"


using namespace std;

int main() {
    string filename;
    cin >> filename;
    ifstream file(filename);
    std::vector<char> fileContents((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
    file.close();


    auto rtn = Packet::GeneratePackets(fileContents.data(), fileContents.size());
    cout << rtn.size();

    ofstream of("2.txt");
    for (int i = 0; i < rtn.size(); ++i) {
        string s = string(rtn[i].GetContent(), rtn[i].GetContent() + rtn[i].GetContentLen());
        of << s;
    }
    of.flush();
    of.close();
}