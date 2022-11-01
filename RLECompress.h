//
// Created by Grigory on 31/10/2022.
//

#ifndef OTIK_ARCHIVE_RLECOMPRESS_H
#define OTIK_ARCHIVE_RLECOMPRESS_H

#include "Packer.h"


class RLECompress {
private:

    int     Lmax,
            a,  //Lmax - 3
            b;  //Lmax - 1

    file_header buildHeader(const string& fileName);
    void writeToArchive( ofstream& archive, const string& sequenceString);
    void writeToArchive( ofstream& archive, const char repeatedChar, int count);

public:
    RLECompress(){
        Lmax = 255;
        a = 3;
        b = 1;
    }

    int Compress(const string& fileName, const string& archiveName);
    void Extract(ifstream& archiveFile, file_header& header);


};


#endif //OTIK_ARCHIVE_RLECOMPRESS_H
