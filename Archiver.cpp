//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Archiver::Compress(CompressType compressType, ErrorCorrection dataProtectionType, ErrorCorrection headerProtectionType) {

    for(auto file : files){

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
                IntelligentArchive(file, tempFileName);

        }

        //protect data if needed
        switch (dataProtectionType) {
            case HAMMING:
                toArchiveFileName = file.append("_protected");
                hammingCodeProtector.AddProtection(tempFileName, toArchiveFileName);
                break;

            case REEDSOL:
                toArchiveFileName = file.append("_protected");
                reedSolomonWrapper.AddProtection(tempFileName, toArchiveFileName);
                break;

            case NONE:
                toArchiveFileName = tempFileName;
                break;
        }

        //build header for file
        file_header header = BuildHeader(file,
                                         compressType,
                                         dataProtectionType,
                                         GetFileSize(toArchiveFileName));

        //protect header if needed
        string tempHeaderFileName = "header.header";
        string protectedHeaderFileName = "header_protected.header";

        //write header to temp file to protect
        WriteHeaderToFile(header, tempHeaderFileName);
        switch (headerProtectionType) {
            case REEDSOL:
                reedSolomonWrapper.AddProtection(tempHeaderFileName, protectedHeaderFileName);
                break;

            case NONE:
                //just copy to another file
                protectedHeaderFileName = tempHeaderFileName;
                break;
            case HAMMING:
                //NOT IMPLEMENTED YET
                protectedHeaderFileName = tempHeaderFileName;
                cout << "WARNING: Header protection using Hamming codes is NOT IMPLEMENTED YET. Header won't be protected!" << endl;
                break;
        }

        //write header + filedata to archive file
        CopyToFile(protectedHeaderFileName, 0, 0, archiveFileName);
        CopyToFile(toArchiveFileName, 0, 0, archiveFileName);

        //todo remove temp files
    }


}


void Archiver::Extract(const string& archiveFileName){
    ifstream archiveFile;

    auto archiveSize = GetFileSize(archiveFileName);
    long currentPosition = 0;

    //read archive to its end
    while(currentPosition < archiveSize){
        file_header header = ReadHeader(archiveFileName, currentPosition);
        currentPosition+=HEADER_SZ;

        //checking header
        if(!CheckHeader(header)){
            //something is wrong with header. trying to read protected version of it
            string protectedHeaderFileName = "protected.header",
                   recoveredHeaderFileName = "recovered.header";

            CopyToFile(archiveFileName, currentPosition, protectedHeaderSize, protectedHeaderFileName);
            reedSolomonWrapper.RemoveProtection(protectedHeaderFileName, recoveredHeaderFileName);

            file_header retrievedHeader = ReadHeader(recoveredHeaderFileName, 0);

            //todo remove temp files

            if(CheckHeader(retrievedHeader)){
                //header is good
                header = retrievedHeader;
                currentPosition+=protectedHeaderSize;
            }
            else{
                cout << "Cannot recover header. EXITING..." << endl;
                break;
            }
        }

        //if here then header is OK

        string protectedDataFileName = "protected.data",
               recoveredDataFileName = "recovered.data";
        if(strcmp(header.errorcorr, "1") == 0){

            //data protected using Hamming Codes
            CopyToFile(archiveFileName, currentPosition,
                       atoi(header.data_size),
                       protectedDataFileName);
            currentPosition += atoi(header.data_size);

            //recovering data
            hammingCodeProtector.RemoveProtection(protectedDataFileName, recoveredDataFileName);
        }
        else if(strcmp(header.errorcorr, "2") == 0){
            //data protected using Reed-Solomon Codes

            CopyToFile(archiveFileName, currentPosition,
                       atoi(header.data_size),
                       protectedDataFileName);
            currentPosition += atoi(header.data_size);

            reedSolomonWrapper.RemoveProtection(protectedDataFileName, recoveredDataFileName);
        }
        else{
            //data wasn't protected. Writing to destination file
            CopyToFile(archiveFileName, currentPosition,
                       atoi(header.data_size),
                       string(header.name));
            currentPosition += atoi(header.data_size);
        }


        if(strcmp(header.algorithm, "1") == 0)
            shannonCompressor.Extract(archiveFile, header);

        else if(strcmp(header.algorithm, "2") == 0)
            RLECompressor.Extract(archiveFile, header);

        else if(strcmp(header.algorithm, "3") == 0)
            LZ77Compressor.Extract(archiveFile, header);


        //todo remove temp files
    }
}

