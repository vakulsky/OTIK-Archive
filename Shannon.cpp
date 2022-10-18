//
// Created by Grigory on 18/10/2022.
//

#include "Shannon.h"

int Shannon::Compress(const string& file, const string& archiveName){

    ifstream f;
    string str;

    codes.clear();
    symbolsAmount = 0;

    f.open(file);
    if(!f)
        cout << "Can't read file " << file << endl;
    else {
        while (getline(f, str)) {
            vector<string> syms = divideString(str);
            for(auto & sym : syms){
                analyzeUTF8(sym);
                symbolsAmount += 1;
            }
        };
        f.close();

        shannonCodes();
        writeToFile();

    }

}


void Shannon::analyzeUTF8(const string& currentSymbol){
    bool found = false;
    for(auto symbol : codes){
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
            freqSum += codes[i].second.first;

        double probabilitySum = (double) freqSum / symbolsAmount;

        //calculating code
        double fraction = probabilitySum - (int) (probabilitySum / symbolsAmount);
        for (int k = 0; k < codeLength; k++) {
            codes[i].second.second += to_string((int) (fraction * 2));
            if ( fraction * 2 > 1 ) {
                fraction = fraction - 1;
            }
        }
    }
}

void Shannon::writeToFile(){
//todo writeToFile()
}

void Shannon::Extract(FILE *archiveFile, file_header &header) {

//todo somehow read codes

}
