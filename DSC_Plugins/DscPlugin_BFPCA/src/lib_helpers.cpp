#include "lib_helpers.h"

//////////////////////////////////////////////////////////////////////////
// String API
void split_str(const std::string& str,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters) {
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void strTrim(std::string& s, const char* delim) {
    s.erase(0,s.find_first_not_of(delim));
    s.erase(s.find_last_not_of(delim) + 1, std::string::npos);
}

bool strStartWith(const std::string& base, const std::string& str) {
    return (base.find(str)==0);
}

bool strEndWith(const std::string& base, const std::string& str) {
    unsigned int idx	= base.rfind(str) + str.size();
    return (idx==base.size());
}

// File-system API
std::string getFileNameFromPath(const std::string& path) {
    char sep    = SEP_FILES;
    size_t idx  = path.rfind(sep, path.length());
    if(idx!=std::string::npos) {
        return path.substr(idx+1, path.length()-1);
    }
    return (path);
}

std::string getDirNameFromPath(const std::string& path) {
    char sep    = SEP_FILES;
    size_t idx  = path.rfind(sep, path.length());
    if(idx!=std::string::npos) {
        return path.substr(0, idx);
    }
    return ("");
}

std::string getFileExt(const std::string& fileName) {
    // small hack for path like: "./dir/file_without_ext"
    char sep	= SEP_FILES;
    size_t idxd	= fileName.rfind(sep, fileName.length());
    size_t idx	= fileName.rfind('.', fileName.length());
    if(idx!=std::string::npos) {
        if(idxd!=std::string::npos) {
            if(idx<idxd) {
                return ("");
            }
        }
        return fileName.substr(idx+1, fileName.length()-1);
    }
    return ("");
}

void parsePath(const std::string& path, std::string& dirName, std::string& fileName, std::string& ext) {
    dirName		= getDirNameFromPath(path);
    std::string tmpFileName	= getFileNameFromPath(path);
    size_t idx		= tmpFileName.rfind('.', tmpFileName.length()-1);
    if(idx!=std::string::npos) {
        ext	= tmpFileName.substr(idx+1, tmpFileName.length()-1);
    } else {
        ext	= "";
    }
    if(idx>0) {
        fileName	= tmpFileName.substr(0,idx);
    } else {
        fileName	= "";
    }
}

bool isFileHasExt(const std::string& fileName, const std::string& ext) {
    if((ext.size() + 1) >fileName.size()) {
        return false;
    } else {
        std::string rext = getFileExt(fileName);
        if(rext==ext) {
            return true;
        }
    }
    return false;
}

bool fileGood(const std::string& f) {
    std::ifstream ifs(f.c_str());
    return ifs.good();
}

// file-IO
bool readStringListFromFile(const std::string& fileName, std::vector<std::string>& stringList, bool isSkipStr) {
    bool ret	= false;
    stringList.clear();
    std::ifstream ifs;
    ifs.open(fileName.c_str());
    if(!ifs) {
//        std::cerr << "Can't read a file [" << fileName << "]" << std::endl;
    } else {
        std::string str;
        while(std::getline(ifs, str)) {
            strTrim(str);
            if(isSkipStr) {
                if(str.size()>0 && str[0]!='#') {
                    stringList.push_back(str);
                }
            } else {
                if(str.size()>0) {
                    stringList.push_back(str);
                }
            }
        }
        ret = true;
    }
    return ret;
}

///////////////////////////////////////////////////
cv::Mat resizeImageProportional(const cv::Mat &img, int maxSize) {
    cv::Size siz(-1,-1);
    if(img.cols>=img.rows) {
        siz.width   = maxSize;
        siz.height  = (maxSize*img.rows)/img.cols;
    } else {
        siz.width   = (maxSize*img.cols)/img.rows;
        siz.height  = maxSize;
    }
    cv::Mat ret;
    cv::resize(img, ret, siz, 0,0, CV_INTER_CUBIC);
    return ret;
}

void waitESC() {
    while (true) {
        char key    = cv::waitKey(0);
        if(key==27) {
            break;
        }
    }
}
