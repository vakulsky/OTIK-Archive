//
// Created by Grigory on 18/10/2022.
//

#include "Packer.h"

using namespace std;

int Packer::Pack(const string& fileName, const string& archiveName, bool writeHeader, file_header& header) {

    ofstream archiveFile;
    ifstream file;
    char buff[1];  // buffer for reading 1 byte

    archiveFile.open(archiveName,ios::app);
    file.open(fileName);
    if(!archiveFile) {
        cout << "Can't open file " << archiveName << endl;
        return 0;
    }
    else if(!file) {
        cout << "Can't read file " << fileName << endl;
        return 0;
    }
    else{

        if(writeHeader) {
            //writing header
            archiveFile.write(header.signature, SIGNATURE_SZ);
            archiveFile.write(header.name, NAME_SZ);
            archiveFile.write(header.version, VERSION_SZ);
            archiveFile.write(header.size, SIZE_SZ);
            archiveFile.write(header.algorithm, ALGORITHM_SZ);
            archiveFile.write(header.padding, PADDING_SZ);
        }

        auto sizeStart = archiveFile.tellp();

        while (file.peek() != EOF) {
            file.read(buff, sizeof buff);
            archiveFile.write(buff, sizeof buff);
        }
        archiveFile << "\n";
        cout << "--- " << string(header.name) << " is packed ---" << endl;

        auto sizeEnd = archiveFile.tellp();
        archiveFile.close();

        return (int)(sizeEnd - sizeStart);
        
    }
}



void Packer::Unpack(ifstream& archiveFile, const string& fileName, int fileSize) {

    char buff[1];
    ofstream outputFile;
    outputFile.open(fileName);
    if(!outputFile) {
        cout << "Can't open file " << fileName << endl;
    }
    else {


        for(int i =0 ; i < fileSize; i++) {
            archiveFile.read(buff, 1);
            outputFile.write(buff, 1);
        }
        //reading '\n'
        archiveFile.read(buff, 1);
        cout << "--- " << fileName << " is extracted ---" << endl;

        outputFile.close();
    }
}