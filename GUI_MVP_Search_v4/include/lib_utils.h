#include <cstdio>


#ifdef unix
#include <sys/time.h>
#include <iostream>

class SimpleTimer {
 private:
    timeval     startTime;
    timeval     stopTime;
    double      duration;
 public:
    void start() {
        gettimeofday(&startTime, NULL);
    }
    double stop()  {
        gettimeofday(&stopTime, NULL);
        duration = (stopTime.tv_sec-startTime.tv_sec)*1000.0 + (stopTime.tv_usec-startTime.tv_usec)/1000.0;
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

#endif