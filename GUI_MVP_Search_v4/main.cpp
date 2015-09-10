#include "mainwindow.h"
#include <QApplication>

//#include <cv.h>
//#include <highgui.h>

//#include "lib_dsc.h"
//#include <pHash.h>

//#include "lib_descriptor2sl.h"
//#include <iostream>
//#include "dscplugincoomatrix.h"

//using namespace dscv2sl;
//using namespace std;


#define DEF_IMG_LENA    "/home/ar/img/lena.png"

///////////////////////////////////////////////

/*
template <typename T>
void printVector(const T* v, int siz) {
    cout << "arr(" << siz << ") = {";
    for(int ii=0; ii<siz; ii++) {
        cout << v[ii] << ", ";
    }
    cout << "}" << endl;
}
*/

///////////////////////////////////////////////
int main(int argc, char *argv[])
{
//    string dscCode="g1p4b1g1r1d";
//    DscCooMatrix dsc(dscCode);

//    cout << "code=" << dscCode << ", " << dsc.toString() << endl;
//    cout << numDscCooTypes << endl;

    /*
    cv::Mat img     = cv::imread(DEF_IMG_LENA, CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat img_128 = resizeImageProportional(img, 128);
    SDescriptorI dcg;
    buildCompoundDscGridFullImage(img_128, dcg, dst_135, dst_135_siz, 3, DSC_TYPE_1P, 3);
    SDescriptorP dcgp;
    dcgp.loadFromIntDsc(dcg);
    cout << dcgp << endl;
    double thresh   = dcgp.getThreshMinMax(0.1);
    dcgp.applyThreshFloat(thresh);
    cout << dcgp << endl;
    */


//    cv::Mat img = cv::imread("/home/ar/img/lena.jpg");
//    double* dscData = NULL;
//    int dscSize     = -1;
//    build_corr_dsc(img, 16, dscData, dscSize);
//    std::cout << dscToString(dscData, dscSize) << std::endl;

//    ulong64 hash;
//    ph_dct_imagehash("/media/bk0/craftkeys.com-all-128/00087b22808351302402b7f27d4cd0df2418.jpg", hash);
//    std::cout << "hash=" << std::hex << hash << std::endl;

	
//    QString imgPath = "G:/dev-git.git/dev.image-processing/mvptree_mod2.git/TestGUI_MVP_Search/img/add.png";
//    QString dscCode="g1p5b1g1r1d";
//    DscCooMatrix* dsc1   = new DscCooMatrix(dscCode.toStdString());
//    DscCooMatrix* dsc2   = new DscCooMatrix(dscCode.toStdString());
//    dsc1->buildDscFromPath(imgPath.toStdString());
//    std::cout << dsc1->toString() << std::endl;
//    QString pathDsc = "C:/dsc.bin";
//    std::ofstream ofs;
//	ofs.open(pathDsc.toStdString().c_str(), std::ofstream::binary);
//    dsc1->saveToStream(ofs);
//    ofs.close();
//    dsc1->clear();
//    delete dsc1;
//    std::ifstream ifs;
//	ifs.open(pathDsc.toStdString().c_str(), std::ifstream::binary);
//    std::cout << "\n" << dsc2->toString() < std::endl;
//    dsc2->loadFromStream(ifs);
//    ifs.close();
//    std::cout << dsc2->toString() << std::endl;
//    dsc2->clear();
//    delete dsc2;
	

	/*
    dscv2sl::SDescriptorP pd;
    std::ifstream ifs;
    ifs.open("C:/dsc.bin");
    pd.bload(ifs);
    ifs.close();
    std::cout << pd << std::endl;
	*/

	/*
    dscv2sl::SDescriptorI *d0 = new dscv2sl::SDescriptorI();
    dscv2sl::SDescriptorI *d1 = new dscv2sl::SDescriptorI();
    dscv2sl::SDescriptorP *p0 = new dscv2sl::SDescriptorP();
    dscv2sl::SDescriptorP *p1 = new dscv2sl::SDescriptorP();
    //
    QString imgPath = "G:/dev-git.git/dev.image-processing/mvptree_mod2.git/TestGUI_MVP_Search/img/add.png";
    QString pathDscI = "C:/dsc_i.bin";
    QString pathDscP = "C:/dsc_p.bin";
    cv::Mat img = cv::imread(imgPath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    if(img.data) {
        dscv2sl::recti roi(3,3,img.cols-2*3, img.rows-2*3);
        dscv2sl::buildDescriptorFromMat(img, roi, DSC_TYPE_1P, 6, *d0, DSC_DISTANCE_1p, true);
        std::ofstream ofs;
        ofs.open(pathDscI.toStdString().c_str());
        d0->bsave(ofs);
        ofs.close();
        std::ifstream ifs;
        ifs.open(pathDscI.toStdString().c_str());
        d1->bload(ifs);
        ifs.close();
        ifs.close();
        std::cout << "int-d0: "<< d0->toString() << std::endl;
        std::cout << "int-d1: "<< d1->toString() << std::endl;
        //
        p0->loadFromIntDsc(*d0);
        ofs.open(pathDscP.toStdString().c_str());
        p0->bsave(ofs);
        ofs.close();
        ifs.open(pathDscP.toStdString().c_str());
        p1->bload(ifs);
        ifs.close();
        std::cout << "float-p0: "<< p0->toString() << std::endl;
        std::cout << "float-p1: "<< p1->toString() << std::endl;
    } else {
        std::cerr << "Can't find image [" << imgPath.toStdString() << "]" << std::endl;
    }
	*/
//	int q;
//	std::cin>>q;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

///////////////////////////////////////////////

