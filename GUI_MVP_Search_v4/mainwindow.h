#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cv.h>
#include <highgui.h>

#include "mvptree_mod.h"
//#include "lib_dsc.h"
#include "lib_utils.h"
#include "model2.h"

#include <QMainWindow>

#include <QStringList>
#include <QFile>

#include <QAbstractItemModel>
#include <QSet>
#include <QVector>

#include <vector>

#include "dscplugininterface.h"
//#include "dscplugincoomatrix.h"

#include "sortedindex.h"
#include "workerthread.h"
#include "workerthreadsorter.h"

#define MVP_BRANCHFACTOR    2
#define MVP_PATHLENGTH      5
#define MVP_LEAFCAP         25
#define MVP_SPLIT_SIZE      1000
#define DSC_IMG_WSIZE       1

static int nbcalls  = 0;

/*
struct RequestExtDstCallDsc {
    std::vector<DscInterface* >*    dscIdx;
    DscInterface*                   dscSearchRequest;
};
*/

float point_distance(MVPDP *pointA, MVPDP *pointB, void* extDataPrt);

////////////////////////////////////////
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    QVector<DscPluginInterface* >   dscPlugins;
    QString                         lastPath;
    QStringList                     listImagesFullNames;
    QAbstractItemModel*             dataList;
    QSet<QString>                   setLoadedImageLists;
    QSet<QString>                   setLoadedDscLists;

    MVPTree*                            tree;
    std::vector<MVPDP* >                listMVPDsc;
    std::vector<DscPluginInterface*>    listDscIndex;

    QStringList             listDscNamesComboBox;
    QStringList             alchListFindedImagePath;
//    Model*                  alchModelImageList;


    QStringList             requestImageList;
//    QStringList             requestResultsImages;
//    QVector<float>          requestDistances;
    QVector<SortedIndex>    requestIndexData;
    WorkerThread*           workerThread;
    WorkerThreadSorter*     workerThreadSorter;

public:
    static void readStringsFromCSV(const QString& csvPath, QStringList& lstStrings);
    QString getDscFileNameFromImageList(const QString& fileNameImageList) const;

    QString getCurrentDscCode() const;
    QString getCurrentDscFilePrefix() const;
    DscPluginInterface *getCurrentDscBuilder() const;

private slots:
    void on_pushButtonSelectImageCSV_clicked();

    void on_pushButtonAppendImagesFromCSV_clicked();

    void slot_alchemyDropDirList(QList<QString>& dirList);
    void slot_alchemyFileListNotDropped();
    void slot_alchemyFileListDropped(QString& fileName);

    void slot_requestDropFileName(QString& fileName);
    void slot_appendImageDatabaseListFileName(QString& fileName);
    void slot_progressFromThread();


    void on_pushButton_3_clicked();

    void on_pushButtonSaveGenImageList_clicked();

    void on_pushButtonClearImageRequest_clicked();

    void on_listViewRequestImages_clicked(const QModelIndex &index);

    void on_pushButtonClearImageDB_clicked();

    void on_action_Quit_triggered();

    void on_action_Fullscreen_triggered();

    void on_pushButtonRequest_clicked();

    void on_pushButton_4_clicked();

    void on_spinBoxKNNMaxNum_valueChanged(int arg1);

    void on_doubleSpinBoxKNNDst_valueChanged(double arg1);

    void on_comboBoxDscTypes_currentIndexChanged(int index);

    void on_pushButtonBreackThread_clicked();

    void on_listWidgetRequestResults_clicked(const QModelIndex &index);

    void on_comboBoxDscSubTypes_currentIndexChanged(int index);

private:
    bool buildAndSaveDscListForListImages(const QString& fileNameImageList);
    void saveStringListToCSVFile(const QStringList& listStr, const QString& fileName);
    void loadDscFromBinFile(const QString& fileName/*, float** dscList, int &dscSize, int &dscNum*/);
    void clearTree();
    void loadDescriptorsToApplication();
    void refreshCodeSubTypesCombobox();
    void updateDscListsForLoadedImages();
private:
    Ui::MainWindow *ui;
    int threadedProgressCounter;
    void run_preThread();
    void run_postThread();
    void run_preThreadSorting();
    void run_postThreadSorting();
};

#endif // MAINWINDOW_H
