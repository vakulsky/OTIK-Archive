//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include <iostream>
#include <cstring>
#include <vector>

#define VERSION "1"
#define SIGN "SVSA"
#define HEADER_SZ 128
#define SIGNATURE_SZ 8
#define NAME_SZ 50
#define VERSION_SZ 8
#define SIZE_SZ 12
#define PADDING_SZ 50


using namespace std;


struct file_header {
    char signature[SIGNATURE_SZ];
    char name[NAME_SZ];
    char version[VERSION_SZ];
    char size[SIZE_SZ];
    char padding[PADDING_SZ];

};

class Archiver {

private:
    vector<string> files; //headers to be written before every file in archive
    string archive_file;   // archive file path
public:
    Archiver(vector<string> &vec, string path) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        archive_file = path;
    }


    file_header buildHeader(const string file);
    void Compress();
    void Extract(const string& archive_file);

    static string get_file_name(string filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }

};

#endif //OTIK_ARCHIVE_ARCHIVER_H
