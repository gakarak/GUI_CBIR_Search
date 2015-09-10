#ifndef _LIB_FPCAN_DSC_H
#define _LIB_FPCAN_DSC_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <cv.h>
#include <highgui.h>

#include "lib_fpcan.h"

////////////////////////////////////////////////////////
void calcHistogram(const cv::Mat& img, std::vector<double>& histBin, int numBit);

////////////////////////////////////////////////////////
class ConfigDscFPCA {
public:
    ConfigDscFPCA() {
        listSigm.clear();
        listNumFlt.clear();
    }
    ConfigDscFPCA(const std::vector<int>& listSigm, const std::vector<int>& listNumFlt) {
        this->listSigm      = listSigm;
        this->listNumFlt    = listNumFlt;
    }
    void clear() {
        listSigm.clear();
        listNumFlt.clear();
    }

    ~ConfigDscFPCA() {}
    std::string toStdString() const {
        std::stringstream ss;
        ss << "cfgDsc {";
        int numMin = cv::min(listSigm.size(), listNumFlt.size());
        for(int ii=0; ii<numMin; ii++) {
            ss << " " << listSigm[ii] << ":" << listNumFlt[ii] << ", ";
        }
        ss << "}";
        return ss.str();
    }
    friend std::ostream& operator<<(std::ostream& out, const ConfigDscFPCA& cfg) {
        out << cfg.toStdString();
        return out;
    }
    //
    std::vector<int> listSigm;
    std::vector<int> listNumFlt;
};

class FPCANDescriptor {
public:
    FPCANDescriptor() { clear(); }
    virtual ~FPCANDescriptor() {}
    void set(const FPCANDescriptor& o);
    //
    void buildBinaryDescriptor(FilterBankN& bank, const cv::Mat& image, int fpcaSigma, int fpcaNumChannels, int border=0);
    void buildBinaryDescriptorNormedL1(FilterBankN& bank, const cv::Mat& image, int fpcaSigma, int fpcaNumChannels, int border=0);
    void buildBinaryCompoundDescriptorNL1(
            FilterBankN& bank,
            const cv::Mat& image,
            const ConfigDscFPCA& cfg, int border=0);
    void normalizeL1();
    void normalizeL2();
    double L1(const FPCANDescriptor& dsc) const;
    double L2(const FPCANDescriptor& dsc) const;
    //
    void clear();
    std::string toString(bool isFullPrint=true) const;
    std::string toCSVString() const;
    //
    void bsave   (std::ofstream& fout) const;
    void bload   (std::ifstream& fin );
    void copyTo  (FPCANDescriptor& o  ) const;
    void copyFrom(const FPCANDescriptor& o);
    friend std::ostream& operator<< (std::ostream& out, const FPCANDescriptor& d);
    //
    std::vector<double> data;
    int sigma;
    int numCh;
    int fSize;
};

#endif
