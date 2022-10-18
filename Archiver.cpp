//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Archiver::Compress(CompressType type) {
    switch (type) {
        case PACK:
            for(auto file : files)
                packer.Pack(file, archive_file);
            break;
        case SHANNON:
            for(auto file : files)
                shannonCompressor.Compress(file, archive_file);

            break;
        case INTELLIGENT:
            intelligentArchive();
    }
}





    void Archiver::Extract(const string& archive_file){
        FILE *archiveFile = fopen(archive_file.c_str(), "rb");   // open archive file
        if(!archiveFile) {
            cout << "Can't open archive file " << archive_file << endl;
        }
        else {

            char byte[1];
            file_header header{};

            fseek(archiveFile, 0, SEEK_SET);

            while (fread(byte, 1, 1, archiveFile) == 1) {

                //going 1 byte back after checking in "while"
                fseek(archiveFile, -1, SEEK_CUR);

                //read header from archive
                memset(&header, 0, sizeof(struct file_header));
                fread(header.signature, SIGNATURE_SZ, 1, archiveFile);
                fread(header.name, NAME_SZ, 1, archiveFile);
                fread(header.version, VERSION_SZ, 1, archiveFile);
                fread(header.size, SIZE_SZ, 1, archiveFile);
                fread(header.algorithm, ALGORITHM_SZ, 1, archiveFile);

                fseek(archiveFile, PADDING_SZ, SEEK_CUR);   //going through padding without reading


                /////
                cout << "DEBUG | (sign from file): " << string(header.signature) << endl;
                cout << "DEBUG | (version from file): " << string(header.version) << endl;
                cout << "DEBUG | (alg code from file): " << string(header.algorithm) << endl;
                /////

                if (strcmp(header.signature, SIGN) != 0) {
                    cout << "Error: Signature mismatch!" << endl;
                    break;
                }


                if (strcmp(header.version, VERSION) != 0) {
                    cout << "Error: Incompatible version!" << endl;
                    break;
                }

                if (strcmp(header.algorithm, "1") == 0) {
                    shannonCompressor.Extract(archiveFile, header);
                    break;
                }
                else if(strcmp(header.algorithm, "0") == 0)
                    packer.Unpack(archiveFile, header);
                else {
                    cout << "Error: Invalid algorithm code!" << endl;
                    break;
                }


            }
            fclose(archiveFile);
        }

    }

void Archiver::intelligentArchive(){

    //todo intelligentArchive

}




