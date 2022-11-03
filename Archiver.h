//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include "LZ77Compress.h"



using namespace std;

enum CompressType{
    PACK,
    SHANNON,
    INTELLIGENT,
    RLE,
    LZ77,
    TESTALL
};

class Archiver {

private:
    vector<string> files; //headers to be written before every file in archive
    string archive_file;   // archive file path
    Packer packer;
    Shannon shannonCompressor;
    RLECompress RLECompressor;
    LZ77Compress LZ77Compressor;
public:
    Archiver(vector<string> &vec, string path) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        archive_file = path;
    }

    void Compress(CompressType);
    void Extract(const string& archive_file);
    void intelligentArchive();
    void compressAllMethods();



    static string get_file_name(string filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }

};

#endif //OTIK_ARCHIVE_ARCHIVER_H
