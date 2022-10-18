//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include "Packer.h"



using namespace std;

enum CompressType{
    PACK,
    SHANNON,
    INTELLIGENT
};

class Archiver {

private:
    vector<string> files; //headers to be written before every file in archive
    string archive_file;   // archive file path
    int algorithmCode;
    Packer packer;
    Shannon shannonCompressor;
public:
    Archiver(vector<string> &vec, string path) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        archive_file = path;
        algorithmCode = 0;
        packer = new Packer(files, archive_file);
        shannonCompressor = new Shannon(files, archive_file);
    }

    void Compress(CompressType);
    void Extract(const string& archive_file);
    void intelligentArchive();



    static string get_file_name(string filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }

};

#endif //OTIK_ARCHIVE_ARCHIVER_H
