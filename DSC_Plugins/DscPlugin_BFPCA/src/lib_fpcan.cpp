#include "lib_fpcan.h"

#include <cmath>

/*
 * static mambers
 */
/////////////////////////////////////////////////////////
std::vector<cv::Mat> FilterBankN::filtersWin;
std::string          FilterBankN::winNameFlts        = "filters-bank";
int                  FilterBankN::trackFilterIdx     = 0;
//
std::vector<cv::Mat> FilterBankN::imageChannelsNorm;
std::string          FilterBankN::winNameChannel     = "multi-channel";
int                  FilterBankN::trackChannelIdx    = 0;
//
std::string          FilterBankN::winNamePanorama    = "panorama multi-channel";

/*
 * friend functions
 */
/////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& out, const FilterBankN& b) {
    out << b.toString();
    return out;
}

/*
 * methods
 */
/////////////////////////////////////////////////////////
std::string FilterBankN::toString() const {
    std::stringstream ss;
    ss << "bank(" << filters.size() << ":" << numChannels << "): size/sigma = {";
    for(uint ii=0; ii<filters.size(); ii++) {
        ss << " (" << ii << ": " << filterSize[ii] << "/" << filterSigma[ii] << "), ";
    }
    ss << "}";
    return ss.str();
}

cv::Mat FilterBankN::processFilterN(const cv::Mat& img, const std::vector<cv::Mat>* listFilters, int numFilters) {
    cv::Mat ret;
    if(listFilters!=NULL && numFilters>0 && numFilters<=listFilters->size()) {
        std::vector<cv::Mat> imgFltrd;
        for(int ii=0; ii<numFilters; ii++) {
            const cv::Mat& cflt = listFilters->at(ii);
            cv::Mat tmp;
            cv::filter2D(img, tmp, CV_64F, cflt);
            imgFltrd.push_back(reduceChannels(tmp));
        }
        cv::merge(imgFltrd, ret);
    }
    return ret;
}

cv::Mat FilterBankN::processFilterByIdx  (const cv::Mat& img, int fltSigmaIdx, int numFilters) {
    cv::Mat ret;
    if(fltSigmaIdx>-1 && fltSigmaIdx<filters.size()) {
        const std::vector<cv::Mat>* listFlt = &filters.at(fltSigmaIdx);
        ret = processFilterN(img, listFlt, numFilters);
    }
    return ret;
}

cv::Mat FilterBankN::processFilterBySigma(const cv::Mat& img, int fltSigmaVal, int numFilters) {
    cv::Mat ret;
    std::vector<cv::Mat>* listFlt = getFiltersForScale(fltSigmaVal);
    if(listFlt!=NULL) {
        ret = processFilterN(img, listFlt, numFilters);
    }
    return ret;
}

cv::Mat FilterBankN::processFilterMagnBySigma(const cv::Mat &img, int fltSigmaVal, int numFilters, bool isNormalized/*=false*/)
{
    cv::Mat ret, tmp;
    tmp = processFilterBySigma(img, fltSigmaVal, numFilters);
    if(tmp.data) {
        int     nrow    = tmp.rows;
        int     ncol    = tmp.cols;
        int     cnt     = 0;
        double  val     = 0.;
        double  tval    = 0.;
        int     nch     = tmp.channels();
        ret = cv::Mat::zeros(nrow, ncol, tmp.depth());
        const double*   pFpca     = NULL;
        double*         pFpcaMag  = NULL;
        for(int rr=0; rr<nrow; rr++) {
            pFpca       = tmp.ptr<double>(rr);
            pFpcaMag    = ret.ptr<double>(rr);
            cnt = 0;
            for(int cc=0; cc<ncol; cc++) {
                val = 0.;
                for(int ch=0; ch<nch; ch++) {
                    tval     = pFpca[cnt+ch];
                    val     += tval*tval;
                }
                pFpcaMag[cc] = val/nch;
                cnt += nch;
            }
        }
        tmp.release();
        if(isNormalized) {
            cv::normalize(ret, ret, 0,255, CV_MINMAX, CV_8U);
        }
    }
    return ret;
}

