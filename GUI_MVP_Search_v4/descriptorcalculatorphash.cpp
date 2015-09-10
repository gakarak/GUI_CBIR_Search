#include "descriptorcalculatorphash.h"

DescriptorCalculatorPHASH::DescriptorCalculatorPHASH()
{
    clean();
}

void DescriptorCalculatorPHASH::setData(QStringList *fileNames, ulong64 **data, int dscSize, int dscNum)
{
    this->listNamesPtr  = fileNames;
    this->dataPtr       = data;
    this->dscSize       = dscSize;
    this->dscNum        = dscNum;
    if(dscNum!=fileNames->size()) {
        std::cerr << "!!!!WARNING : #descriptors != #fileNames [DescriptorCalculatorPHASH::setData]" << std::endl;
    }
}


void DescriptorCalculatorPHASH::calcDscPHASH(int pos)
{
    if(pos<dscNum) {
        std::string tmpFileName = listNamesPtr->at(pos).toStdString();
        cv::Mat img = cv::imread(tmpFileName, CV_LOAD_IMAGE_GRAYSCALE);
        ulong64* dscData  = new ulong64[dscSize];
        if(img.data) {
            ph_dct_imagehash(tmpFileName.c_str(), dscData[0]);
        } else {
            std::fill(dscData, dscData+dscSize, 0);
            std::cerr << "***WARNING*** bad-image (" << tmpFileName << "), use ZERO-descriptor" << std::endl;
        }
        dataPtr[pos]    = dscData;
    } else {
        std::cerr << "!!!!WARNING : position greater than #descriptors! [DescriptorCalculatorPHASH::calcDscPHASH]" << std::endl;
    }
}

void DescriptorCalculatorPHASH::clean()
{
    dscSize         = -1;
    dscNum          = -1;
    listNamesPtr    = NULL;
    dataPtr         = NULL;
}
