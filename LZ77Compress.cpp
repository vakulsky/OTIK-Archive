//
// Created by grigory on 03/11/22.
//

#include "LZ77Compress.h"


file_header LZ77Compress::buildHeader(const string& fileName){
    file_header header{};
    ifstream file;

    file.open(fileName, ios_base::in);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {

        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

        memset(&header, 0, sizeof(struct file_header));
        snprintf(header.signature, SIGNATURE_SZ, "%s", SIGN);
        snprintf(header.name, NAME_SZ, "%s", fileName.c_str());
        snprintf(header.version, VERSION_SZ, "%s", VERSION);
        snprintf(header.size, SIZE_SZ, "%d", fileSize);
        snprintf(header.algorithm, ALGORITHM_SZ, "%s", "3");
    }
    return header;

}


int LZ77Compress::Compress(const string& fileName, const string& archiveName, bool writeHeader){

}



void LZ77Compress::Extract(ifstream& archiveFile, file_header& header){

}




