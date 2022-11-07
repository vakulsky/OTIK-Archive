//
// Created by grigory on 07/11/22.
//

#include <cstring>
#include "tools.h"


void addAlgorithm(std::vector<CompressType>& v, const char* arg){
    if(strcmp(arg,"normal")==0) v.emplace_back(PACK);
    if(strcmp(arg,"shannon")==0) v.emplace_back(SHANNON);
    if(strcmp(arg,"intelligent")==0) v.emplace_back(INTELLIGENT);
    if(strcmp(arg,"rle")==0) v.emplace_back(RLE);
    if(strcmp(arg,"lz77")==0) v.emplace_back(LZ77);
}
