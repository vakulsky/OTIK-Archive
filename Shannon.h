//
// Created by Grigory on 18/10/2022.
//

#ifndef OTIK_ARCHIVE_SHANNON_H
#define OTIK_ARCHIVE_SHANNON_H

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <cmath>
#include "fileHeader.h"

using namespace std;

class Shannon {


    int CompressShannon();
    void ExtractShannon(const string& archive_file);
    void CompressWrapper();
    void ExtractWrapper();
    string buildShannonTable(const vector<pair<string, pair<int, string>>>& codes);

    void analyzeUTF8(vector<pair<string, pair<int, string>>>& symbols, const string& currentSymbol);
    void shannonCodes(vector<pair<string, pair<int, string>>>& symbols, int symbolsAmount);
    void writeShannon(const string& file, vector<pair<std::string, pair<int, std::string>>> &codes);


    static vector<string> divideString(const string& text){
        vector<string> array;
        for(size_t i = 0; i < text.length();)
        {
            int cplen = 1;
            if((text[i] & 0xf8) == 0xf0) cplen = 4;
            else if((text[i] & 0xf0) == 0xe0) cplen = 3;
            else if((text[i] & 0xe0) == 0xc0) cplen = 2;
            if((i + cplen) > text.length()) cplen = 1;

            array.emplace_back(text.substr(i, cplen));
            i += cplen;
        }
        return array;
    }

};


#endif //OTIK_ARCHIVE_SHANNON_H
