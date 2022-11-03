//
// Created by grigory on 03/11/22.
//

#ifndef OTIK_ARCHIVE_LZ77COMPRESS_H
#define OTIK_ARCHIVE_LZ77COMPRESS_H

#include "RLECompress.h"

class LZ77Compress {

private:

    /** BUFFERS **/
    std::string TEXT_IN; // contains the text from an archive
    std::string TEXT_OUT; // contains the text encoded
    std::string strToAnalyze; // contains the string to analyze
    std::string window; // contains the string where will search matches
    /** END BUFFERS **/

    unsigned int maxChunkLength = 9;
    unsigned int windowSize = 9;
    unsigned int position;
    unsigned int windowStartPos;

    std::pair< unsigned int, unsigned int> SL = {0, 0};

    file_header buildHeader(const string& fileName);
    void encodeLZ77();

    void updateWindow();

public:

    int Compress(const string& fileName, const string& archiveName, bool writeHeader);
    void Extract(ifstream& archiveFile, file_header& header);



};


#endif //OTIK_ARCHIVE_LZ77COMPRESS_H
