#ifndef DESCRIPTORCALCULATOR_H
#define DESCRIPTORCALCULATOR_H

#include <iostream>
#include <QStringList>

#include "dscplugininterface.h"

#include <cv.h>
#include <highgui.h>


class DescriptorCalculator
{
public:
    DescriptorCalculator();

    void setData(const QString& path, QStringList *fileNames, DscPluginInterface** listDscPtr, DscPluginInterface* dscBuilder, const QString& dscStrCode);
    void calcDsc(int pos);
    void prepareData();
//    void prepareData(const QString& path, const std::string& code);
    void clean();
public:
    QString                 path;
    QStringList*            listNamesPtr;
    DscPluginInterface**    listDscPtr;
    DscPluginInterface*     dscBuilder;
    std::string     strCode;
    int             dscNum;
};

#endif // DESCRIPTORCALCULATOR_H
