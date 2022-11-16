//
// Created by Grigory on 18/10/2022.
//

#include "Packer.h"

using namespace std;

void Packer::Pack(const string& inFileName, const string& outFileName) {

    ofstream archiveFile;
    ifstream file;
    file_header header{};
    char buff[1];  // buffer for reading 1 byte

    archiveFile.open(outFileName, ios::app);
    file.open(inFileName);
    if(!archiveFile) {
        cout << "Can't open file " << outFileName << endl;

    }
    else if(!file) {
        cout << "Can't read file " << inFileName << endl;

    }
    else{

        while (file.peek() != EOF) {
            file.read(buff, sizeof buff);
            archiveFile.write(buff, sizeof buff);
        }
        archiveFile << "\n";
        cout << "--- " << string(header.name) << " is packed ---" << endl;

        auto sizeEnd = archiveFile.tellp();
        archiveFile.close();
    }
}


void Packer::Unpack(const string& inFileName, const int inFileSize, const string& outFileName) {

    char buff[1];
    ofstream outputFile;
    outputFile.open(header.name);
    if(!outputFile) {
        cout << "Can't open file " << header.name << endl;
    }
    else {


        for(int i =0 ; i < atoi(header.file_size); i++) {
            archiveFile.read(buff, 1);
            outputFile.write(buff, 1);
        }
        //reading '\n'
        archiveFile.read(buff, 1);
        cout << "--- " << header.name << " is extracted ---" << endl;

        outputFile.close();
    }
}