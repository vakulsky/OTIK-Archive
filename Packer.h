//
// Created by Grigory on 18/10/2022.
//

#ifndef OTIK_ARCHIVE_PACKER_H
#define OTIK_ARCHIVE_PACKER_H

#include "Shannon.h"


class Packer {
private:

public:
    Packer() {};

    //pack single file to archive. Return: size in archive
    int Pack(const string& file, const string& archiveName, bool writeHeader, file_header& header);

    //get single file from archive
    void Unpack(ifstream& archiveFile, const string& fileName, int fileSize);

    static string getFileName(const string& filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }


};


#endif //OTIK_ARCHIVE_PACKER_H
