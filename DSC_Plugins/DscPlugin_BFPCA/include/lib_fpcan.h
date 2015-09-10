#ifndef _LIB_FPCAN_H
#define _LIB_FPCAN_H

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>

#include <cv.h>
#include <highgui.h>

#include "lib_helpers.h"

//////////////////////////////////////////////////
class ConfigFPCA {
public:
    ConfigFPCA() {
        init(0,11);
    }
    ConfigFPCA(int sigma, int size, int numSamples=1000) {
        init(sigma, size, numSamples);
    }
    ~ConfigFPCA() {}
    std::string toStdString() const;
    friend std::ostream& operator<< (std::ostream& os, const ConfigFPCA& cfg) {
        os << cfg.toStdString();
        return os;
    }
    void setNumCh(int numCh) {
        this->numCh   = numCh;
        if(numCh==1) {
            this->isMultiChannel = false;
        } else {
            this->isMultiChannel = true;
        }
    }
public:
    int sigma;
    int sizeFlt;
    int numSamples;
    //
    int numSamplesPerImage;
    bool isMultiChannel;
    bool isRot4;
    int numBasis;
    int numCh;
    //
    int numSigmas; //FIXME: for external usage, listSigmas = [0,1,...,numSigmas-1]
private:
    void init(int sigma, int size, int numSamples=1000);
};

//////////////////////////////////////////////////
/*
%
% Format binary-data:
% ---------------------------
% numChannels                  /int32
% num_scales(sigma)     = Ns   /int32
% size_frame_0                 /in32
% sigm_0                       /int32
% num_filters_per_scale = Nf0  /int32
% data_0_flt_0[ch_0]           /float64
% data_0_flt_0[ch_1]
% ....
% data_0_flt_0[numChannels-1]
% data_0_flt_1[...]
% ...
% data_0_flt_Nf0[...]
% ...
% ...
% size_frame_Ns
% sigm_Ns
% num_filters_per_scale = Nfs
% data_Ns_flt_0[..]
% data_Ns_flt_1[..]
% ...
% data_Ns_flt_Nfs[..]
% ---------------------------
*/
//////////////////////////////////////////////////
class FilterBankN {
public:
    FilterBankN() {
        clear();
    }
    void clear() {
        filterSize.clear();
        filterSigma.clear();
        filters.clear();
        //
        filtersWin.clear();
        imageChannelsNorm.clear();
    }
    bool isInitialized() const {
        if(filters.size()>0) {
            return true;
        }
        return false;
    }
    //
    bool        loadFromFile(const std::string& filePath);
    static bool makeBankForImage_MultiSimg(
            const cv::Mat& img,
            const std::string& fbank,
            const ConfigFPCA& cfg,
            const std::vector<int>& sigmas);
    static bool makeBankForImageList_MultiSigm(
            const std::string& fimgList,
            const std::string& fbank,
            const ConfigFPCA& cfg,
            const std::vector<int>& sigmas);
    //
    void    showFilterGUI();
    void    showMchImageGUI(const cv::Mat& img);
    void    showMchImagePanoramaGUI(const cv::Mat &img);
    std::string toString() const;
    std::string filePathBin;
    //
    std::vector<cv::Mat>*   getFiltersForScale(int scale);
    int                     getFilterSizeForScale(int scale) const;
    cv::Mat                 calcFilterImage(const cv::Mat& img, int numFilters);
    cv::Mat                 processFilterN       (const cv::Mat& img, const std::vector<cv::Mat> *listFilters, int numFilters);
    cv::Mat                 processFilterByIdx  (const cv::Mat& img, int fltSigmaIdx, int numFilters);
    cv::Mat                 processFilterBySigma(const cv::Mat& img, int fltSigmaVal, int numFilters);
    //
    cv::Mat                 processFilterMagnBySigma(const cv::Mat& img, int fltSigmaVal, int numFilters, bool isNormalized=false);
    cv::Mat                 processFilterMagnPairsBySigmaNrm(const cv::Mat& img, int fltSigmaVal, int numFiltersPairs, bool isNormalized=false);
    cv::Mat                 processFilterMagnPairsBySigmaSqr(const cv::Mat& img, int fltSigmaVal, int numFiltersPairs, bool isNormalized=false);
    //
    cv::Mat                 processBinaryFilterResponseBySigma(const cv::Mat& img, int fltSigmaVal, int numFilters);

    std::vector<int>                    filterSize;
    std::vector<int>                    filterSigma;
    std::vector<std::vector<cv::Mat> >  filters;
    std::vector<ConfigFPCA>             filterCfg;
    int                                 numChannels; //FIXME: doubled information
    //
    friend std::ostream& operator<< (std::ostream& out, const FilterBankN& b);
private:
    // visualisation Filters
    static void on_track_filter_sigma(int idx, void *);
    static std::vector<cv::Mat> filtersWin;
    static std::string          winNameFlts;
    static int                  trackFilterIdx;
    // visualisation multichannel-image
    static std::vector<cv::Mat> imageChannelsNorm;
    static void on_track_channel(int idx, void *);
    static std::string          winNameChannel;
    static int                  trackChannelIdx;
    // visualisation panorama for multichannel-image
    static std::string          winNamePanorama;
    cv::Mat get3ChannelFlt(const cv::Mat& flt);
};

//////////////////////////////////////////////////
cv::Mat rot90(const cv::Mat& img, bool isCW=true);
cv::Mat getGaussKernel2D(int siz, int sigm, int dtype=CV_64F);
cv::Mat getGaussKernel2D_NCh(int siz, int sigm, int nch, int dtype=CV_64F);
cv::Mat normFilter(const cv::Mat& flt);
cv::Mat reduceChannels(const cv::Mat& nchImage);

/////////////////////////////////////////////////
cv::Mat getPCASamplesFromImage(const cv::Mat& img, const ConfigFPCA& cfg);
cv::Mat getPCASamplesFromListOfImages(const std::string& fimgList, const ConfigFPCA& cfg);
std::vector<cv::Mat> buildFPCABasisForImage(const cv::Mat& img, ConfigFPCA& cfg);
std::vector<cv::Mat> buildFPCABasisForImageList(const std::string& fimgList, ConfigFPCA& cfg);

//////////////////////////////////////////////////

#endif
