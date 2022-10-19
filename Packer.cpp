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

        header  = buildHeader(fileName);

        //writing header
        archiveFile.write(header.signature, SIGNATURE_SZ);
        archiveFile.write(header.name, NAME_SZ);
        archiveFile.write(header.version, VERSION_SZ);
        archiveFile.write(header.size, SIZE_SZ);
        archiveFile.write(header.algorithm, ALGORITHM_SZ);
        archiveFile.write(header.padding, PADDING_SZ);

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


file_header Packer::buildHeader(const string& fileName)
{
    file_header header{};
    ifstream file;

    file.open(fileName);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {

        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

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


        for(int i =0 ; i < atoi(header.size); i++) {
            archiveFile.read(buff, 1);
            outputFile.write(buff, 1);
        }
        cout << "--- " << header.name << " is extracted ---" << endl;

        outputFile.close();
    }
}