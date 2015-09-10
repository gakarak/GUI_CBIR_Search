#ifndef DESCRIPTORCALCULATORPHASH_H
#define DESCRIPTORCALCULATORPHASH_H

#include <iostream>
#include <QStringList>
#include <pHash.h>
#include <cv.h>
#include <highgui.h>


class DescriptorCalculatorPHASH
{
public:
    DescriptorCalculatorPHASH();

    void setData(QStringList *fileNames, ulong64 **data, int dscSize, int dscNum);
    void calcDscPHASH(int pos);
    void clean();
public:
    QStringList* listNamesPtr;
    ulong64**    dataPtr;
    int          dscSize;
    int          dscNum;
};

#endif // DESCRIPTORCALCULATORPHASH_H
