#include "dscpluginbfpca.h"

#include <QFileInfo>

/*static */QString      DscPluginBFPCA::codeType/*        = ""*/;
/*static */QStringList  DscPluginBFPCA::codeSubTypes/*    = QStringList()*/;
/*static */ConfigFPCA   DscPluginBFPCA::cfgFPCA/*    = QStringList()*/;
/*static */FilterBankN  DscPluginBFPCA::bank;

DscPluginBFPCA::DscPluginBFPCA(const std::string& strCode) {
    setStrCode(strCode);
}

// str =   |c,g  |0,1 |_|Si1.Nf1|-|Si2.Nf2|-|Si3.Nf3|
//  ex#1   = c0r_1.3-2.4
void DscPluginBFPCA::setStrCode(const std::string &strCode)
{
    bool isOk   = false;
    if(strCode.size()>6) {
        QStringList strCodeSpl0 = QString(strCode.c_str()).split('_');
        if(strCodeSpl0.size()>1) {
            dscType = 1;
            if(strCode[0]=='c') {
                this->isColor   = true;
            } else {
                this->isColor   = false;
            }
            if(strCode[1]=='1') {
                isRot4   = true;
            } else {
                isRot4   = false;
            }
            //
            QStringList strCodeSpl1 = strCodeSpl0[1].split('-');
            QStringList tmp;
            cfgDscFPCA.clear();
            for(int ii=0; ii<strCodeSpl1.size(); ii++) {
                tmp = strCodeSpl1[ii].split('.');
                cfgDscFPCA.listSigm.push_back(tmp[0].toInt());
                cfgDscFPCA.listNumFlt.push_back(tmp[1].toInt());
            }
//            std::cout << cfgDscFPCA << std::endl;
            isOk = true;
        }
    }
    if(!isOk){
        dscType = -1;
        std::cerr << "*** ERROR-DSC *** bad-dsc string-code [" << strCode << "]" << std::endl;
    }
}

void DscPluginBFPCA::initConfigByDefault()
{
    cfgFPCA.numSamples          = DEF_NUMSAMPLES;
    cfgFPCA.numSamplesPerImage  = DEF_NUMSMP_PER_IMG;
    cfgFPCA.sizeFlt             = DEF_FLT_SIZE;
    cfgFPCA.numBasis            = DEF_NUM_BASIS;
    cfgFPCA.numSigmas           = DEF_NUM_SIGMS;
    std::cout << "cfgFPCA-Default: " << cfgFPCA << std::endl;
}

void DscPluginBFPCA::readConfigFPCA(const QString &pathCfg)
{
    QFileInfo infoCfg(pathCfg);
    if(infoCfg.exists()) {
        QSettings sett(infoCfg.absoluteFilePath(), QSettings::IniFormat);
        sett.beginGroup(DEF_CFG_GROUP_DSCCODE_STR);
        cfgFPCA.numSamples          = sett.value("numSamples",          DEF_NUMSAMPLES).toInt();
        cfgFPCA.numSamplesPerImage  = sett.value("numSamplesPerImage",  DEF_NUMSMP_PER_IMG).toInt();
        cfgFPCA.sizeFlt             = sett.value("sizFlt",              DEF_FLT_SIZE).toInt();
        cfgFPCA.numBasis            = sett.value("numBasis",            DEF_NUM_BASIS).toInt();
        cfgFPCA.numSigmas           = sett.value("numSigmas",           DEF_NUM_SIGMS).toInt();
        sett.endGroup();
    } else {
        initConfigByDefault();
    }
}

QString DscPluginBFPCA::getFilterBankPrefix() const
{
    QString ret;
    if(isColor) {
        ret = QString("fbank_c%1r").arg(isRot4?1:0);
    } else {
        ret = QString("fbank_g%1r").arg(isRot4?1:0);
    }
    return ret;
}

void DscPluginBFPCA::buildDscFromPath(const std::string &imgPath)
{
    cv::Mat imgb;
    if(isColor) {
        imgb = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);
    } else {
        imgb = cv::imread(imgPath, CV_LOAD_IMAGE_GRAYSCALE);
    }
    buildDscFromMat(imgb);
}