cv::Mat FilterBankN::processFilterMagnPairsBySigmaNrm(const cv::Mat &img, int fltSigmaVal, int numFiltersPairs, bool isNormalized)
{
    cv::Mat ret = processFilterMagnPairsBySigmaSqr(img, fltSigmaVal, numFiltersPairs, isNormalized);
    if(ret.data) {
        int     nrow    = ret.rows;
        int     ncol    = ret.cols;
        int     nch     = ret.channels();
        for(int chi=0; chi<nch; chi++) {
            for(int rr=0; rr<nrow; rr++) {
                double* ptr = ret.ptr<double>(rr);
                int cnt     = 0;
                for(int cc=0; cc<ncol; cc++) {
                    for(int hh=0; hh<nch; hh++) {
                        ptr[cnt]    = sqrt(ptr[cnt]);
                        cnt++;
                    }
                }
            }
        }
    }
    return ret;
}

cv::Mat FilterBankN::processFilterMagnPairsBySigmaSqr(const cv::Mat &img, int fltSigmaVal, int numFiltersPairs, bool isNormalized)
{
    cv::Mat ret, tmp;
    int numFilters  = 2*numFiltersPairs;
    tmp = processFilterBySigma(img, fltSigmaVal, numFilters);
    if(tmp.data) {
        int     nrow    = tmp.rows;
        int     ncol    = tmp.cols;
        int     nch     = tmp.channels();
        double  val     = 0.;
        double  tval0   = 0.;
        double  tval1   = 0.;
        std::vector<cv::Mat> lstImgMagPair;
        for(int chi=0; chi<numFilters; chi+=2) {
            int ch0 = chi+0;
            int ch1 = chi+1;
            cv::Mat tmpImgMagPair   = cv::Mat::zeros(nrow, ncol, CV_64F);
            for(int rr=0; rr<nrow; rr++) {
                double* ptrFrom = tmp.ptr<double>(rr);
                double* ptrTo   = tmpImgMagPair.ptr<double>(rr);
                int     cnt     = 0;
                for(int cc=0; cc<ncol; cc++) {
                    tval0       = ptrFrom[cnt+ch0];
                    tval1       = ptrFrom[cnt+ch1];
                    val         = tval0*tval0 + tval1*tval1;
                    ptrTo[cc]   = val/nch;
                    cnt+=nch;
                }
            }
//            if (isNormalized) {
//                cv::normalize(tmpImgMagPair, tmpImgMagPair, 0,255, CV_MINMAX, CV_8U);
//            }
            lstImgMagPair.push_back(tmpImgMagPair);
        }
        cv::merge(lstImgMagPair, ret);
    }
    return ret;
}

cv::Mat FilterBankN::processBinaryFilterResponseBySigma(const cv::Mat &img, int fltSigmaVal, int numFilters)
{
    cv::Mat ret, flt;
    flt = processFilterBySigma(img, fltSigmaVal, numFilters);
    if(flt.data) {
        CV_Assert(flt.channels()==numFilters);
        int nrow    = flt.rows;
        int ncol    = flt.cols;
        ret     = cv::Mat::zeros(nrow, ncol, CV_32S);
        double* pFlt    = NULL;
        int*    pRet    = NULL;
        int     val     = 0;
        int     cnt     = 0;
        int     nch     = flt.channels();
        for(int rr=0; rr<nrow; rr++) {
            pFlt    = flt.ptr<double>(rr);
            pRet    = ret.ptr<int>(rr);
            cnt     = 0;
            for(int cc=0; cc<ncol; cc++) {
                val     = 0;
                for(int ch=0; ch<nch; ch++) {
                    val |= ((int)(pFlt[cnt+ch]>0.)<<ch)-0;
                }
                pRet[cc]    = val;
                cnt += nch;
            }
        }
    }
    return ret;
}

std::vector<cv::Mat>* FilterBankN::getFiltersForScale(int scale) {
    if(filters.size()>0) {
        std::vector<int>::const_iterator it;
        it  = std::find(filterSigma.begin(), filterSigma.end(), scale);
        if(it!=filterSigma.end()) {
            const std::vector<int>::size_type idx   = it - filterSigma.begin();
            return &filters[idx];
        } else {
            return NULL;
        }
    }
    return NULL;
}

int FilterBankN::getFilterSizeForScale(int scale) const
{
    if(filters.size()>0) {
        std::vector<int>::const_iterator it;
        it  = std::find(filterSigma.begin(), filterSigma.end(), scale);
        if(it!=filterSigma.end()) {
            const std::vector<int>::size_type idx   = it - filterSigma.begin();
            return filterSize[idx];
        } else {
            return -1;
        }
    }
    return -1;
}

