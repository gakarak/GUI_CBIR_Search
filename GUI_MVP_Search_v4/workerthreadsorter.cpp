#include "workerthreadsorter.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

WorkerThreadSorter::WorkerThreadSorter(QObject *parent) :
    QThread(parent)
{
    stepEmit    = DEF_STEP_EMIT;
}

void WorkerThreadSorter::setData(std::vector<DscPluginInterface *> *dscList, DscPluginInterface *dscCmp, QVector<SortedIndex> *dst)
{
    this->dscListPtr    = dscList;
    this->dscCmpPtr     = dscCmp;
    this->dstPtr        = dst;
}

void WorkerThreadSorter::run()
{
#ifdef _OPENMP
    std::cout << "WorkerThreadSorter::openmp-run()" << std::endl;
#endif
    isNeedForceStop = false;
    completeRun = true;
    int num     = dscListPtr->size();
    if(useMultiThreading) {
#pragma omp parallel for
        for(int ii=0; ii<num; ii++) {
            if(isNeedForceStop)
                continue;
            (*dstPtr)[ii].idx   = ii;
            (*dstPtr)[ii].val   = dscCmpPtr->calcDscDistance(dscListPtr->at(ii));
//            if((ii%stepEmit)==0) {
//                emit signal_calcStep();
//            }
        }
    } else {
        for(int ii=0; ii<num; ii++) {
            if(isNeedForceStop)
                continue;
            (*dstPtr)[ii].idx   = ii;
            (*dstPtr)[ii].val  = dscCmpPtr->calcDscDistance(dscListPtr->at(ii));
//            if((ii%stepEmit)==0) {
//                emit signal_calcStep();
//            }
        }
    }
}
