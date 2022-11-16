//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"



int main(int argv, char* argc[])
{

    cout<<endl<<"------------------------ ARCHIVER ------------------------"<<endl<<endl;
    if(argv>1)
    {
        vector<string> files;  //files from console
        string path = "";  // path
        bool filesFlag = false,
             pathFlag = false,
             dataProtectionFlag = false,
             headerProtectionFlag = false,
             unpackFlag = false,
             compressTypeFlag = false;  // flags

        ErrorCorrection dataProtectionType = NONE,
                        headerProtectionType = NONE;
        CompressType compressType;

        for(int i=1;i<argv;i++)
        { //filesFlag=pathFlag=dataProtectionFlag=headerProtectionFlag=unpackFlag=compressTypeFlag
            if(strcmp(argc[i],"-pack")==0) { compressTypeFlag=true; filesFlag=pathFlag=dataProtectionFlag=unpackFlag=false; continue;}
            if(strcmp(argc[i],"-protectData")==0) { dataProtectionFlag=true; filesFlag=pathFlag=unpackFlag=compressTypeFlag=false; continue;}
            if(strcmp(argc[i],"-protectHeader")==0) { headerProtectionFlag=true; continue;}
            if(strcmp(argc[i],"-unpack")==0) { unpackFlag = true; continue;}
            if(strcmp(argc[i],"-path")==0) { pathFlag=true; filesFlag=dataProtectionFlag=unpackFlag=compressTypeFlag=false; continue; }
            if(strcmp(argc[i],"-files")==0) { filesFlag=true; pathFlag=dataProtectionFlag=unpackFlag=compressTypeFlag=false; continue; }

            if(pathFlag) {path.assign(argc[i]); }
            if(filesFlag) files.emplace_back(argc[i]);
            if(compressTypeFlag){
                if(strcmp(argc[i],"norman")==0) compressType = PACK;
                if(strcmp(argc[i],"shannon")==0) compressType = SHANNON;
                if(strcmp(argc[i],"intelligent")==0) compressType = INTELLIGENT;
                if(strcmp(argc[i],"rle")==0) compressType = RLE;
                if(strcmp(argc[i],"lz77")==0) compressType = LZ77;
            }

            if(dataProtectionFlag){
                if(strcmp(argc[i],"hamming")==0) dataProtectionType = HAMMING;
                if(strcmp(argc[i],"shannon")==0) dataProtectionType = REEDSOL;
            }

            if(headerProtectionFlag){
               headerProtectionType = REEDSOL;
            }
        }


        Archiver zip(files, path);
        if(unpackFlag){
            zip.Extract(files[0]);
        }
        else{
            zip.Compress(compressType, dataProtectionType, headerProtectionType);
        }
    }
    else {
        cout << "USAGE:" << endl;
        cout << "-pack <normal | shannon | rle | lz77 | intelligent> -protectData <hamming | reedsol> [-protectHeader] -files <files to compress> "
                "-path <folder_to_save_archive_file>" << endl;
        cout << "OR" << endl;
        cout << "-unpack -files <archiveFileName> -path <path for extracted files>" << endl;
    }
    cout<<endl<<"----------------------------------------------------------"<<endl<<endl;


}
