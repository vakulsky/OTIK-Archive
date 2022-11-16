//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Archiver::Compress(CompressType compressType, ErrorCorrection dataProtectionType, ErrorCorrection headerProtectionType) {

    for(auto file : files){

        //use different name for temp compressed file
        string compressedDataFileName = "compressed.data",
               protectedDataFileName = "protected.data";

        //compress file using preferred algorithm
        switch (compressType) {
            case PACK:
                CopyToFile(file, 0, 0, compressedDataFileName);
                break;

            case SHANNON:
                shannonCompressor.Compress(file, compressedDataFileName);
                break;

            case RLE:
                RLECompressor.Compress(file, compressedDataFileName);
                break;

            case LZ77:
                LZ77Compressor.Compress(file, compressedDataFileName);
                break;

            case INTELLIGENT:
                compressType = IntelligentArchive(file, compressedDataFileName);

        }

        //protect data if needed
        switch (dataProtectionType) {
            case HAMMING:
                hammingCodeProtector.AddProtection(compressedDataFileName, protectedDataFileName);
                break;

            case REEDSOL:
                reedSolomonWrapper.AddProtection(compressedDataFileName, protectedDataFileName);
                break;

            case NONE:
                protectedDataFileName = compressedDataFileName;
                break;
        }

        //build header for file
        file_header header = BuildHeader(file,
                                         compressType,
                                         dataProtectionType,
                                         GetFileSize(protectedDataFileName),
                                         headerProtectionType);

        //protect header if needed
        string headerFileName = "raw.header",
               protectedHeaderFileName = "protected.header";

        //write header to temp file to protect
        WriteHeaderToFile(header, headerFileName);
        switch (headerProtectionType) {
            case REEDSOL:
                reedSolomonWrapper.AddProtection(headerFileName, protectedHeaderFileName);
                break;

            case NONE:
                //nothing to do
                break;

            case HAMMING:
                //NOT IMPLEMENTED YET
                protectedHeaderFileName = headerFileName;
                cout << "WARNING: Header protection using Hamming codes is NOT IMPLEMENTED YET. Header won't be protected!" << endl;
                break;
        }

        CopyToFile(headerFileName, 0, 0, archiveFileName);
        if(headerProtectionType != NONE){
            CopyToFile(protectedHeaderFileName, 0, 0, archiveFileName);
        }
        CopyToFile(protectedDataFileName, 0, 0, archiveFileName);

        RemoveFiles(vector<string>{
                protectedDataFileName,
                compressedDataFileName,
                headerFileName,
                protectedHeaderFileName
        });
    }


}