bool FilterBankN::loadFromFile(const std::string& filePath) {
    clear();
    std::ifstream ifs(filePath.c_str(), std::ifstream::binary);
    if(!ifs) {
        std::cerr << "Can't fin file [" << filePath << "]" << std::endl;
        return false;
    }
    ifs.read((char*)&numChannels, sizeof(int));
    if(numChannels<1) {
        std::cerr << "Bad number of Channels #" << numChannels << std::endl;
        return false;
    }
    int numFilters  = -1;
    ifs.read((char*)&numFilters, sizeof(int));
    if(numFilters<1) {
        std::cerr << "Bad size filters-bank #" << numFilters << std::endl;
        return false;
    }
    for(int ii=0; ii<numFilters; ii++) {
        ConfigFPCA cfgTmpl;
        cfgTmpl.setNumCh(numChannels);
        //
        std::vector<cv::Mat> tmpListFiltersOnSigm;
        int tmpFilterSize           = 0;
        int tmpFilterSigm           = 0;
        int tmpNumFiltersOnSigm     = 0;
        ifs.read((char*)&tmpFilterSize,         sizeof(int));
        ifs.read((char*)&tmpFilterSigm,         sizeof(int));
        ifs.read((char*)&tmpNumFiltersOnSigm,   sizeof(int));
        filterSize.push_back(tmpFilterSize);
        filterSigma.push_back(tmpFilterSigm);
        cfgTmpl.sizeFlt = tmpFilterSize;
        cfgTmpl.sigma   = tmpFilterSigm;
        filterCfg.push_back(cfgTmpl);
        //
        int tmpFilterSize2   = tmpFilterSize*tmpFilterSize*numChannels;
        double* tmpData = new double[tmpFilterSize2];
        for(int ss=0; ss<tmpNumFiltersOnSigm; ss++) {
            ifs.read((char*)tmpData, tmpFilterSize2*sizeof(double));
            cv::Mat tmpMat  = cv::Mat(tmpFilterSize, tmpFilterSize, CV_64FC(numChannels), tmpData).clone();
            tmpListFiltersOnSigm.push_back(tmpMat);
        }
        delete [] tmpData;
        filters.push_back(tmpListFiltersOnSigm);
    }
    return true;
}

bool FilterBankN::makeBankForImage_MultiSimg(
        const cv::Mat &img,
        const std::string &fbank,
        const ConfigFPCA &cfg,
        const std::vector<int> &sigmas)
{
    ConfigFPCA cfgIter = cfg;
    int numCh   = img.channels();
    int numSigm = sigmas.size();
    int sizFlt  = cfg.sizeFlt;
    int dataSize    = sizFlt*sizFlt*numCh*sizeof(double);
    cfgIter.numCh   = numCh;
    if(numCh==1) {
        cfgIter.isMultiChannel  = false;
    } else {
        cfgIter.isMultiChannel  = true;
    }
    std::ofstream ofs(fbank.c_str(), std::ofstream::binary);
    if(!ofs) {
        std::cerr << "Can't open file [" << fbank << "] for writing" << std::endl;
        return false;
    }
    ofs.write((char*)&numCh,    sizeof(int));
    ofs.write((char*)&numSigm,  sizeof(int));
    for(int ii=0; ii<numSigm; ii++) {
        int cSgm        = sigmas.at(ii);
        cfgIter.sigma   = cSgm;
        std::cout << "(" << ii<< ") : " << cfgIter << std::endl;
        //
        std::vector<cv::Mat> lstPC = buildFPCABasisForImage(img, cfgIter);
        //
        int numPC = lstPC.size();
        ofs.write((char*)&sizFlt, sizeof(int));
        ofs.write((char*)&cSgm,   sizeof(int));
        ofs.write((char*)&numPC,  sizeof(int));
        for(int kk=0; kk<numPC; kk++) {
            cv::Mat cflt = lstPC.at(kk);
            double* ptr     = cflt.ptr<double>(0);
            ofs.write((char*)ptr, dataSize);
        }
    }
    ofs.close();
    return true;
}

