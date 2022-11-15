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
            if(strcmp(argc[i],"-pack")==0) { compressTypeFlag=true; filesFlag=pathFlag=dataProtectionFlag=headerProtectionFlag=unpackFlag=false; continue;}
            if(strcmp(argc[i],"-protectData")==0) { dataProtectionFlag=true; filesFlag=pathFlag=headerProtectionFlag=unpackFlag=compressTypeFlag=false; continue;}
            if(strcmp(argc[i],"-protectHeader")==0) { headerProtectionFlag=true; filesFlag=pathFlag=dataProtectionFlag=unpackFlag=compressTypeFlag=false; continue;}
            if(strcmp(argc[i],"-unpack")==0) { unpackFlag = true; continue;}
            if(strcmp(argc[i],"-path")==0) { pathFlag=true; filesFlag=dataProtectionFlag=headerProtectionFlag=unpackFlag=compressTypeFlag=false; continue; }
            if(strcmp(argc[i],"-files")==0) { filesFlag=true; pathFlag=dataProtectionFlag=headerProtectionFlag=unpackFlag=compressTypeFlag=false; continue; }

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
                if(strcmp(argc[i],"hamming")==0) headerProtectionType = HAMMING;
                if(strcmp(argc[i],"shannon")==0) headerProtectionType = REEDSOL;
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
        cout << "-pack <normal | shannon | rle | lz77 | intelligent> -protectData <hamming | reedsol> -protectHeader <hamming | reedsol> -files <files to compress> "
                "-path <folder_to_save_archive_file>" << endl;
        cout << "OR" << endl;
        cout << "-unpack -files <archiveFileName> -path <path for extracted files>" << endl;
    }
    cout<<endl<<"----------------------------------------------------------"<<endl<<endl;




//    LZ77Compress cc;
//    char c = cc.toByte(6, 4);
//
//    auto res = cc.toInt(c);
//
//    cout << "DEBUG: int -> byte -> int" << endl;
//    for(auto n : res){
//        cout << n << endl;
//    };


//    unsigned int s=5, l=12;  //12 = 1100, 5 = 101  =>   10101100
//    int win=5, len=15;
//    cout << "(s << win): " << (s << win) << endl;
//    cout << "(l & len): " << (l & len) << endl << endl;
//    cout << "(s << win) | (l & len): " << ((s << win) | (l & len)) << endl;




}