void Archiver::Extract(const string& inFileName){
    ifstream archiveFile;

    auto archiveSize = GetFileSize(inFileName);
    long currentPosition = 0;

    //read archive to its end
    while(currentPosition < archiveSize){
        file_header header = ReadHeader(inFileName, currentPosition);
        currentPosition+=HEADER_SZ;

        //checking header
        if(!CheckHeader(header)){
            //something is wrong with header. trying to read protected version of it
            string protectedHeaderFileName = "protected.header",
                   recoveredHeaderFileName = "recovered.header";

            /////
            cout << "DEBUG | header was wrong" << endl;
            /////

            CopyToFile(inFileName, currentPosition, protectedHeaderSize, protectedHeaderFileName);
            reedSolomonWrapper.RemoveProtection(protectedHeaderFileName, recoveredHeaderFileName);

            file_header retrievedHeader = ReadHeader(recoveredHeaderFileName, 0);

            RemoveFiles(vector<string>{
                    protectedHeaderFileName,
                    recoveredHeaderFileName
            });

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
        else{
            if(strcmp(header.header_protection_flag, "0") != 0) {
                //header ok but there is still its protected copy in archive
                currentPosition += protectedHeaderSize;
            }
        }

        //if here then header is OK

        /////
        cout << "DEBUG | header OK" << endl;
        /////

        string protectedDataFileName = "protected.data",
               recoveredDataFileName = "recovered.data";
        if(strcmp(header.errorcorr, "1") == 0){

            /////
            cout << "DEBUG | file was protected using hamming" << endl;
            /////

            //data protected using Hamming Codes
            CopyToFile(inFileName, currentPosition,
                       atoi(header.data_size),
                       protectedDataFileName);
            currentPosition += atoi(header.data_size);

            //recovering data
            hammingCodeProtector.RemoveProtection(protectedDataFileName, recoveredDataFileName);
        }
        else if(strcmp(header.errorcorr, "2") == 0){
            //data protected using Reed-Solomon Codes

            /////
            cout << "DEBUG | file was protected using reed-solomon" << endl;
            /////

            CopyToFile(inFileName, currentPosition,
                       atoi(header.data_size),
                       protectedDataFileName);
            currentPosition += atoi(header.data_size);

            reedSolomonWrapper.RemoveProtection(protectedDataFileName, recoveredDataFileName);
        }
        else{
            //data wasn't protected. Writing to destination file

            /////
            cout << "DEBUG | file wasn't protected" << endl;
            /////

            CopyToFile(inFileName, currentPosition,
                       atoi(header.data_size),
                       recoveredDataFileName);
            currentPosition += atoi(header.data_size);
        }

        if(strcmp(header.algorithm, "0") == 0) {

            /////
            cout << "DEBUG: PACK"  <<  endl;
            /////

            string originalFileName = string(header.name);
            CopyToFile(recoveredDataFileName, 0, 0, originalFileName);
        }
        else
            if(strcmp(header.algorithm, "1") == 0) {
                /////
                cout << "DEBUG: SHANNON"  <<  endl;
                /////
                shannonCompressor.Extract(recoveredDataFileName, header);
            }
            else
                if(strcmp(header.algorithm, "2") == 0) {
                    /////
                    cout << "DEBUG: RLE"  <<  endl;
                    /////
                    RLECompressor.Extract(recoveredDataFileName, header);
                }
                else
                    if(strcmp(header.algorithm, "3") == 0) {
                        /////
                        cout << "DEBUG: LZ77"  <<  endl;
                        /////
                        LZ77Compressor.Extract(recoveredDataFileName, header);
                    }


        RemoveFiles(vector<string>{
            protectedDataFileName,
            recoveredDataFileName
        });
    }
}

CompressType Archiver::IntelligentArchive(const string& inFileName, const string& outFileName){
   int packSize, shannonSize, RLESize, LZ77Size;

    packSize = GetFileSize(inFileName);

    shannonCompressor.Compress(inFileName, outFileName + "_TMP");
    shannonSize = GetFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    RLECompressor.Compress(inFileName, outFileName + "_TMP");
    RLESize = GetFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());

    LZ77Compressor.Compress(inFileName, outFileName + "_TMP");
    LZ77Size = GetFileSize(outFileName + "_TMP");
    remove((outFileName + "_TMP").c_str());


    if( shannonSize <= RLESize &&  shannonSize <= packSize &&  shannonSize <= LZ77Size){
        shannonCompressor.Compress(inFileName, outFileName);
        return SHANNON;
    }

    if( RLESize <= shannonSize &&  RLESize <= packSize &&  RLESize <= LZ77Size){
        RLECompressor.Compress(inFileName, outFileName);
        return RLE;
    }

    if( LZ77Size <= shannonSize &&  LZ77Size <= packSize &&  LZ77Size <= RLESize){
        LZ77Compressor.Compress(inFileName, outFileName);
        return LZ77;
    }

    CopyToFile(inFileName, 0, 0, outFileName);
    return PACK;

}


