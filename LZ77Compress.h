//
// Created by grigory on 03/11/22.
//

#ifndef OTIK_ARCHIVE_LZ77COMPRESS_H
#define OTIK_ARCHIVE_LZ77COMPRESS_H

#include "RLECompress.h"

class LZ77Compress {

private:


    file_header buildHeader(const string& fileName);

public:

    int Compress(const string& fileName, const string& archiveName, bool writeHeader);
    void Extract(ifstream& archiveFile, file_header& header);



};


#endif //OTIK_ARCHIVE_LZ77COMPRESS_H
