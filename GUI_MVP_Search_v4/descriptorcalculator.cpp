#include "descriptorcalculator.h"

//#include "dscplugincoomatrix.h"

DescriptorCalculator::DescriptorCalculator()
{
    clean();
}

void DescriptorCalculator::setData(const QString& path, QStringList *fileNames, DscPluginInterface** listDscPtr, DscPluginInterface* dscBuilder, const QString& dscStrCode)
{
    this->path          = path;
    this->listNamesPtr  = fileNames;
    this->listDscPtr    = listDscPtr;
    this->dscBuilder    = dscBuilder;
    this->strCode       = dscStrCode.toStdString();
    this->dscNum        = fileNames->size();
}

void DescriptorCalculator::calcDsc(int pos)
{
    if(pos<dscNum) {
        //FIXME: in future add Buil-Factory for support multiple-descriptor types
        DscPluginInterface* tmpDsc    = dscBuilder->createObject(strCode);
        tmpDsc->buildDscFromPath(listNamesPtr->at(pos).toStdString());
        listDscPtr[pos] = tmpDsc;
    } else {
        std::cerr << "!!!!WARNING : position greater than #descriptors! [DescriptorCalculatorPHASH::calcDscPHASH]" << std::endl;
    }
}

void DescriptorCalculator::prepareData()
{
    DscPluginInterface* tmpDsc    = dscBuilder->createObject(strCode);
    tmpDsc->prepareDscData(path, strCode);
}

void DescriptorCalculator::clean()
{
    listNamesPtr    = NULL;
    listDscPtr      = NULL;
    strCode         = "";
    dscNum          = -1;
}
