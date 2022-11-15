//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Archiver::Compress(CompressType compressType, ErrorCorrection dataProtectionType, ErrorCorrection headerProtectionType) {

    for(auto file : files){
        //build header for file
        file_header header = buildHeader(file, compressType, dataProtectionType);

        //use different name for temp compressed file
        string tempFileName = file.append("_compressed");
        string toArchiveFileName;

        //compress file using preferred algorithm
        switch (compressType) {
            case PACK:
                packer.Pack(file, tempFileName);
                break;

            case SHANNON:
                shannonCompressor.Compress(file, tempFileName);
                break;

            case RLE:
                RLECompressor.Compress(file, tempFileName);
                break;

            case LZ77:
                LZ77Compressor.Compress(file, tempFileName);
                break;

            case INTELLIGENT:
                intelligentArchive(file, tempFileName);

        }

        //protect data if needed
        switch (dataProtectionType) {
            case HAMMING:
                toArchiveFileName = file.append("_protected");
                hammingCodeProtector.AddProtection(tempFileName, toArchiveFileName);
                break;

            case REEDSOL:
                toArchiveFileName = file.append("_protected");
                //todo add external lib with reed-solomon algo(filein, fileout)
                break;

            case NONE:
                toArchiveFileName = tempFileName;
                break;
        }

        //protect header if needed
        string tempHeaderFileName = "header.header";
        string protectedHeaderFileName = "header_protected.header";

        //write header to temp file to protect
        WriteHeaderToFile(header, tempHeaderFileName);
        switch (headerProtectionType) {
            case HAMMING:
                hammingCodeProtector.AddProtection(tempHeaderFileName, protectedHeaderFileName);
                break;

            case REEDSOL:
                //todo add external lib with reed-solomon algo(filein, fileout)
                break;

            case NONE:
                //just copy to another file
                protectedHeaderFileName = tempHeaderFileName;
                break;
        }

        //write header + filedata to archive file
        CopyToFile(protectedHeaderFileName, archiveFileName);
        CopyToFile(toArchiveFileName, archiveFileName);
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
            archiveFile.read(header.errorcorr, ERRORCORR_SZ);
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

void Archiver::intelligentArchive(const string& inFileName, const string& outFileName){
   int packSize, shannonSize, RLESize, LZ77Size;

    packer.Pack(inFileName, outFileName + "_TMP");
    packSize = getFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    shannonCompressor.Compress(inFileName, outFileName + "_TMP");
    shannonSize = getFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    RLECompressor.Compress(inFileName, outFileName + "_TMP");
    RLESize = getFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    LZ77Compressor.Compress(inFileName, outFileName + "_TMP");
    LZ77Size = getFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());



    if( packSize <= shannonSize &&  packSize <= RLESize  &&  packSize <= LZ77Size){
        packer.Pack(inFileName, outFileName);
    }

    if( shannonSize <= RLESize &&  shannonSize <= packSize &&  shannonSize <= LZ77Size){
        shannonCompressor.Compress(inFileName, outFileName);
    }

    if( RLESize <= shannonSize &&  RLESize <= packSize &&  RLESize <= LZ77Size){
        RLECompressor.Compress(inFileName, outFileName);
    }

    if( LZ77Size <= shannonSize &&  LZ77Size <= packSize &&  LZ77Size <= RLESize){
        LZ77Compressor.Compress(inFileName, outFileName);
    }


}


file_header Archiver::buildHeader(const string& fileName, CompressType compressType, ErrorCorrection errorCorrection){
    file_header header{};
    ifstream file;

    file.open(fileName, ios_base::in);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {
        //file size
        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

        memset(&header, 0, sizeof(struct file_header));
        snprintf(header.signature, SIGNATURE_SZ, "%s", SIGN);
        snprintf(header.name, NAME_SZ, "%s", fileName.c_str());
        snprintf(header.version, VERSION_SZ, "%s", VERSION);
        snprintf(header.size, SIZE_SZ, "%d", fileSize);
        snprintf(header.algorithm, ALGORITHM_SZ, "%u", compressType);
        snprintf(header.errorcorr, ERRORCORR_SZ, "%u", errorCorrection);
    }
    return header;

}


void Archiver::WriteHeaderToFile(const file_header& header, const string& outFileName){

    ofstream outFile;

    outFile.open(outFileName, ios::app | ios::binary);

    if(!outFile){
        cout << "Error opening outFile!" << endl;
    }
    else{

        //writing header
        outFile.write(header.signature, SIGNATURE_SZ);
        outFile.write(header.name, NAME_SZ);
        outFile.write(header.version, VERSION_SZ);
        outFile.write(header.size, SIZE_SZ);
        outFile.write(header.algorithm, ALGORITHM_SZ);
        outFile.write(header.errorcorr, ERRORCORR_SZ);
        outFile.write(header.padding, PADDING_SZ);
    }
    outFile.close();
}

void Archiver::CopyToFile(const string& from, const string& to){

    ofstream outFile;
    ifstream inFile;

    outFile.open(to, ios::app | ios::binary);

    if(!outFile){
        cout << "Error opening outFile!" << endl;
    }
    else{
        inFile.open(from, ios::binary);
        //writing data
        std::copy(
                std::istreambuf_iterator<char>(inFile),
                std::istreambuf_iterator<char>( ),
                std::ostreambuf_iterator<char>(outFile));
        inFile.close();
    }
    outFile.close();
}


int Archiver::getFileSize(const string& fileName) {
    ifstream file;

    file.open(fileName, ios::in);
    if (!file) {
        cout << "Can't open file " << fileName << endl;
        return 0;
    } else {
        //file size
        file.seekg(0, std::ios::end);
        int fileSize = (int) (file.tellg());
        return fileSize;
    }


}

