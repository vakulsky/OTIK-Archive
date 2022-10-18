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


    file_header buildHeader(string file);

    //pack single file to archive. Return: size in archive
    int Pack(const string& file, const string& archiveName);

    //get single file from archive
    void Unpack(FILE* archiveFile, file_header& header);

    static string getFileName(const string& filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }


};


#endif //OTIK_ARCHIVE_PACKER_H