void DscPluginBFPCA::buildDscFromMat(const cv::Mat &imgin)
{
    cv::Mat timg = imgin;
    int nch = imgin.channels();
    if(isColor && nch==1) {
        cv::cvtColor(imgin, timg, CV_GRAY2BGR);
    }
    if(!isColor && nch==3) {
        cv::cvtColor(imgin, timg, CV_BGR2GRAY);
    }
    if(timg.data) {
        cv::Mat img = resizeImageProportional(timg, DEF_IMAGE_BASESIZE);
        timg.release();
        if(dscType>0) {
            ConfigFPCA tmpCfg;
            tmpCfg = cfgFPCA;
            tmpCfg.isRot4           = isRot4;
            tmpCfg.isMultiChannel   = isColor;
            dsc.buildBinaryCompoundDescriptorNL1(bank, img, cfgDscFPCA);
        } else {
            std::cerr << "!!! ERROR incorrect parameters for build BFPCA-descriptro..." << std::endl;
        }
    } else {
        std::cerr << "!!! WARNING !!! image can't be loaded..." << std::endl;
    }
}

double DscPluginBFPCA::calcDscDistance(const DscPluginInterface *d) const
{
    return this->dsc.L1(((DscPluginBFPCA*)d)->dsc);
}

void DscPluginBFPCA::saveToStream(std::ofstream &ofs)
{
    dsc.bsave(ofs);
//    std::cout << "raw-save-dsc: " << dsc << std::endl;
}

void DscPluginBFPCA::loadFromStream(std::ifstream &ifs)
{
    dsc.bload(ifs);
//    std::cout << "raw-load-dsc: " << dsc << std::endl;
}

void DscPluginBFPCA::clear()
{
    dsc.clear();
}

void DscPluginBFPCA::init(const QString &pathToLoadedLibrary/*=""*/)
{
    qDebug() << "::DscPluginBFPCA::init()";
    bool ret = loadBaseConfig(pathToLoadedLibrary, DscPluginBFPCA::codeType, DscPluginBFPCA::codeSubTypes);
    if(!ret) {
        initDefault();
    } else {
        readConfigFPCA(getPathToDscConfig(pathToLoadedLibrary));
    }
}

bool DscPluginBFPCA::prepareDscData(const QString &path, const std::string &code) {
    qDebug() << "::DscPluginBFPCA::prepareDscData()";
    DscPluginBFPCA tmp;
    tmp.setStrCode(code);
    isColor                = tmp.isColor;
    isRot4                 = tmp.isRot4;
    cfgFPCA.isMultiChannel = tmp.isColor;
    cfgFPCA.isRot4         = tmp.isRot4;
    QString prefBnk = tmp.getFilterBankPrefix();
    qDebug() << " ---> prefBnk = " << prefBnk;
    QFileInfo infoPath(path);
    if(!infoPath.exists()) {
        qDebug() << "*** Error: can't find file-path [" << infoPath.absoluteFilePath() << "]";
        return false;
    }
    QFileInfo infoBnk(QString("%1.%2").arg(path).arg(prefBnk));
    if(infoBnk.exists()) {
        bank.loadFromFile(infoBnk.absoluteFilePath().toStdString());
    } else {
        std::vector<int> lstSgm;
        for(int ss=0; ss<cfgFPCA.numSigmas; ss++){
            lstSgm.push_back(ss);
        }
        bank.makeBankForImageList_MultiSigm(
                    infoPath.absoluteFilePath().toStdString(),
                    infoBnk.absoluteFilePath().toStdString(),
                    cfgFPCA,
                    lstSgm);
        bank.loadFromFile(infoBnk.absoluteFilePath().toStdString());
    }
    return true;
}

DscPluginInterface *DscPluginBFPCA::createObject(const std::string &dscCode) const
{
    if(dscCode.empty()) {
        return new DscPluginBFPCA();
    } else {
        return new DscPluginBFPCA(dscCode);
    }
}

QString DscPluginBFPCA::getCodeType() const
{
    return DscPluginBFPCA::codeType;
}

QStringList DscPluginBFPCA::getCodeSubTypes() const
{
    return DscPluginBFPCA::codeSubTypes;
}

QString DscPluginBFPCA::getSuffix() const
{
    return QString("%1_%2").arg(DscPluginBFPCA::codeType).arg(suffix);
}

QString DscPluginBFPCA::getShortInfo() const
{
    return DscPluginBFPCA::codeType;
}

void DscPluginBFPCA::initDefault()
{
    DscPluginBFPCA::codeType    = default_codeType;
    DscPluginBFPCA::codeSubTypes.clear();
    for(int ii=0; ii<default_numDscTypes; ii++) {
        codeSubTypes.append(QString(default_dscTypesCodes[ii]));
    }
}
