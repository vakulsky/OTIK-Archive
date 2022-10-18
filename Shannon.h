//
// Created by Grigory on 18/10/2022.
//

#ifndef OTIK_ARCHIVE_SHANNON_H
#define OTIK_ARCHIVE_SHANNON_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <cmath>
#include "fileHeader.h"
#include <algorithm>

using namespace std;

class Shannon {
private:
    //{symbol, {frequency, code}}
    vector<pair<string, pair<int, string>>> codes;
    int symbolsAmount;

    void analyzeUTF8(const string& currentSymbol);
    void shannonCodes();
    int writeToFile(const string& file, const string& archiveName, file_header& header);
    file_header buildHeader(const string& file);


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

    void parseCode(const string& str){
        codes.emplace_back(make_pair(str.substr(0, 1),
                                     make_pair(1,
                                                  str.substr(2, str.find('\n')-2))));

    }

public:
    int Compress(const string& file, const string& archiveName);
    void Extract(FILE* archiveFile, file_header& header);




    //comparators to sort symbols array
    struct frequencyDescending
    {
        inline bool operator() (const pair<string, pair<int, string>>& a, const pair<string, pair<int, string>>& b) {
            return (a.second.first > b.second.first);
        }
    };

};


#endif //OTIK_ARCHIVE_SHANNON_H
