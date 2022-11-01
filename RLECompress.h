//
// Created by Grigory on 31/10/2022.
//

#ifndef OTIK_ARCHIVE_RLECOMPRESS_H
#define OTIK_ARCHIVE_RLECOMPRESS_H

#include "Packer.h"


class RLECompress {
private:
    //some fields
    //some private methods if needed

public:
    int Compress(const string& file, const string& archiveName);
    void Extract(ifstream& archiveFile, file_header& header);


};


#endif //OTIK_ARCHIVE_RLECOMPRESS_H
