//
// Created by Grigory on 18/10/2022.
//

#include "Packer.h"

using namespace std;

int Packer::Pack(const string& fileName, const string& archiveName) {

    ofstream archiveFile;
    ifstream file;
    file_header header{};
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
        archiveFile.seekp(0, ios_base::end);

        //writing header
        archiveFile.write(header.signature, sizeof(header.signature));
        archiveFile.write(header.name, sizeof(header.name));
        archiveFile.write(header.version, sizeof(header.version));
        archiveFile.write(header.size, sizeof(header.size));
        archiveFile.write(header.algorithm, sizeof(header.algorithm));
        archiveFile.write(header.padding, sizeof(header.padding));

        auto size = archiveFile.tellp();

        while (!file.eof()) {
            file.read(buff, sizeof buff);
            archiveFile.write(buff, sizeof buff);
        }
        cout << "--- " << string(header.name) << " is packed ---" << endl;

        size = archiveFile.tellp() - size;
        archiveFile.close();

        return size;
        
    }
}


file_header Packer::buildHeader(const string& fileName)
{
    file_header header{};
    ifstream file;

    file.open(fileName);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {

        int fileSize = file.tellg();

        string name = Packer::getFileName(fileName);  // get file name

        memset( &header, 0, sizeof( struct file_header ) );
        snprintf( header.signature, SIGNATURE_SZ, "%s", SIGN  );
        snprintf( header.name, NAME_SZ, "%s", name.c_str()  );
        snprintf( header.version, VERSION_SZ, "%s", VERSION );
        snprintf( header.size, SIZE_SZ, "%d", fileSize );
        snprintf( header.algorithm, ALGORITHM_SZ, "%d", 0 );
    }

    return header;
}


void Packer::Unpack(ifstream& archiveFile, file_header& header) {

    char buff[1];
    ofstream outputFile;
    outputFile.open(header.name);
    if(!outputFile) {
        cout << "Can't open file " << header.name << endl;
    }
    else {

        while (!archiveFile.eof()) {
            archiveFile.read(buff, sizeof buff);
            outputFile.write(buff, sizeof buff);
        }
        cout << "--- " << header.name << " is extracted ---" << endl;

        outputFile.close();
    }
}