bool FilterBankN::makeBankForImageList_MultiSigm(const std::string &fimgList, const std::string &fbank, const ConfigFPCA &cfg, const std::vector<int> &sigmas)
{
    ConfigFPCA cfgIter = cfg;
    int numCh   = 1;
    if(cfg.isMultiChannel) {
        numCh  = 3;
    }
    int numSigm = sigmas.size();
    int sizFlt  = cfg.sizeFlt;
    int dataSize    = sizFlt*sizFlt*numCh*sizeof(double);
    std::ofstream ofs(fbank.c_str(), std::ofstream::binary);
    if(!ofs) {
        std::cerr << "Can't open file [" << fbank << "] for writing" << std::endl;
        return false;
    }
    ofs.write((char*)&numCh,    sizeof(int));
    ofs.write((char*)&numSigm,  sizeof(int));
    for(int ii=0; ii<numSigm; ii++) {
        int cSgm        = sigmas.at(ii);
        cfgIter.sigma   = cSgm;
        std::cout << "(" << ii<< ") : " << cfgIter << std::endl;
        //
        std::vector<cv::Mat> lstPC = buildFPCABasisForImageList(fimgList, cfgIter);
        //
        int numPC = lstPC.size();
        ofs.write((char*)&sizFlt, sizeof(int));
        ofs.write((char*)&cSgm,   sizeof(int));
        ofs.write((char*)&numPC,  sizeof(int));
        for(int kk=0; kk<numPC; kk++) {
            cv::Mat cflt = lstPC.at(kk);
            double* ptr     = cflt.ptr<double>(0);
            ofs.write((char*)ptr, dataSize);
        }
    }
    ofs.close();
    return true;
}

void FilterBankN::showFilterGUI() {
    if(filters.size()>0) {
        const int sizFltGui     = 160;
        const int border        = 8;
        const int sizFltGuiB    = sizFltGui+2*border;
        filtersWin.clear();
        trackFilterIdx  = 0;
        for(int ii=0; ii<filters.size(); ii++) {
            const std::vector<cv::Mat>& tmpFilters  = filters.at(ii);
            int numr    = floor(sqrt((double)tmpFilters.size()));
            int numc    = ceil((double)tmpFilters.size()/(double)numr);
            cv::Mat tmp = cv::Mat::zeros(numr*sizFltGuiB, numc*sizFltGuiB, CV_64FC3);
//            if(numChannels==1) {
//                tmp = cv::Mat::zeros(numr*sizFltGuiB, numc*sizFltGuiB, CV_64F);
//            } else {
//                tmp = cv::Mat::zeros(numr*sizFltGuiB, numc*sizFltGuiB, CV_64FC3);
//            }
            int pr  = 0;
            int pc  = 0;
            for(int ff=0; ff<tmpFilters.size(); ff++) {
                cv::Mat cflt;
                cv::Mat tmpf = get3ChannelFlt(tmpFilters[ff]);
                cv::resize(tmpf, cflt, cv::Size(sizFltGui, sizFltGui), 0,0, CV_INTER_NN);
                cv::Rect roi(border+sizFltGuiB*pc, border+sizFltGuiB*pr, sizFltGui, sizFltGui);
                cflt.copyTo(tmp(roi));
                pc++;
                if(pc>=numc) {
                    pr++;
                    pc  = 0;
                }
            }
            cv::normalize(tmp, tmp, 0,255, CV_MINMAX, CV_8U);
            filtersWin.push_back(tmp);
        }
        cv::namedWindow(winNameFlts);
        cv::createTrackbar("filter Id", winNameFlts, &trackFilterIdx, filters.size()-1, FilterBankN::on_track_filter_sigma, 0);
        cv::imshow(winNameFlts, filtersWin.at(trackFilterIdx));
    }
}

cv::Mat FilterBankN::get3ChannelFlt(const cv::Mat &flt) {
    std::vector<cv::Mat> spl;
    cv::split(flt, spl);
    std::vector<cv::Mat> mrg;
    int nch = numChannels;
    for(int ii=0; ii<3; ii++) {
        if(ii<nch) {
            mrg.push_back(spl.at(ii));
        } else {
            mrg.push_back(spl.at(nch-1));
        }
    }
    cv::Mat ret;
    cv::merge(mrg, ret);
    return ret;
}

void FilterBankN::on_track_filter_sigma(int idx, void*) {
    if(idx>-1 && idx <filtersWin.size()) {
        cv::imshow(winNameFlts, filtersWin.at(idx));
    }
}