file_header Archiver::BuildHeader(const string& fileName,
                                  CompressType compressType,
                                  ErrorCorrection errorCorrection,
                                  int compressedDataSize,
                                  int headerProtectionFlag){
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
        snprintf(header.data_size, DATASIZE_SZ, "%d", compressedDataSize);
        snprintf(header.algorithm, ALGORITHM_SZ, "%u", compressType);
        snprintf(header.errorcorr, ERRORCORR_SZ, "%u", errorCorrection);
        snprintf(header.header_protection_flag, HEADERPROTECT_SZ, "%u", headerProtectionFlag);
    }
    return header;

}


void Archiver::WriteHeaderToFile(const file_header& header, const string& outFileName){

    ofstream outFile;

    outFile.open(outFileName, ios::app | ios::binary);

    if(!outFile){
        cout << "WriteHeaderToFile | Error opening outFile!" << endl;
    }
    else{

        //writing header
        outFile.write(header.signature, SIGNATURE_SZ);
        outFile.write(header.name, NAME_SZ);
        outFile.write(header.version, VERSION_SZ);
        outFile.write(header.file_size, FILESIZE_SZ);
        outFile.write(header.data_size, DATASIZE_SZ);
        outFile.write(header.algorithm, ALGORITHM_SZ);
        outFile.write(header.errorcorr, ERRORCORR_SZ);
        outFile.write(header.header_protection_flag, HEADERPROTECT_SZ);
        outFile.write(header.padding, PADDING_SZ);
    }
    outFile.close();
}

void Archiver::CopyToFile(const string& from, long startPosition, int copySize, const string& to){

    ofstream outFile;
    ifstream inFile;

    outFile.open(to, ios::app | ios::binary);

    if(!outFile){
        cout << "CopyToFile | Error opening outFile!" << endl;
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
        throw invalid_argument("File opening error");
    } else {
        file.seekg(position, ios_base::beg);

        //read header from archive
        memset(&header, 0, sizeof(struct file_header));
        file.read(header.signature, SIGNATURE_SZ);
        file.read(header.name, NAME_SZ);
        file.read(header.version, VERSION_SZ);
        file.read(header.file_size, FILESIZE_SZ);
        file.read(header.data_size, DATASIZE_SZ);
        file.read(header.algorithm, ALGORITHM_SZ);
        file.read(header.errorcorr, ERRORCORR_SZ);
        file.read(header.header_protection_flag, HEADERPROTECT_SZ);
        file.read(header.padding, PADDING_SZ);   //going through padding

        /////
        cout << "DEBUG | (sign from file): " << string(header.signature) << endl;
        cout << "DEBUG | (name from file): " << string(header.name) << endl;
        cout << "DEBUG | (version from file): " << string(header.version) << endl;
        cout << "DEBUG | (file_size from file): " << string(header.file_size) << endl;
        cout << "DEBUG | (data_size from file): " << string(header.data_size) << endl;
        cout << "DEBUG | (alg code from file): " << string(header.algorithm) << endl;
        cout << "DEBUG | (error corr code from file): " << string(header.errorcorr) << endl;
        cout << "DEBUG | (header protect flag from file): " << string(header.header_protection_flag) << endl;
        /////
    }

    return header;
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


    if (strcmp(header.algorithm, "0") != 0 &&
        strcmp(header.algorithm, "1") != 0 &&
        strcmp(header.algorithm, "2") != 0 &&
        strcmp(header.algorithm, "3") != 0) {
        cout << "Error: Wrong algorithm code!" << endl;
        return false;
    }

    if (strcmp(header.errorcorr, "0") != 0 &&
        strcmp(header.errorcorr, "1") != 0 &&
        strcmp(header.errorcorr, "2") != 0) {
        cout << "Error: Wrong error correction algorithm code!" << endl;
        return false;
    }

    return true;
}


int Archiver::RemoveFiles(const vector<string>& files){
    int count = 0;
    for(auto file : files){
        try{
            remove(file.c_str());
        }
        catch(...){

        }
        count++;
    }

    return count;
}

