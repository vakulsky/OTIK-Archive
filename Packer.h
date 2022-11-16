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

    //pack single inFileName to archive. Return: file_size in archive
    void Pack(const string& inFileName, const string& outFileName);

    //get single file from archive
    void Unpack(const string& inFileName, file_header& header);

    static string getFileName(const string& filename) {
        return filename.substr(filename.find_last_of("\\") + 1, filename.size());
    }


};


#endif //OTIK_ARCHIVE_PACKER_H