void FilterBankN::showMchImageGUI(const cv::Mat &img) {
    if(img.data) {
        imageChannelsNorm.clear();
        std::vector<cv::Mat> imgSpl;
        cv::split(img, imgSpl);
        for(int ii=0; ii<imgSpl.size(); ii++) {
            cv::Mat tmp;
            cv::normalize(imgSpl.at(ii), tmp, 0,255, CV_MINMAX, CV_8U);
            imageChannelsNorm.push_back(tmp);
        }
        cv::namedWindow(winNameChannel);
        int trackNum    = imageChannelsNorm.size()-1;
        if(trackNum<1) trackNum = 1;
        cv::createTrackbar("channel #", winNameChannel, &trackChannelIdx, trackNum, FilterBankN::on_track_channel, 0);
        cv::imshow(winNameChannel, imageChannelsNorm.at(trackChannelIdx));
    } else {
        std::cerr << "bad image data" << std::endl;
    }
}

void FilterBankN::on_track_channel(int idx, void *) {
    if(idx>-1 && idx <imageChannelsNorm.size()) {
        cv::imshow(winNameChannel, imageChannelsNorm.at(idx));
    }
}

void FilterBankN::showMchImagePanoramaGUI(const cv::Mat& img) {
    if(img.data) {
        std::vector<cv::Mat> imgSpl;
        cv::split(img, imgSpl);
        int sizPan  = 900;
        double  koeff   = sqrt((double)(sizPan*sizPan)/((double)imgSpl.size()*img.cols*img.rows));
        int newCols     = img.cols*koeff;
        int newRows     = img.rows*koeff;
        cv::Size newSiz = cv::Size(newCols, newRows);
        int numr        = floor(sqrt((double)imgSpl.size()));
        int numc        = ceil((double)imgSpl.size()/numr);
        //
        cv::Mat panrm   = cv::Mat::zeros(newRows*numr, newCols*numc, CV_MAKETYPE(img.depth(),1));
        int pr  = 0;
        int pc  = 0;
        for(int ii=0; ii<imgSpl.size(); ii++) {
            const cv::Mat& tmpCh    = imgSpl.at(ii);
            cv::Mat tmpChRes;
            cv::resize(tmpCh, tmpChRes, newSiz);
            cv::Rect roi(newCols*pc, newRows*pr, newCols, newRows);
            tmpChRes.copyTo(panrm(roi));
            pc++;
            if(pc>=numc) {
                pr++;
                pc  = 0;
            }
        }
        cv::normalize(panrm, panrm, 0,255, CV_MINMAX, CV_8U);
        cv::imshow(winNamePanorama, panrm);
    } else {
        std::cerr << "bad image data" << std::endl;
    }
}

//////////////////////////////////////////////////
std::string ConfigFPCA::toStdString() const {
    std::stringstream ss;
    ss << "cfg { "
       << "sigma: "         << this->sigma      << ", "
       << "sizeFlt: "       << this->sizeFlt    << ", "
       << "numSmpl: "       << this->numSamples << ", "
       << "numSmplPerImg: " << this->numSamplesPerImage << ", "
       << "isMultiCh: "     << this->isMultiChannel << ", "
       << "isRot4: "        << this->isRot4     << ", "
       << "numBasis: "      << this->numBasis   << ", "
       << "numCh: "         << this->numCh << "; "
       << "numSigm: "       << this->numSigmas << " }"
       << std::endl;
    return ss.str();
}

void ConfigFPCA::init(int sigma, int size, int numSamples) {
    this->sigma         = sigma;
    this->sizeFlt          = size;
    this->numSamples    = numSamples;
    //
    this->numSamplesPerImage = 100;
    this->isMultiChannel     = false;
    this->isRot4             = false;
    this->numBasis           = 10;
    this->numCh              = 1;
}

//////////////////////////////////////////////////
cv::Mat rot90(const cv::Mat &img, bool isCW) {
    cv::Mat ret;
    if(isCW) {
        cv::transpose(img, ret);
        cv::flip(ret, ret, 1);
    } else {
        cv::transpose(img, ret);
        cv::flip(ret, ret, 0);
    }
    return ret;
}

cv::Mat getGaussKernel2D(int siz, int sigm, int dtype) {
    cv::Mat ret;
    if(sigm==0) {
        ret = cv::Mat::ones(siz, siz, dtype);
    } else {
        cv::Mat img     = cv::Mat::zeros(siz, siz, dtype);
        int p0 = (siz/2) + 0;
        img.at<double>(p0,p0)=1;
        cv::GaussianBlur(img, ret, cv::Size(siz, siz), sigm,sigm);
    }
    return ret;
}

