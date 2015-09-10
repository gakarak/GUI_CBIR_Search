/*
 * fsutil.h
 *
 *  Created on: 20.09.2012
 *      Author: ar
 */

#ifndef _LIB_HELPERS_H_
#define _LIB_HELPERS_H_


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <cstring>
#include <vector>

#include <cv.h>
#include <highgui.h>

#ifdef __linux__
    #define SEP_FILES   '/'
#elif defined _WIN32
    #define SEP_FILES   '\\'
#endif

///////////////////////////////////////////////////
class SimpleTimer {
 private:
    double     startTime;
    double     stopTime;
    double      duration;
 public:
    void start() {
        startTime   = 1000.* ((double)cv::getTickCount()) / cv::getTickFrequency();
    }
    double stop()  {
        stopTime    = 1000.* ((double)cv::getTickCount()) / cv::getTickFrequency();
        duration    = stopTime - startTime;
        return duration;
    }
    void printTime() {
        printf("%5.6fms\n", duration);
    }
    void stopAndPrint() {
        stop();
        printTime();
    }
    void stopAndPrint(const std::string& msg) {
        stop();
        printf("%s: %5.6fms\n", msg.c_str(),  duration);
    }
};

///////////////////////////////////////////////////
typedef std::vector<std::string> StringList;

///////////////////////////////////////////////////
// string operations
void split_str(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");
void strTrim(std::string& s, const char* delim = " \t");
bool strStartWith(const std::string& base, const std::string& str);
bool strEndWith(const std::string& base, const std::string& str);
// filesystem-operations
std::string getFileNameFromPath(const std::string& path);
std::string getDirNameFromPath(const std::string& path);
std::string getFileExt(const std::string& fileName);
void parsePath(const std::string& path, std::string& dirName, std::string& fileName, std::string& ext);
bool isFileHasExt(const std::string& fileName, const std::string& ext);
bool fileGood(const std::string& f);

//bool dirGood(const std::string& dir);
//bool makePath( std::string path );
//void listFilesInDir(const std::string& dirName, std::vector<std::string>& fileList, const std::string& ext = "", bool skipDir=true);
//void listDirInDir(const std::string& dirName, std::vector<std::string>& dirList, const std::string& startS="", const std::string& endS="",  bool skipHidden=true, bool retFullPath=true);

// file-operations
bool readStringListFromFile(const std::string& fileName, std::vector<std::string>& stringList, bool isSkipStr=true);

// OpenCV Helpers
cv::Mat resizeImageProportional(const cv::Mat &img, int maxSize);
void waitESC();

#endif /* FSUTIL_H_ */
