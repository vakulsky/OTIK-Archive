//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <cmath>

#define VERSION "2"
#define SIGN "SVSA"
#define HEADER_SZ 128
#define SIGNATURE_SZ 8
#define NAME_SZ 50
#define VERSION_SZ 8
#define SIZE_SZ 12
#define TABLE_SZ 8
#define ALGORITHM_SZ 1
#define PADDING_SZ 41


using namespace std;


struct file_header {
    char signature[SIGNATURE_SZ];
    char name[NAME_SZ];
    char version[VERSION_SZ];
    char size[SIZE_SZ];
    char algorithm[ALGORITHM_SZ];
    char codeTableSize[TABLE_SZ];
    char padding[PADDING_SZ];

};

class Archiver {

private:
    vector<string> files; //headers to be written before every file in archive
    string archive_file;   // archive file path
    int algorithmCode;
public:
    Archiver(vector<string> &vec, string path) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        archive_file = path;
        algorithmCode = 0;
    }


    file_header buildHeader(const string file, const int codeTableSize);
    void Compress();
    void Extract(const string& archive_file);
    int CompressShannon();
    void ExtractShannon(const string& archive_file);
    void CompressWrapper();
    void ExtractWrapper();
    string buildShannonTable(const vector<pair<string, pair<int, string>>>& codes);

    void analyzeUTF8(vector<pair<string, pair<int, string>>>& symbols, const string& currentSymbol);
    void shannonCodes(vector<pair<string, pair<int, string>>>& symbols, int symbolsAmount);
    void writeShannon(const string& file, vector<pair<std::string, pair<int, std::string>>> &codes);


    static string get_file_name(string filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }

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

#endif //OTIK_ARCHIVE_ARCHIVER_H