void Archiver::IntelligentArchive(const string& inFileName, const string& outFileName){
   int packSize, shannonSize, RLESize, LZ77Size;

    packer.Pack(inFileName, outFileName + "_TMP");
    packSize = GetFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    shannonCompressor.Compress(inFileName, outFileName + "_TMP");
    shannonSize = GetFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    RLECompressor.Compress(inFileName, outFileName + "_TMP");
    RLESize = GetFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    LZ77Compressor.Compress(inFileName, outFileName + "_TMP");
    LZ77Size = GetFileSize(outFileName + "_TMP");
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


file_header Archiver::BuildHeader(const string& fileName,
                                  CompressType compressType,
                                  ErrorCorrection errorCorrection,
                                  int compressedDataSize){
    file_header header{};
    ifstream file;

    file.open(fileName, ios_base::in);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {
        //file file_size
        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

        memset(&header, 0, sizeof(struct file_header));
        snprintf(header.signature, SIGNATURE_SZ, "%s", SIGN);
        snprintf(header.name, NAME_SZ, "%s", fileName.c_str());
        snprintf(header.version, VERSION_SZ, "%s", VERSION);
        snprintf(header.file_size, FILESIZE_SZ, "%d", fileSize);
        snprintf(header.algorithm, ALGORITHM_SZ, "%u", compressType);
        snprintf(header.errorcorr, ERRORCORR_SZ, "%u", errorCorrection);
        snprintf(header.data_size, DATASIZE_SZ, "%d", compressedDataSize);
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
        outFile.write(header.file_size, FILESIZE_SZ);
        outFile.write(header.algorithm, ALGORITHM_SZ);
        outFile.write(header.errorcorr, ERRORCORR_SZ);
        outFile.write(header.data_size, DATASIZE_SZ);
        outFile.write(header.padding, PADDING_SZ);
    }
    outFile.close();
}

void Archiver::CopyToFile(const string& from, long startPosition, int copySize, const string& to){

    ofstream outFile;
    ifstream inFile;

    outFile.open(to, ios::app | ios::binary);

    if(!outFile){
        cout << "Error opening outFile!" << endl;
    }
    else{
        inFile.open(from, ios::binary);
        inFile.seekg(startPosition, ios::beg);
        if(copySize > 0){
            //copy data of specified length
            std::copy_n(std::istreambuf_iterator<char>(inFile),
                        copySize,
                        std::ostreambuf_iterator<char>(outFile));
        }
        else{
            //copy whole data
            std::copy(
                    std::istreambuf_iterator<char>(inFile),
                    std::istreambuf_iterator<char>( ),
                    std::ostreambuf_iterator<char>(outFile));
        }


        inFile.close();
    }
    outFile.close();
}


int Archiver::GetFileSize(const string& fileName) {
    ifstream file;

    file.open(fileName, ios::in);
    if (!file) {
        cout << "Can't open file " << fileName << endl;
        return 0;
    } else {
        //file file_size
        file.seekg(0, std::ios::end);
        int fileSize = (int) (file.tellg());
        return fileSize;
    }

}

file_header Archiver::ReadHeader(const string& fileName, long position) {
    ifstream file;
    file_header header{};

    file.open(fileName);
    if (!file) {
        cout << "Can't read file " << fileName << endl;
    } else {
        file.seekg(position, ios_base::beg);


        while (!file.eof() && file.peek() != EOF) {

            //read header from archive
            memset(&header, 0, sizeof(struct file_header));
            file.read(header.signature, SIGNATURE_SZ);
            file.read(header.name, NAME_SZ);
            file.read(header.version, VERSION_SZ);
            file.read(header.file_size, FILESIZE_SZ);
            file.read(header.algorithm, ALGORITHM_SZ);
            file.read(header.errorcorr, ERRORCORR_SZ);
            file.read(header.data_size, DATASIZE_SZ);
            file.read(header.padding, PADDING_SZ);   //going through padding

        }
    }
}


bool Archiver::CheckHeader(const file_header& header){
    if (strcmp(header.signature, SIGN) != 0) {
        cout << "Error: Signature mismatch!" << endl;
        return false;
    }

    if (strcmp(header.version, VERSION) != 0) {
        cout << "Error: Incompatible version!" << endl;
        return false;
    }

    if (strcmp(header.algorithm, "0") != 0 ||
        strcmp(header.algorithm, "1") != 0 ||
        strcmp(header.algorithm, "2") != 0 ||
        strcmp(header.algorithm, "3") != 0 ||
        strcmp(header.algorithm, "4") != 0) {
        cout << "Error: Wrong algorithm code!" << endl;
        return false;
    }

    if (strcmp(header.errorcorr, "0") != 0 ||
        strcmp(header.errorcorr, "1") != 0 ||
        strcmp(header.errorcorr, "2") != 0) {
        cout << "Error: Wrong error correction algorithm code!" << endl;
        return false;
    }

    return true;
}


void ReadArchivePartToFile(const string& inFileName, const string& outFileName, long& position){

}

