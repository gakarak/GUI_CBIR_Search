#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>

#include <QStringList>
#include <QString>

#include "descriptorcalculator.h"

#define DEF_STEP_EMIT   20

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = 0);

    void setData();
    void markAsNeedToStop();
    bool isCompleteRun() {
        return completeRun;
    }
    int       stepEmit;
    DescriptorCalculator   calculator;
    bool      useMultiThreading;

signals:
    void signal_calcStep();
    
public slots:
    
private:
    bool      isNeedForceStop;
    bool      completeRun;

protected:
    void run();
};

#endif // WORKERTHREAD_H
