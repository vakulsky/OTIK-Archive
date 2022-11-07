//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Archiver::Compress(vector<CompressType> algorithms) {
    string fileIN;
    string fileOUT;
    bool writeHeader;


    for(const auto& file : files){
        file_header header = buildHeader(file, algorithms);

        for(int i = 0; i < algorithms.size(); i++) {

            fileIN = file + to_string(i-1);
            fileOUT = file + to_string(i);
            writeHeader = false;

            if(i == 0){
                fileIN = file;
                fileOUT = file + to_string(i);
                writeHeader = false;
            }

            if(i == algorithms.size()-1){
                fileIN = file + to_string(i-1);
                fileOUT = archive_file;
                writeHeader = true;
            }

            if(algorithms.size() == 1){
                fileIN = file;
                fileOUT = archive_file;
                writeHeader = true;

            }

            switch (algorithms[i]) {

                case PACK:
                    packer.Pack(fileIN, fileOUT, writeHeader, header);
                    break;

                case SHANNON:
                    shannonCompressor.Compress(fileIN, fileOUT, writeHeader, header);
                    break;

                case RLE:
                    RLECompressor.Compress(fileIN, fileOUT, writeHeader, header);
                    break;

                case LZ77:
                    LZ77Compressor.Compress(fileIN, fileOUT, writeHeader, header);
                    break;

                case INTELLIGENT:
                    intelligentArchive(file, header);
            }
        }
    }


}





    void Archiver::Extract(const string& archiveName){

        ifstream archiveFile;

        archiveFile.open(archiveName);
        if(!archiveFile) {
            cout << "Can't read file " << archiveName << endl;
        }
        else {
            file_header header{};

            archiveFile.seekg(0, ios_base::beg);


            while (!archiveFile.eof() && archiveFile.peek() != EOF) {

                //read header from archive
                memset(&header, 0, sizeof(struct file_header));
                archiveFile.read(header.signature, SIGNATURE_SZ);
                archiveFile.read(header.name, NAME_SZ);
                archiveFile.read(header.version, VERSION_SZ);
                archiveFile.read(header.size, SIZE_SZ);
                archiveFile.read(header.algorithm, ALGORITHM_SZ);
                archiveFile.read(header.padding, PADDING_SZ);   //going through padding without reading

                /////
                cout << "DEBUG | (sign from file): " << string(header.signature) << endl;
                cout << "DEBUG | (version from file): " << string(header.version) << endl;
                cout << "DEBUG | (alg code from file): " << string(header.algorithm) << endl;
                cout << "DEBUG | (size code from file): " << string(header.size) << endl;
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
                }
                else if(strcmp(header.algorithm, "0") == 0)
                    packer.Unpack(archiveFile, header);
                else if(strcmp(header.algorithm, "2") == 0)
                    RLECompressor.Extract(archiveFile, header);
                else if(strcmp(header.algorithm, "3") == 0)
                    LZ77Compressor.Extract(archiveFile, header);
                else {
                    cout << "Error: Invalid algorithm code!" << endl;
                    break;
                }


            }
            archiveFile.close();
        }

    }

void Archiver::intelligentArchive(const string& fileName, file_header& header){


    int packSize = packer.Pack(fileName, archive_file+"_packTMP", false, header),
        shannonSize = shannonCompressor.Compress(fileName, archive_file+"_shannonTMP", false, header),
        RLESize = RLECompressor.Compress(fileName, archive_file+"_rleTMP", false, header),
        LZ77Size = LZ77Compressor.Compress(fileName, archive_file+"_lz77TMP", false, header);

    if( packSize <= shannonSize &&  packSize <= RLESize  &&  packSize <= LZ77Size){
        packer.Pack(fileName, archive_file, true, header);
        remove((archive_file+"_shannonTMP").c_str());
        remove((archive_file+"_packTMP").c_str());
        remove((archive_file+"_rleTMP").c_str());
        remove((archive_file+"_lz77TMP").c_str());
    }

    if( shannonSize <= RLESize &&  shannonSize <= packSize &&  shannonSize <= LZ77Size){
        shannonCompressor.Compress(fileName, archive_file, true, header);
        remove((archive_file+"_shannonTMP").c_str());
        remove((archive_file+"_packTMP").c_str());
        remove((archive_file+"_rleTMP").c_str());
        remove((archive_file+"_lz77TMP").c_str());
    }

    if( RLESize <= shannonSize &&  RLESize <= packSize &&  RLESize <= LZ77Size){
        RLECompressor.Compress(fileName, archive_file, true, header);
        remove((archive_file+"_shannonTMP").c_str());
        remove((archive_file+"_packTMP").c_str());
        remove((archive_file+"_rleTMP").c_str());
        remove((archive_file+"_lz77TMP").c_str());
    }

    if( LZ77Size <= shannonSize &&  LZ77Size <= packSize &&  LZ77Size <= RLESize){
        LZ77Compressor.Compress(fileName, archive_file, true, header);
        remove((archive_file+"_shannonTMP").c_str());
        remove((archive_file+"_packTMP").c_str());
        remove((archive_file+"_rleTMP").c_str());
        remove((archive_file+"_lz77TMP").c_str());
    }
}


file_header Archiver::buildHeader(const string& fileName, const vector<CompressType>& algorithms)
{
    file_header header{};
    ifstream file;
    string algos;

    file.open(fileName);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {

        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

        string name = Packer::getFileName(fileName);  // get file name
        
        for(auto algo : algorithms){
            algos.append(to_string(algo));
        }

        memset( &header, 0, sizeof( struct file_header ) );
        snprintf( header.signature, SIGNATURE_SZ, "%s", SIGN  );
        snprintf( header.name, NAME_SZ, "%s", name.c_str()  );
        snprintf( header.version, VERSION_SZ, "%s", VERSION );
        snprintf( header.size, SIZE_SZ, "%d", fileSize );
        snprintf( header.algorithm, ALGORITHM_SZ, "%s", algos.c_str() );
    }

    return header;
}






