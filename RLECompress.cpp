//
// Created by Grigory on 31/10/2022.
//

#include "RLECompress.h"

int RLECompress::Compress(const string& fileName, const string& archiveName, bool writeHeader) {

    ifstream file;
    ofstream archive;
    char buff[1];
    char previous[1];
    char repeatedSym[1];
    string diffAccum;
    int count = 0, minRep = 3;
    bool firstIteration = true;


    file.open(fileName, ios::in);
    file.seekg(0, ios::beg);
    archive.open(archiveName, ios::app);
    if (!file) {
        cout << "Can't read file " << fileName << endl;
        return 0;
    } else if (!archive) {
        cout << "Can't open archive file " << fileName << endl;
        return 0;
    } else {

        if(writeHeader) {
            file_header header = buildHeader(fileName);

            //writing header
            archive.write(header.signature, SIGNATURE_SZ);
            archive.write(header.name, NAME_SZ);
            archive.write(header.version, VERSION_SZ);
            archive.write(header.size, SIZE_SZ);
            archive.write(header.algorithm, ALGORITHM_SZ);
            archive.write(header.padding, PADDING_SZ);
        }

        auto size = archive.tellp();


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

        size = archive.tellp() - size;


        file.close();
        archive.close();
        return size;


    }
}


void RLECompress::Extract(ifstream& archiveFile, file_header& header){

    ofstream file;
    char buff[1];
    string fileName = header.name;
    char sequenceAccum[Lmax+2];
    int count;



    file.open(fileName, ios::out);
    if (!file) {
        cout << "Can't read file " << fileName << endl;

    } else if (!archiveFile) {
        cout << "Can't open archive file" << endl;

    } else {

        archiveFile.read(buff, 1);

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
                archiveFile.read(buff, 1);
                count = atoi(&buff[0]) + a;

                /////
                cout << "DEBUG| EXTR (size): " << count << endl;
                /////

                //read byte
                archiveFile.read(buff, 1);

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
                archiveFile.read(buff, 1);
                count = atoi(&buff[0]) + b;

                /////
                cout << "DEBUG| EXTR (size): " << count << endl;
                /////



                //read sequence
                memset(sequenceAccum, 0, sizeof sequenceAccum);
                archiveFile.read(sequenceAccum, count);

                /////
                cout << "DEBUG| EXTR (seq): " << sequenceAccum << endl;
                /////

                file.write(sequenceAccum, count);

            }

            archiveFile.read(buff, 1);
        }

    }


        file.close();





}




file_header RLECompress::buildHeader(const std::string &fileName) {
    file_header header{};
    ifstream file;

    file.open(fileName, ios_base::in);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {

        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

        memset(&header, 0, sizeof(struct file_header));
        snprintf(header.signature, SIGNATURE_SZ, "%s", SIGN);
        snprintf(header.name, NAME_SZ, "%s", fileName.c_str());
        snprintf(header.version, VERSION_SZ, "%s", VERSION);
        snprintf(header.size, SIZE_SZ, "%d", fileSize);
        snprintf(header.algorithm, ALGORITHM_SZ, "%s", "2");
    }
    return header;
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