cv::Mat getGaussKernel2D_NCh(int siz, int sigm, int nch, int dtype) {
    cv::Mat gauss = getGaussKernel2D(siz, sigm, dtype);
    if(nch==1) {
        return gauss;
    } else {
        std::vector<cv::Mat> arr;
        for(int ii=0; ii<nch; ii++) {
            arr.push_back(gauss.clone());
        }
        cv::Mat ret;
        cv::merge(arr, ret);
        return ret;
    }
}

cv::Mat normFilter(const cv::Mat &flt) {
    cv::Mat ret;
    double stdThresh = 0.001;
    if(flt.channels()==1) {
        cv::Scalar mean, std;
        cv::meanStdDev(flt, mean, std);
        if(std[0]<stdThresh) {
            ret = (flt-mean[0])/1.0;
        } else {
            ret = (flt-mean[0])/std[0];
        }
    } else {
        std::vector<cv::Mat> fltSplit, fltMerg;
        cv::split(flt, fltSplit);
        double std2;
        int nch = flt.channels();
        for(int ii=0; ii<nch; ii++) {
            cv::Scalar tm, ts;
            cv::Mat tmp = fltSplit.at(ii);
            cv::meanStdDev(tmp, tm, ts);
            fltMerg.push_back(tmp-tm[0]);
            std2 += ts[0]*ts[0];
        }
        cv::merge(fltMerg, ret);
        double std = sqrt(std2);
        if(std>stdThresh) {
            ret = ret/std;
        }
    }
    return ret;
}

cv::Mat reduceChannels(const cv::Mat &nchImage) {
    cv::Mat tmpr = nchImage.reshape(1, nchImage.cols*nchImage.rows);
    cv::Mat ret;
    cv::reduce(tmpr, ret, 1, CV_REDUCE_SUM);
    ret = ret.reshape(1,nchImage.rows);
    return ret;
}

//////////////////////////////////////////////////
cv::Mat getPCASamplesFromImage(const cv::Mat &img, const ConfigFPCA &cfg) {
    int numSamples  = cfg.numSamples;
    int siz         = cfg.sizeFlt;
    int sigm        = cfg.sigma;
    bool isRot4     = cfg.isRot4;
    //
    cv::Mat imgd, ret;
    int nch = img.channels();
    int nr  = img.rows;
    int nc  = img.cols;
    int dscSize = nch*siz*siz;
    img.convertTo(imgd, CV_64F);
    ret = cv::Mat::zeros(numSamples,   dscSize, CV_64F);
    int maxr    = nr - siz - 1;
    int maxc    = nc - siz - 1;
    cv::Rect roi(0,0, siz, siz);
    int cnt = 0;
    cv::Mat gauss   = getGaussKernel2D_NCh(siz, sigm, nch);
    cv::Mat qqq;
    for(int ii=0; ii<numSamples; ii++) {
        int cc = cv::theRNG().uniform(0,maxc);
        int rr = cv::theRNG().uniform(0,maxr);
        roi.x = cc;
        roi.y = rr;
        cv::Mat tmp     = imgd(roi).clone();
        tmp = tmp.mul(gauss);
        //
        //        cv::Mat qqq;
        //        cv::resize(tmp, qqq, cv::Size(256,256));
        //        cv::normalize(qqq, qqq, 0,255, CV_MINMAX, CV_8U);
        //        cv::imshow("aaa", qqq);
        //        cv::waitKey(0);
        //
        ret.row(cnt) = tmp.clone().reshape(1,1)*1.0; cnt++;
        if(isRot4) {
            tmp = rot90(tmp);
            ret.row(cnt) = tmp.clone().reshape(1,1)*1.0; cnt++;
            if(cnt>=numSamples) break;
            //
            tmp = rot90(tmp);
            ret.row(cnt) = tmp.clone().reshape(1,1)*1.0; cnt++;
            if(cnt>=numSamples) break;
            //
            tmp = rot90(tmp);
            ret.row(cnt) = tmp.clone().reshape(1,1)*1.0; cnt++;
            if(cnt>=numSamples) break;
        }
    }
    return ret;
}


