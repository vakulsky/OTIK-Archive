//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"



int main(int argv, char* argc[])
{
    /*/  Supported args:
    //
    //    -pack, -unpack, -files, -path
    //
    /*/

    setlocale(LC_ALL,"Russian");
    cout<<endl<<"######################## ZIPPER ########################"<<endl<<endl;
    if(argv>1)
    {
        vector<string> files;  // массив файлов, переданных через параметры из консоли
        string path = "";  // путь
        bool flag_fs = false, flag_path = false;  // флаги режима чтения/записи
        char type[7];              // тип: упаковка или распаковка
        memset(type,0,7);

        for(int i=1;i<argv;i++)
        {
            if(strcmp(argc[i],"-pack")==0) { strcpy(type,"pack"); flag_fs=flag_path=false;}
            if(strcmp(argc[i],"-unpack")==0) { strcpy(type,"unpack"); flag_fs=flag_path=false;}
            if(strcmp(argc[i],"-path")==0) {flag_path=true; flag_fs=false; continue; }
            if(strcmp(argc[i],"-files")==0) {flag_fs=true; flag_path=false; continue; }

            if(flag_path) {path.assign(argc[i]); }
            if(flag_fs) files.push_back(string(argc[i]));

        }
        Zipper *zip = new Zipper(files,path);
        if(strcmp(type,"pack")==0) zip->InCompress();
        if(strcmp(type,"unpack")==0) zip->OutCompress(files[0]);
    }
    else cout<<"Параметры -pack/-unpack , -files, -path обязательны!"<<endl;
    cout<<endl<<"########################################################"<<endl<<endl;

}

//todo English
//todo fix -path
//todo separate headers
