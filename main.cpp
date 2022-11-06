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
        bool flag_fs = false, flag_path = false;  // flags
        char type[15];              // flag container
        memset(type,0,7);

        for(int i=1;i<argv;i++)
        {
            if(strcmp(argc[i],"-pack")==0) { strcpy(type,argc[i+1]); flag_fs=flag_path=false;}
            //todo choose between normal pack, Shannon and intelligent
            if(strcmp(argc[i],"-unpack")==0) { strcpy(type,"unpack"); flag_fs=flag_path=false;}
            if(strcmp(argc[i],"-path")==0) {flag_path=true; flag_fs=false; continue; }
            if(strcmp(argc[i],"-files")==0) {flag_fs=true; flag_path=false; continue; }

            if(flag_path) {path.assign(argc[i]); }
            if(flag_fs) files.emplace_back(argc[i]);
        }

        Archiver zip(files, path);
        if(strcmp(type,"normal")==0) zip.Compress(PACK);
        if(strcmp(type,"shannon")==0) zip.Compress(SHANNON);
        if(strcmp(type,"intelligent")==0) zip.Compress(INTELLIGENT);
        if(strcmp(type,"rle")==0) zip.Compress(RLE);
        if(strcmp(type,"lz77")==0) zip.Compress(LZ77);

        if(strcmp(type,"testall")==0) zip.Compress(TESTALL);
        if(strcmp(type,"unpack")==0) zip.Extract(files[0]);
    }
    else {
        cout << "USAGE:" << endl;
        cout << "-pack <normal | shannon | rle | lz77 | intelligent | testall> -files <files to compress> "
                "-path <folder_to_save_archive_file>" << endl;
        cout << "OR" << endl;
        cout << "-unpack -files <archive_file> -path <path for extracted files>" << endl;
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
