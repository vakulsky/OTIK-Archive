//
// Created by Grigory on 18/10/2022.
//

#include "Shannon.h"

int Shannon::Compress(const string& file, const string& archiveName, bool writeHeader, file_header& header){

    ifstream f;
    string str;
    int compressedSize = 0;

    codes.clear();
    symbolsAmount = 0;

    f.open(file);
    if(!f) {
        cout << "Can't read file " << file << endl;
        return compressedSize;
    }
    else {
        while (getline(f, str)) {
            vector<string> syms = divideString(str);
            for(auto & sym : syms){
                analyzeUTF8(sym);
                symbolsAmount++;
            }
        };
        f.close();

        shannonCodes();

        /////
        for(auto & code : codes){
            cout << "DEBUG| (codes): " << code.first << " | " << code.second.first << " | " << code.second.second << endl;
        }
        /////

        compressedSize = writeToFile(file, archiveName, header, writeHeader);

        return compressedSize;

    }

}


void Shannon::analyzeUTF8(const string& currentSymbol){
    bool found = false;
    for(auto & symbol : codes){
        if(symbol.first == currentSymbol){
            //symbol is in the list, so increasing the amount
            symbol.second.first++;
            found = true;
        }
    }
    if(!found) {
    //symbol was not found in the list, adding it
    codes.emplace_back(make_pair(currentSymbol, make_pair(1, "")));
    }
}

void Shannon::shannonCodes() {

    //sort
    sort(codes.begin(), codes.end(), frequencyDescending());

    for (int i = 0; i < codes.size(); i++) {
        int freqSum = 0;

        //probability = frequency/symbolsAmount
        int codeLength = ceil(-log2((double)(codes[i].second.first)/symbolsAmount));

        for (int j = 0; j < i; j++)
            freqSum += codes[j].second.first;

        double probabilitySum = (double) freqSum / symbolsAmount;

        //calculating code
        double fraction = probabilitySum - (int) (probabilitySum);

        /////
        cout << "DEBUG| (shannonCodes fun (sym)): " << codes[i].first << endl;
        cout << "DEBUG| (freq sum): " << freqSum << endl;
        cout << "DEBUG| (code length): " << codeLength << endl;
        cout << "DEBUG| (fraction): " << fraction << endl;
        cout << "DEBUG| (prob sum): " << probabilitySum << endl;
        /////

        for (int k = 0; k < codeLength; k++) {
            fraction = fraction * 2;
            codes[i].second.second += to_string((int) (fraction));
            if ( fraction > 1 ) {
                fraction = fraction - 1;
            }
        }
    }

    /////
    cout << "DEBUG| (syms amount): " << symbolsAmount << endl;
    /////
}


int Shannon::writeToFile(const string& file, const string& archiveName, file_header& header, bool writeHeader) {

    ofstream archive;
    ifstream fileStream;
    string str;

    archive.open(archiveName, ios::app);
    if (!archive) {
        cout << "Can't open file " << archiveName << endl;
        return 0;
    } else {

        if(writeHeader) {
            //writing header
            archive.write(header.signature, SIGNATURE_SZ);
            archive.write(header.name, NAME_SZ);
            archive.write(header.version, VERSION_SZ);
            archive.write(header.size, SIZE_SZ);
            archive.write(header.algorithm, ALGORITHM_SZ);
            archive.write(header.padding, PADDING_SZ);
        }

        auto size = archive.tellp();

        //writing table size (symbols in table)
        archive << codes.size() << endl;

        //writing table
        for (auto code: codes) {
            archive << code.first  << code.second.second << endl;
        }

        fileStream.open(file);
        if (!fileStream) {
            cout << "Can't read file " << file << endl;
        } else {
            while (getline(fileStream, str)) {
                vector<string> syms = divideString(str);
                for (auto &sym: syms) {

                    for (auto code: codes) {
                        if (sym == code.first) {
                            archive << code.second.second;
                        }
                    }
                }


            };
            archive << "\n";
            fileStream.close();


        };
        size = archive.tellp() - size;
        archive.close();

        return size;
    }
}

void Shannon::Extract(ifstream & archiveFile, const string& fileName) {

    string symbolsInTable,
                    accum,
                    buff;
    char byte[1];

    ofstream extractedFile;
    extractedFile.open(fileName);

    getline(archiveFile, symbolsInTable);

    codes.clear();
    for(int i = 0; i < stoi(symbolsInTable); i++){
        buff.clear();
       getline(archiveFile, buff);
        parseCode(buff);
    }

    /////
    cout << "DEBUG | CODES FROM ARCHIVE" << endl;
    for(auto code : codes){
        cout << code.first << " | " << code.second.second << endl;
    }
    /////


    archiveFile.read(byte, 1);
    while (byte[0] != '\n') {
        accum += byte[0];

        for(const auto& sym : codes){
            if(accum == sym.second.second){

                /////
                cout << "DEBUG | (matched): " << accum << " --> " << sym.first << endl;
                /////

                extractedFile << sym.first;
                accum.clear();
            }
        }

        archiveFile.read(byte, 1);

    }
    extractedFile.close();

}
