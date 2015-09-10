#include "lib_fpcan_dsc.h"

////////////////////////////////////////////////////////
void calcHistogram(const cv::Mat& img, std::vector<double> &histBin, int numBit) {
    int maxBin  = (1<<numBit);
    histBin.clear();
    histBin.resize(maxBin, 0);
    int nrow        = img.rows;
    int ncol        = img.cols;
    const int*  p   = NULL;
    for(int rr=0; rr<nrow; rr++) {
        p   = img.ptr<int>(rr);
        for(int cc=0; cc<ncol; cc++) {
            histBin[p[cc]]++;
        }
    }
}

////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& out, const FPCANDescriptor& d) {
    out << d.toString();
    return out;
}

void FPCANDescriptor::set(const FPCANDescriptor& o) {
    this->sigma = o.sigma;
    this->numCh = o.numCh;
    this->fSize = o.fSize;
    this->data  = o.data;
}

void FPCANDescriptor::bsave(std::ofstream& fout) const
{
    fout.write((char*)&sigma,   sizeof(int));
    fout.write((char*)&numCh,   sizeof(int));
    fout.write((char*)&fSize,   sizeof(int));
    int dataSiz = data.size();
    fout.write((char*)&dataSiz, sizeof(int));
    fout.write(reinterpret_cast<const char*>(&data[0]), dataSiz*sizeof(double));
}
void FPCANDescriptor::bload(std::ifstream& fin ) {
    clear();
    fin.read((char*)&sigma,     sizeof(int));
    fin.read((char*)&numCh,     sizeof(int));
    fin.read((char*)&fSize,     sizeof(int));
    int dataSiz;
    fin.read((char*)&dataSiz,   sizeof(int));
    data.resize(dataSiz, 0);
    fin.read(reinterpret_cast<char*>(&data[0]), dataSiz*sizeof(double));
}

void FPCANDescriptor::copyTo(FPCANDescriptor& o) const {
    o.set(*this);
}
void FPCANDescriptor::copyFrom(const FPCANDescriptor& o) {
    set(o);
}

void FPCANDescriptor::clear() {
    data.clear();
    sigma   = -1;
    numCh   = -1;
    fSize   = -1;
}

void FPCANDescriptor::buildBinaryDescriptor(FilterBankN &bank,
        const cv::Mat& image,
        int fpcaSigma, int fpcaNumChannels, int border/*=0*/)
{
    CV_Assert(image.channels()==bank.numChannels);
    clear();
    this->sigma     = fpcaSigma;
    this->numCh     = fpcaNumChannels;
    int filterSize  = bank.getFilterSizeForScale(sigma);
    if(filterSize<0) {
        std::cerr << "ERROR::FPCADescriptor.buildBinaryDescriptor() : bad sigma-value [" << fpcaSigma << "], break" << std::endl;
        clear();
        return;
    }
    fSize   = filterSize;
    //
    cv::Mat imgBinFlt, imgBinFltCrop;
    imgBinFlt   = bank.processBinaryFilterResponseBySigma(image, sigma, numCh);
    if(border>0) {
        cv::Rect roi(border, border, image.cols-2*border, image.rows-2*border);
        imgBinFlt(roi).copyTo(imgBinFltCrop);
    } else {
        imgBinFltCrop   = imgBinFlt;
    }
    calcHistogram(imgBinFltCrop, data, numCh);
}

void FPCANDescriptor::buildBinaryDescriptorNormedL1(FilterBankN &bank, const cv::Mat &image, int fpcaSigma, int fpcaNumChannels, int border)
{
    buildBinaryDescriptor(bank, image, fpcaSigma, fpcaNumChannels, border);
    normalizeL1();
}

void FPCANDescriptor::buildBinaryCompoundDescriptorNL1(
        FilterBankN &bank,
        const cv::Mat &image,
        const ConfigDscFPCA &cfg, int border)
{
    clear();
    int numDsc = cv::min(cfg.listSigm.size(), cfg.listNumFlt.size());
    sigma = -1; //FIXME: not good solution for compoud descriptor case
    numCh =  0;
    for(int ii=0; ii<numDsc; ii++) {
        int sigm = cfg.listSigm[ii];
        int numf = cfg.listNumFlt[ii];
        FPCANDescriptor tmpDsc;
        tmpDsc.buildBinaryDescriptorNormedL1(bank, image, sigm, numf, border);
        data.insert(data.end(), tmpDsc.data.begin(),  tmpDsc.data.end());
        numCh += numf;
    }
    normalizeL1();
}

void FPCANDescriptor::normalizeL1()
{
    int sizDsc = data.size();
    if(sizDsc>0) {
        double sum = 0;
        for(int ii=0; ii<sizDsc; ii++) {
            sum += data[ii];
        }
        if(sum>0) {
            for(int ii=0; ii<sizDsc; ii++) {
                data[ii]/=sum;
            }
        }
    }
}

void FPCANDescriptor::normalizeL2()
{
    int sizDsc = data.size();
    if(sizDsc>0) {
        double sum = 0;
        for(int ii=0; ii<sizDsc; ii++) {
            sum += data[ii]*data[ii];
        }
        if(sum>0) {
            sum = sqrt(sum);
            for(int ii=0; ii<sizDsc; ii++) {
                data[ii]/=sum;
            }
        }
    }
}

double FPCANDescriptor::L1(const FPCANDescriptor &dsc) const
{
    int sizDsc = cv::min(data.size(), dsc.data.size());
    double dst = 0.;
    for(int ii=0; ii<sizDsc; ii++) {
        dst += fabs(data[ii] - dsc.data[ii]);
    }
    return dst;
}

double FPCANDescriptor::L2(const FPCANDescriptor &dsc) const
{
    int sizDsc = cv::min(data.size(), dsc.data.size());
    double dst = 0.;
    double tmp;
    for(int ii=0; ii<sizDsc; ii++) {
        tmp = data[ii] - dsc.data[ii];
        dst += tmp*tmp;
    }
    return sqrt(dst);
}

std::string FPCANDescriptor::toString(bool isFullPrint/*=true*/) const
{
    std::stringstream ss;
    ss << "FPCA-Dsc( sigma=" << sigma << ", numCh=" << numCh << ", filterSize=" << fSize << ") :";
    if(isFullPrint) {
        ss << "\n\tdata[" << data.size() << "] = {";
        for(int ii=0; ii<data.size(); ii++) {
            ss << ii << ":" << data[ii] << ", ";
        }
        ss << "}";
    }
    return ss.str();
}

std::string FPCANDescriptor::toCSVString() const
{
    std::stringstream ss;
    for(int ii=0; ii<data.size(); ii++) {
        if(ii>0) {
            ss << ", ";
        }
        ss << data[ii];
    }
    return ss.str();
}
