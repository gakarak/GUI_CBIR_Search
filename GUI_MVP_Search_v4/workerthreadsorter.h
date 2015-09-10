#ifndef WORKERTHREADSORTER_H
#define WORKERTHREADSORTER_H

#include <QThread>
#include <QVector>

#include "sortedindex.h"
#include "dscplugininterface.h"

#define DEF_STEP_EMIT   20

class WorkerThreadSorter : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThreadSorter(QObject *parent = 0);
    void markAsNeedToStop() {
        isNeedForceStop = true;
        completeRun     = false;
    }
    bool isCompleteRun() {
        return completeRun;
    }
    void setData(std::vector<DscPluginInterface*>* dscList, DscPluginInterface* dscCmp, QVector<SortedIndex>* dst);
    int     stepEmit;
    bool    useMultiThreading;
    int     getSize() {
        return dscListPtr->size();
    }

signals:
    void signal_calcStep();
    
private:
    bool      isNeedForceStop;
    bool      completeRun;
    std::vector<DscPluginInterface*>* dscListPtr;
    DscPluginInterface*               dscCmpPtr;
    QVector<SortedIndex>*       dstPtr;

protected:
    void run();
    
};

#endif // WORKERTHREADSORTER_H
