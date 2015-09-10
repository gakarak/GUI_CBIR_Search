#ifndef DSCBFPCA_H
#define DSCBFPCA_H

#include "dscplugininterface.h"
#include "dscpluginbfpca_global.h"
#include "lib_fpcan_dsc.h"

#include <QObject>
#include <iostream>
#include <sstream>

#define     DEF_IMAGE_BASESIZE  128
#define     DEF_NUMSAMPLES      20000
#define     DEF_NUMSMP_PER_IMG  100
#define     DEF_FLT_SIZE        15
#define     DEF_NUM_BASIS       20
#define     DEF_NUM_SIGMS       5

static const char* default_dscTypesCodes[] = {
    // gray:
    "g0r_2.8",
    "g0r_2.4-3.4",
    // color:
    "c0r_2.8",
    "c0r_2.6-3.6",
    "c1r_2.4-3.4",
};

static const int    default_numDscTypes  = sizeof(default_dscTypesCodes)/sizeof(default_dscTypesCodes[0]);
const QString       default_codeType        = "bfpca";

class DSCPLUGIN_BFPCA_LIBRARY DscPluginBFPCA : public QObject, DscPluginInterface
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "by.basnet.uiip.lab225.DscPluginInterface" FILE "simpletestplugin.json")
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "by.basnet.uiip.lab225.DscPluginInterface")
#endif
    Q_INTERFACES(DscPluginInterface)
public:
    DscPluginBFPCA() {}
    DscPluginBFPCA(const std::string& strCode);
//    std::string toString() const;
    void    buildDscFromPath(const std::string &imgPath);
    void    buildDscFromMat (const cv::Mat &imgin);
    double  calcDscDistance(const DscPluginInterface *d) const;
    void    saveToStream(std::ofstream &ofs);
    void    loadFromStream(std::ifstream &ifs);
    void    clear();
    //
    void                init(const QString& pathToLoadedLibrary="");
    bool                prepareDscData(const QString &path, const std::string &code);
    DscPluginInterface* createObject(const std::string& dscCode="") const;
    QString             getCodeType() const;
    QStringList         getCodeSubTypes() const;
    //
    QString getSuffix() const;
    QString getShortInfo() const;

    FPCANDescriptor dsc;

    void setStrCode(const std::string& strCode);

public:
    bool                isColor;// Config for base properties: color, rot
    bool                isRot4;
    int                 dscType;
private:
    void initDefault();
//    void setStrCode(const std::string& strCode);
    QString suffix;
    static QString      codeType;
    static QStringList  codeSubTypes;
    //
    static ConfigFPCA   cfgFPCA;
    static FilterBankN  bank;
    //
    ConfigDscFPCA       cfgDscFPCA; // Config for setup compound descriptor

    void initConfigByDefault();
    void readConfigFPCA(const QString& pathCfg);
    QString getFilterBankPrefix() const;

//    int  dscGridSize;
//    int  dscNBin;
//    bool isColor;
//    bool isMultiDst;
//    int  dscDst;
//    bool isRotateInvariant;
};

#if QT_VERSION < 0x050000
Q_DECLARE_INTERFACE(DscPluginBFPCA, "by.basnet.uiip.lab225.DscPluginBFPCA/1.0")
#endif

#endif // DSCBFPCA_H
