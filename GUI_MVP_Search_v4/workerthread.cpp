#include "workerthread.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

WorkerThread::WorkerThread(QObject *parent) :
    QThread(parent)
{
    stepEmit    = DEF_STEP_EMIT;
}

void WorkerThread::run()
{
    isNeedForceStop = false;
    completeRun = true;
    // TODO: Prepare Descriptor Data
    calculator.prepareData();
    //
    if(useMultiThreading) {
#ifdef _OPENMP
    std::cout << "::openmp-run()" << std::endl;
#endif
#pragma omp parallel for
        for(int ii=0; ii<calculator.dscNum; ii++) {
            if(isNeedForceStop)
                continue;
            calculator.calcDsc(ii);
            if((ii%stepEmit)==0) {
                emit signal_calcStep();
            }
        }
    } else {
        for(int ii=0; ii<calculator.dscNum; ii++) {
            if(isNeedForceStop)
                continue;
            calculator.calcDsc(ii);
            if((ii%stepEmit)==0) {
                emit signal_calcStep();
            }
        }
    }
}


void WorkerThread::markAsNeedToStop()
{
    isNeedForceStop = true;
    completeRun     = false;
}
