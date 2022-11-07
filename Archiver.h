//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include "LZ77Compress.h"
#include "CompressType.h"


using namespace std;

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

    void Compress(vector<CompressType> algorithms);
    void Extract(const string& archive_file);
    void intelligentArchive(const string& fileName, file_header& header);
    file_header buildHeader(const string& fileName, const vector<CompressType>& algorithms);


    static string get_file_name(string filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }

};

#endif //OTIK_ARCHIVE_ARCHIVER_H
