//
// Created by Grigory on 18/10/2022.
//

#include "Shannon.h"

int Shannon::CompressShannon(){

    //vector of {symbol, {frequency, code}}
    vector<pair<string, pair<int, string>>> symbols;
    ifstream f;
    string str;
    int symbolsAmount=0;

    f.open(archive_file);
    if(!f)
        cout << "Can't read file " << archive_file << endl;
    else {
        while (getline(f, str)) {
            vector<string> syms = divideString(str);
            for(int i = 0; i < syms.size(); i++){
                analyzeUTF8(symbols, syms[i]);
                symbolsAmount += 1;
            }
        };
        f.close();

        shannonCodes(symbols, symbolsAmount);

    }

}


void CompressWrapper(){

}
void ExtractWrapper(){

}

void ExtractShannon(const string& archive_file){

//todo somehow read codes


}


void Shannon::analyzeUTF8(vector<pair<string, pair<int, string>>>& symbols, const string& currentSymbol){
bool found = false;
for(auto symbol : symbols){
if(symbol.first == currentSymbol){
//symbol is in the list, so increasing the amount
symbol.second.first++;
found = true;
}
}
if(!found) {
//symbol was not found in the list, adding it
symbols.emplace_back(make_pair(currentSymbol, make_pair(1, "")));
}
}

void Shannon::shannonCodes(vector<pair<std::string, pair<int, std::string>>> &symbols, int symbolsAmount) {
for(int i = 0; i < symbols.size(); i++){
int freqSum = 0;

int codeLength = ceil(-log2(symbols[i].second.first));

for(int j = 0; j < i; j++){
freqSum += symbols[i].second.first;
}
double probabilitySum = (double)freqSum/symbolsAmount;

//calculating code
double fraction = probabilitySum - (int)(probabilitySum / symbolsAmount);
for(int k = 0; k < codeLength; k++){
symbols[i].second.second += to_string((int)(fraction*2));
if(fraction*2 > 1){
fraction = fraction - 1;
}
}
}
}

void Shannon::writeShannon(const string& file, vector<pair<std::string, pair<int, std::string>>> &codes){
char byte[1];  // buffer for reading 1 byte

file_header header;
ifstream f;
ofstream main;
string str;
main.open(archive_file, ios_base::binary);  // opening archive

string codeTable = buildShannonTable(codes);


//writing file and header to archive
header = buildHeader(file, sizeof codeTable);

//todo write table
//todo write codes


f.open(file, ios_base::in);
if(!f)
cout << "Can't read file " << file << endl;
else {
while (getline(f, str)) {
vector<string> syms = divideString(str);
for (int i = 0; i < syms.size(); i++) {
analyzeUTF8(symbols, syms[i]);
symbolsAmount += 1;
}
};
f.close();
}

//write header
fwrite( reinterpret_cast<const unsigned char *>(&header), 1, sizeof( struct file_header ), main );

//write file data
while (!feof(f)) {
if (fread(byte, 1, 1, f) == 1) fwrite(byte, 1, 1, main);
}
cout << *itr << " added to archive '" << this->archive_file << "'." << endl;
fclose(f);

fclose(main);
}

string Shannon::buildShannonTable(const vector<pair<string, pair<int, string>>>& codes){
string table;

for(auto code : codes){
table += code.first + " " + code.second.second + "\n";
}
}
