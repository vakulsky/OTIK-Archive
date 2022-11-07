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

    void writeToArchive( ofstream& archive, const string& sequenceString);
    void writeToArchive( ofstream& archive, const char repeatedChar, int count);

public:
    RLECompress(){
        Lmax = 9;
        a = 3;
        b = 1;
    }

    int Compress(const string& fileName, const string& archiveName, bool writeHeader, file_header& header);
    void Extract(ifstream& archiveFile, const string& fileName);


};


#endif //OTIK_ARCHIVE_RLECOMPRESS_H
