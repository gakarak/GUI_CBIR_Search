#ifndef DSC_MEANCH_H
#define DSC_MEANCH_H

#include "dscplugininterface.h"
#include "dscplugin_meanch_global.h"

#include <QObject>
#include <iostream>
#include <sstream>
#include <vector>

#define     DEF_IMAGE_BASESIZE  128

static const char* default_dscCooTypesCodes[] = {
    "color",
    "gray"
};

static const int    default_numDscCooTypes  = sizeof(default_dscCooTypesCodes)/sizeof(default_dscCooTypesCodes[0]);
const QString       default_codeType        = "meanch";

class DSCPLUGIN_MEANCH_EXPORT DscPluginMeanCh : public QObject, DscPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "by.basnet.uiip.lab225.DscPluginInterface")
#endif
    Q_INTERFACES(DscPluginInterface)
public:
    DscPluginMeanCh() {}
    DscPluginMeanCh(const std::string& strCode);

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

    std::vector<double> dsc;
private:
    void initDefault();
    void setStrCode(const std::string& strCode);
    QString suffix;
    static QString      codeType;
    static QStringList  codeSubTypes;
    //
    bool isColor;
};

#if QT_VERSION < 0x050000
Q_DECLARE_INTERFACE(DscPluginMeanCh, "by.basnet.uiip.lab225.DscPluginMeanCh/1.0")
#endif

#endif