cv::Mat getPCASamplesFromListOfImages(const std::string &fimgList, const ConfigFPCA &cfg) {
    int numSamples  = cfg.numSamples;
    int numSamplesPerImage = cfg.numSamplesPerImage;
    bool isColored  = cfg.isMultiChannel;
    //
    int numIterMax = floor(numSamples/numSamplesPerImage)+1;
    cv::Mat ret;
    bool isRetInit = false;
    StringList strList;
    cv::Rect roi;
    ConfigFPCA tmpCfg = cfg;
    if( readStringListFromFile(fimgList, strList) ) {
        if(strList.size()>0) {
            int numImg = strList.size();
            int cnt = 0;
            for(int it=0; it<numIterMax; it++) {
                if(cnt>=numSamples) {
                    break;
                }
                int imgIdx = cv::theRNG().uniform(0,numImg);
                int smpPerImg = numSamplesPerImage;
                if((cnt+smpPerImg)>=numSamples) {
                    smpPerImg = numSamples - cnt;
                }
                std::string cfimg = strList.at(imgIdx);
                cv::Mat cimg;
                if(isColored) {
                    cimg = cv::imread(cfimg, CV_LOAD_IMAGE_COLOR);
                } else {
                    cimg = cv::imread(cfimg, CV_LOAD_IMAGE_GRAYSCALE);
                }
                if(!cimg.data) {
                    continue;
                }
                tmpCfg.numSamples = smpPerImg;
                cv::Mat retFPCA = getPCASamplesFromImage(cimg, tmpCfg);
                if(!isRetInit) {
                    int nc = retFPCA.cols;
                    ret = cv::Mat::zeros(numSamples, nc, CV_64F);
                    roi.x       = 0;
                    roi.width   = nc;
                    roi.height  = numSamplesPerImage;
                    isRetInit = true;
                }
                //
                roi.y       = cnt;
                roi.height  = smpPerImg;
                retFPCA.copyTo(ret(roi));
                cnt += smpPerImg;
            }
        } else {
            std::cerr << "*** Error: string inf file [" << fimgList << "] not found" << std::endl;
        }
    } else {
        std::cerr << "*** Error: can't find file [" << fimgList << "]" << std::endl;
    }
    return ret;
}


std::vector<cv::Mat> buildFPCABasisForImage(const cv::Mat &img, ConfigFPCA &cfg) {
    cv::Mat pcaMat = getPCASamplesFromImage(img, cfg);
    if(img.channels()>1) {
        cfg.isMultiChannel = true;
        cfg.numCh = img.channels();
    } else {
        cfg.isMultiChannel = false;
        cfg.numCh = 1;
    }
    cv::PCA pca;
    pca = cv::PCA(pcaMat, cv::Mat(), /*cv::PCA::DATA_AS_ROW*/ CV_PCA_DATA_AS_ROW, cfg.numBasis);
    if(pca.eigenvectors.rows<cfg.numBasis) {
        cfg.numBasis = pca.eigenvectors.rows;
    }
    //
    int nch = img.channels();
    int siz = cfg.sizeFlt;
    int npc = cfg.numBasis;
    std::vector<cv::Mat> ret;
    for(int ii=0; ii<npc; ii++) {
        cv::Mat tmp  = pca.eigenvectors.row(ii).clone().reshape(nch,siz);
        cv::Mat tmpn = normFilter(tmp);
        ret.push_back(tmpn);
    }
    return ret;
}


std::vector<cv::Mat> buildFPCABasisForImageList(const std::string &fimgList, ConfigFPCA &cfg) {
    cv::Mat pcaMat = getPCASamplesFromListOfImages(fimgList, cfg);
    cv::PCA pca;
    pca = cv::PCA(pcaMat, cv::Mat(), CV_PCA_DATA_AS_ROW, cfg.numBasis);
    if(pca.eigenvectors.rows<cfg.numBasis) {
        cfg.numBasis = pca.eigenvectors.rows;
    }
    //FIXME: in multichannel case this is bad solution
    int nch = 1;
    if(cfg.isMultiChannel) {
        nch = 3;
    }
    int siz = cfg.sizeFlt;
    int npc = cfg.numBasis;
    std::vector<cv::Mat> ret;
    for(int ii=0; ii<npc; ii++) {
        cv::Mat tmp  = pca.eigenvectors.row(ii).clone().reshape(nch,siz);
        cv::Mat tmpn = normFilter(tmp);
        ret.push_back(tmpn);
    }
    return ret;
}
