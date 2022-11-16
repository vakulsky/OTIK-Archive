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
    RLE,
    LZ77,
    INTELLIGENT
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
    Shannon shannonCompressor;
    RLECompress RLECompressor;
    LZ77Compress LZ77Compressor;
    HammingCode hammingCodeProtector;
    ReedSolomonWrapper reedSolomonWrapper;

    const int protectedHeaderSize = HEADER_SZ + 6;  // code_length + fec_length from ReedSolomonWrapper functions

    static file_header BuildHeader(const string& fileName,
                                   CompressType compressType,
                                   ErrorCorrection errorCorrection,
                                   int compressedDataSize,
                                   int headerProtectionFlag);

    CompressType IntelligentArchive(const string& inFileName, const string& outFileName);
    static void WriteHeaderToFile(const file_header& header, const string& outFileName);
    static void CopyToFile(const string& from, long startPosition, int copySize, const string& to);
    static int GetFileSize(const string& fileName);
    file_header ReadHeader(const string& fileName, long position);
    bool CheckHeader(const file_header& header);
    int RemoveFiles(const vector<string>& files);

public:
    Archiver(vector<string> &vec, string path) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        archiveFileName = path;
    }

    void Compress(CompressType, ErrorCorrection dataProtectionType, ErrorCorrection headerProtectionType);
    void Extract(const string& inFileName);

};

#endif //OTIK_ARCHIVE_ARCHIVER_H
