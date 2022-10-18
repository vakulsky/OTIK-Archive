//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"



int main(int argv, char* argc[])
{

    cout  << argc[0] << endl << endl;

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
            if(flag_fs) files.push_back(string(argc[i]));
        }

        Archiver zip(files, path);
        if(strcmp(type,"normal")==0) zip.Compress(PACK);
        if(strcmp(type,"shannon")==0) zip.Compress(SHANNON);
        if(strcmp(type,"intelligent")==0) zip.Compress(INTELLIGENT);
        if(strcmp(type,"unpack")==0) zip.Extract(files[0]);
    }
    else {
        cout << "USAGE:" << endl;
        cout << "-pack <normal | shannon | intelligent> -files <files to compress> "
                "-path <folder_to_save_archive_file>" << endl;
        cout << "OR" << endl;
        cout << "-unpack -files <archive_file> -path <path for extracted files>" << endl;
    }
    cout<<endl<<"----------------------------------------------------------"<<endl<<endl;

}
