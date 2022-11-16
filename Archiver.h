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
    LZ77
};

enum ErrorCorrection{
    NONE,
    HAMMING,
    REEDSOL
};

class Archiver {

private:
    vector<string> files; //headers to be written before every file in archive
    string archiveFileName;   // archive file path
    Packer packer;
    Shannon shannonCompressor;
    RLECompress RLECompressor;
    LZ77Compress LZ77Compressor;
    HammingCode hammingCodeProtector;
    ReedSolomonWrapper reedSolomonWrapper;

    file_header BuildHeader(const string& fileName, CompressType compressType, ErrorCorrection errorCorrection);
    void IntelligentArchive(const string& inFileName, const string& outFileName);
    static void WriteHeaderToFile(const file_header& header, const string& outFileName);
    static void CopyToFile(const string& from, const string& to);
    static int GetFileSize(const string& fileName);
    file_header ReadHeader(const string& fileName);
    bool CheckHeader(const file_header& header);
public:
    Archiver(vector<string> &vec, string path) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        archiveFileName = path;
    }

    void Compress(CompressType, ErrorCorrection dataProtectionType, ErrorCorrection headerProtectionType);
    void Extract(const string& archive_file);





    static string get_file_name(string filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }

};

#endif //OTIK_ARCHIVE_ARCHIVER_H
