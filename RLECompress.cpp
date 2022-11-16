//
// Created by Grigory on 31/10/2022.
//

#include "RLECompress.h"

void RLECompress::Compress(const string& inFileName, const string& outFileName) {

    ifstream file;
    ofstream archive;
    char buff[1];
    char previous[1];
    char repeatedSym[1];
    string diffAccum;
    int count = 0, minRep = 3;
    bool firstIteration = true;


    file.open(inFileName, ios::in);
    file.seekg(0, ios::beg);
    archive.open(outFileName, ios::app);
    if (!file) {
        cout << "Can't read file " << inFileName << endl;

    } else if (!archive) {
        cout << "Can't open archive file " << inFileName << endl;

    } else {

        while (file.peek() != EOF) {
            file.read(buff, 1);


            if (previous[0] == buff[0]) {
                count++;
                repeatedSym[0] = buff[0];

                //remove repeated sym
                if(!diffAccum.empty()) {
                    if (diffAccum.find(repeatedSym[0]) == diffAccum.length() - 1)
                        diffAccum.pop_back();
                }

                if (count >= minRep) {
                    //write diff if any
                    if (!diffAccum.empty()) {
                        writeToArchive(archive, diffAccum);
                        diffAccum.clear();
                    }
                }

            } else {

                if (count >= minRep) {
                    writeToArchive(archive, repeatedSym[0], count);
                    count = 1;
                }

                if ((diffAccum.size() + count) > Lmax) {
                    //if we cant add more symbols to diffAccum
                    writeToArchive(archive, diffAccum);
                    diffAccum.clear();
                }

                if (count > 1) {
                    for (int j = 0; j < count; j++)
                        diffAccum.append(1, repeatedSym[0]);
                    count = 1;
                }

                diffAccum.append(1, buff[0]);

            }
            previous[0] = buff[0];
            firstIteration = false;
        }

        //write remaining symbols
        if(count > minRep){
            writeToArchive(archive, repeatedSym[0], count);
        }
        else{
            if(!diffAccum.empty())
                writeToArchive(archive, diffAccum);
        }

        //finally write '\n'
        archive << "\n";

        file.close();
        archive.close();
    }
}


void RLECompress::Extract(const string& inFileName, file_header& header){

    ifstream inFile;
    ofstream file;
    char buff[1];
    string fileName = header.name;
    char sequenceAccum[Lmax+2];
    int count;

    inFile.open(inFileName);
    file.open(fileName, ios::out);
    if (!file) {
        cout << "Can't read file " << fileName << endl;

    } else if (!inFile) {
        cout << "Can't open archive file" << endl;

    } else {

        inFile.read(buff, 1);

        /////
        cout << "DEBUG| EXTR (buff before while): " << buff[0] << endl;
        /////

        while (buff[0] != '\n') {
            if(buff[0] == '1'){
                //compressed

                /////
                cout << "DEBUG| EXTR (code): " << buff[0] << endl;
                /////

                //read count
                inFile.read(buff, 1);
                count = atoi(&buff[0]) + a;

                /////
                cout << "DEBUG| EXTR (file_size): " << count << endl;
                /////

                //read byte
                inFile.read(buff, 1);

                /////
                cout << "DEBUG| EXTR (sym): " << buff[0] << endl;
                /////

                for(int i = 0; i < count; i++){
                    file.write(&buff[0], sizeof buff[0]);
                }
            }
            else{
                //raw

                /////
                cout << "DEBUG| EXTR (code): " << buff[0] << endl;
                /////

                //read length
                inFile.read(buff, 1);
                count = atoi(&buff[0]) + b;

                /////
                cout << "DEBUG| EXTR (file_size): " << count << endl;
                /////



                //read sequence
                memset(sequenceAccum, 0, sizeof sequenceAccum);
                inFile.read(sequenceAccum, count);

                /////
                cout << "DEBUG| EXTR (seq): " << sequenceAccum << endl;
                /////

                file.write(sequenceAccum, count);

            }

            inFile.read(buff, 1);
        }

    }

        inFile.close();
        file.close();





}


void RLECompress::writeToArchive( ofstream& archive, const string& sequenceString){

    /////
    cout << "DEBUG| SEQ: " << sequenceString << endl;
    /////

    archive << 0 << sequenceString.size() - b;
    archive.write(sequenceString.c_str(), sequenceString.size());
}

void RLECompress::writeToArchive( ofstream& archive, const char repeatedChar, int count){

    /////
    cout << "DEBUG| REP: " << repeatedChar << " , count = " << count << endl;
    /////

    archive << 1 << count-a;
    archive.write(&repeatedChar, sizeof repeatedChar);

}
