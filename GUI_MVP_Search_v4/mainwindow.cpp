#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include <QDebug>
#include <QTextStream>
#include <QFileDialog>

#include <QMessageBox>

#include "model2.h"
#include <fstream>

#include <cstdio>

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <QPluginLoader>

#include <QModelIndex>

#ifdef _OPENMP
    #include <omp.h>
#endif

////////////////////////////////////////////////////////////////////
float point_distance(MVPDP *pointA, MVPDP *pointB, void* extDataPrt) {
//    std::vector<DscInterface* >* dscIdx = (std::vector<DscInterface* >*) extDataPrt;
    if (!pointA || !pointB) {
        return -1.0f;
    } else if (pointA->datalen != pointB->datalen){
        fprintf(stdout," dataA %d, dataB %d\n", pointA->datalen, pointB->datalen);
        return -2.0f;
    }
    nbcalls++;
    if( (pointA->idx<0) || (pointB->idx<0)) {
//        std::cout << "pA: " << ((DscCooMatrix*)pointA->dscExtPtr)->dsc.toString() << std::endl;
//        std::cout << "pB: " << ((DscCooMatrix*)pointB->dscExtPtr)->dsc.toString() << std::endl;
    }
//    std::cout << "L1(pA,pB) = " << pointA->dscExtPtr->calcDscDistance(pointB->dscExtPtr) << std::endl;
//    std::cout.flush();
//    return dscIdx->at(pointA->idx)->calcDscDistance(dscIdx->at(pointB->idx));
    return (float)(pointA->dscExtPtr->calcDscDistance(pointB->dscExtPtr));
}

////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //
//    ui->labelSelectedImageRequest->setPixmap(QPixmap(":/img/img_eipi", Qt::KeepAspectRatio));
    ui->labelSelectedImageRequest->adjustSize();
    lastPath    = QDir::homePath();
    dataList   = NULL;
    ui->progressBar->setValue(0);
    setLoadedImageLists.clear();
    tree    = NULL;
    clearTree();
    //
    alchListFindedImagePath.clear();
    ui->listViewAlchemy->setViewMode(QListView::ListMode);
    ui->listViewAlchemy->setUniformItemSizes(true);
    ui->listViewAlchemy->setIconSize(QSize(256,256));
    ui->listViewAlchemy->viewport()->setAttribute(Qt::WA_StaticContents);
    //
    ui->listViewImageDB->setViewMode(QListView::ListMode);
    ui->listViewImageDB->setUniformItemSizes(true);
    ui->listViewImageDB->setIconSize(QSize(128,128));
    ui->listViewImageDB->viewport()->setAttribute(Qt::WA_StaticContents);
    //
    ui->listViewRequestImages->setViewMode(QListView::ListMode);
    ui->listViewRequestImages->setUniformItemSizes(true);
    ui->listViewRequestImages->setIconSize(QSize(128,128));
    ui->listViewRequestImages->viewport()->setAttribute(Qt::WA_StaticContents);
    //
    connect(ui->labelDraDirList, SIGNAL(signalDirectorisDropped(QList<QString>&)), this, SLOT(slot_alchemyDropDirList(QList<QString>&)));
    connect(ui->labelDraDirList, SIGNAL(signalFileNameListNotDropped()), this, SLOT(slot_alchemyFileListNotDropped()));
    connect(ui->labelDraDirList, SIGNAL(signalFileNameListDropped(QString&)), this, SLOT(slot_alchemyFileListDropped(QString&)));
    //
    connect(ui->listViewRequestImages, SIGNAL(signalDropFileName(QString&)), this, SLOT(slot_requestDropFileName(QString&)));
    //
    connect(ui->listViewImageDB, SIGNAL(signalDropFileName(QString&)), this, SLOT(slot_appendImageDatabaseListFileName(QString&)));
    //
    workerThread    = new WorkerThread();
    workerThread->useMultiThreading = false;
#ifdef _OPENMP
    workerThread->useMultiThreading = ui->checkBoxMultiThreading->isChecked();
#else
    ui->checkBoxMultiThreading->setEnabled(false);
    ui->checkBoxMultiThreading->setChecked(false);
    ui->checkBoxMultiThreading->setToolTip(tr("MultiThreading is not allowed on your systems..."));
#endif
    connect(workerThread, SIGNAL(signal_calcStep()), this, SLOT(slot_progressFromThread()));
    //
    workerThreadSorter  = new WorkerThreadSorter();
    workerThreadSorter->useMultiThreading = false;
#ifdef _OPENMP
    workerThreadSorter->useMultiThreading = ui->checkBoxMultiThreading->isChecked();
#endif
//    connect(workerThreadSorter, SIGNAL(signal_calcStep()), this, SLOT(slot_progressFromThread()));
    //
    /*
    listDscNamesComboBox.clear();
    for(int ii=0; ii<numDscCooTypes; ii++) {
        listDscNamesComboBox.append(dscCooTypesCodes[ii]);
    }
    ui->comboBoxDscTypes->addItems(listDscNamesComboBox);
    */
    loadDescriptorsToApplication();
//    refreshCodeSubTypesCombobox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

template <typename T>
void printArray(const T* arr, int siz) {
    std::cout << "array(" << siz << ") = {";
    for(int ii=0; ii<siz; ii++) {
        std::cout << arr[ii] << ", ";
    }
    std::cout << "}" << std::endl;
}

bool MainWindow::buildAndSaveDscListForListImages(const QString &fileNameImageList)
{
    QFileInfo fileInfo(fileNameImageList);
    if(fileInfo.exists()) {
        qDebug() << fileInfo.completeBaseName();
        //
        // TODO: add DSC Preparation code!!!

        //
        QString dscFileName = getDscFileNameFromImageList(fileNameImageList);
//        QString dscFileName = fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".dsc";
        if(!QFile::exists(dscFileName)) {
            qDebug() << "File (" << dscFileName << ") not found!";
            //
            QStringList tmpLst;
            readStringsFromCSV(fileNameImageList, tmpLst);
            int numDsc              = tmpLst.size();
            QString dscStrCode      = getCurrentDscCode();
            DscPluginInterface** dscList  = new DscPluginInterface*[numDsc];
            workerThread->calculator.setData(fileNameImageList, &tmpLst, dscList, getCurrentDscBuilder(), dscStrCode);
            run_preThread();
#ifdef _OPENMP
            workerThread->useMultiThreading = ui->checkBoxMultiThreading->isChecked();
#endif
            workerThread->start();
            std::cout << "wait Thread calculation..." << std::endl;
            QTime time;
            time.start();
            while(true) {
                bool ret    = workerThread->wait(40);
                if(ret) {
                    break;
                }
                QApplication::processEvents();
//                std::cout << "*** wait ***" << std::endl;
            }
            std::cout << "Average time: " << time.elapsed() << std::endl;
            run_postThread();
            ui->labelStatusInfo->setText("...");
            QApplication::processEvents();
            //
            if(workerThread->isCompleteRun()) {
                std::ofstream ofs;
                ofs.open(dscFileName.toStdString().c_str(), std::ofstream::binary);
                if(ofs) {
                    ui->labelStatusInfo->setText("save dsc-index...");
                    ofs.write((char*)&numDsc, sizeof(int));
                    for(int ii=0; ii<numDsc; ii++) {
                        dscList[ii]->saveToStream(ofs);
                        ui->progressBar->setValue(ii);
                        QApplication::processEvents();
                    }
                    ui->labelStatusInfo->setText("...");
                    ofs.close();
                }
                for(int ii=0; ii<numDsc; ii++) {
                    delete dscList[ii];
                }
                delete [] dscList;
                std::cout << "[FINISH] ::buildAndSaveDscListForListImages()" << std::endl;
            } else {
                QMessageBox::information(this, "WARNING", "Calculations were interrupted.");
                return false;
            }
            return true;
        } else {
            getCurrentDscBuilder()->prepareDscData(fileNameImageList, getCurrentDscCode().toStdString());
            return true;
        }
    }
}

void MainWindow::saveStringListToCSVFile(const QStringList &listStr, const QString &fileName)
{
    QFile fout(fileName);
    if(fout.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&fout);
        QString tmp;
        foreach (tmp, listStr) {
            out << tmp << "\n";
        }
        fout.close();
    } else {
        QMessageBox::critical(this, "ERROR", QString("Can't write to file (%1)").arg(fileName));
    }
}

void MainWindow::loadDscFromBinFile(const QString &fileName)
{
    int dscNum;
    if(QFile::exists(fileName)) {
        if(!setLoadedDscLists.contains(fileName)) {
            std::ifstream ifs;
            ifs.open(fileName.toStdString().c_str(), std::ifstream::binary);
            if(ifs) {
                DscPluginInterface* dscBuilder  = getCurrentDscBuilder();
                ifs.read((char*)&dscNum,  sizeof(int));
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(dscNum);
                ui->progressBar->setValue(0);
                ui->labelListStatus->setText("read dsc-index...");
                for(int ii=0; ii<dscNum; ii++) {
                    DscPluginInterface* tmpDsc   = dscBuilder->createObject();
                    tmpDsc->loadFromStream(ifs);
//                    std::cout << tmpDsc->getShortInfo() << std::endl;
                    listDscIndex.push_back(tmpDsc);
                    ui->progressBar->setValue(ii);
                    QApplication::processEvents();
                }
                ifs.close();
                //
                ui->labelListStatus->setText("convert dsc...");
                ui->progressBar->setValue(0);
                MVPDP** points  = (MVPDP**)malloc(dscNum*sizeof(MVPDP*));
                int cnt_idx = listMVPDsc.size();
                for(int ii=0; ii<dscNum; ii++) {
                    MVPDP* newpnt       = dp_alloc(MVP_UINT64ARRAY);
                    newpnt->datalen     = 0;
                    newpnt->data        = NULL;
                    newpnt->id          = NULL;
                    newpnt->idx         = cnt_idx+ii;
                    newpnt->dscExtPtr   = listDscIndex.at(ii + cnt_idx);
//                    cnt_idx++;
                    points[ii]  = newpnt;
                    listMVPDsc.push_back(newpnt);
                }
                //
                int numParts        = /*floor*/(dscNum/MVP_SPLIT_SIZE) + 1;
                int cnt_for_test    = 0;
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(numParts);
                ui->progressBar->setValue(0);
                ui->labelListStatus->setText("append dsc to tree...");
                QApplication::processEvents();
                MVPError err;
                bool isNoError  = true;
                for(int pp=0; pp<numParts; pp++) {
                    std::cout << pp << " : iter" << std::endl;
                    int delta   = MVP_SPLIT_SIZE;
                    int idxFrom = pp*MVP_SPLIT_SIZE;
                    int idxTo   = idxFrom + MVP_SPLIT_SIZE;
                    if(idxFrom>=dscNum) {
                        break;
                    }
                    if(idxTo>=dscNum) {
                        delta   = dscNum - idxFrom;
                    }
                    //
                    MVPDP** tmpPoints    = (MVPDP**)malloc(delta*sizeof(MVPDP*));
                    for(int kk=0; kk<delta; kk++) {
                        tmpPoints[kk]   = points[idxFrom + kk];
                        cnt_for_test++;
                    }
                    err = mvptree_add(tree, tmpPoints, delta);
                    if(err!=MVP_SUCCESS) {
                        ui->tabWidget->setCurrentIndex(2);
                        ui->plainTextEdit->appendPlainText(QString("***ERROR*** iter #%1/%2 mvptree_add(%3), code=%4/%5 [%6]").arg(pp).arg(numParts).arg(delta).arg(err).arg(MVP_SUCCESS).arg(MVPError_ShortNames[err]));
                        std::cerr << "***ERROR*** mvptree_add(" << delta << "), code=" << err << "/" << MVP_SUCCESS << std::endl;
                        isNoError   = false;
                    }
                    free(tmpPoints);
                    ui->progressBar->setValue(pp);
                    QApplication::processEvents();
                }
                if(!isNoError) {
                    ui->plainTextEdit->appendPlainText(tr("Search-Tree may be incomplete/inconsistent!"));
                } else {
                    ui->plainTextEdit->appendPlainText(tr("File [%1] sucsessfull loaded in DB").arg(fileName));
                }
                ui->progressBar->setValue(numParts);
                std::cout << "[DEBUG] : dscNum/cnt-tree-add = " << dscNum << "/" << cnt_for_test << std::endl;
                ui->labelListStatus->setText(QString("db-size: #%1/%2").arg(listMVPDsc.size()).arg(listImagesFullNames.size()));
                free(points);
                setLoadedDscLists.insert(fileName);
            }
        } else {
            std::cerr << "***WARNING*** dsc-file (" << fileName.toStdString() << ") already loaded!" << std::endl;
        }
    } else {
        QMessageBox::information(this, tr("Warning"), QString("Can't open file (%1)").arg(fileName));
    }
}

void MainWindow::clearTree()
{
    if(tree!=NULL) {
        mvptree_clear(tree, free);
        free(tree);
    }
    CmpFunc distance_func = point_distance;
    tree                = mvptree_alloc(NULL, distance_func,MVP_BRANCHFACTOR, MVP_PATHLENGTH, MVP_LEAFCAP);
    tree->extDataPtr    = &listDscIndex;
}

void MainWindow::loadDescriptorsToApplication()
{
    if(dscPlugins.size()>0) {
        for(int ii=0; ii<dscPlugins.size(); ii++) {
            DscPluginInterface* tmp = dscPlugins.at(ii);
            delete tmp;
            tmp = NULL;
        }
        dscPlugins.clear();
    }
    // TODO: in feature need dynamic loading in FOR-loop
    /*
    DscPluginCooMatrix* tmp = new DscPluginCooMatrix();
    tmp->init("");
    dscPlugins.append(tmp);

    */
    QStringList dscPliginNames;
    QDir dir(QDir(QApplication::applicationDirPath()).absoluteFilePath(DEF_PLUGIN_DIR));
    foreach (QString strFileName, dir.entryList(QDir::Files)) {
        QString pluginFileName  = dir.absoluteFilePath(strFileName);
        QPluginLoader loader(pluginFileName);
        QObject* tmpObj = qobject_cast<QObject*>(loader.instance());
        if(tmpObj!=NULL) {
            DscPluginInterface* tmpIFace = qobject_cast<DscPluginInterface*>(tmpObj);
            if(tmpIFace!=NULL) {
                tmpIFace->init(pluginFileName);
                dscPlugins.append(tmpIFace);
                dscPliginNames.append(tmpIFace->getCodeType());
            }
        }
    }
    if(dscPlugins.size()<1) {
        QMessageBox::information(this, tr("WARNING!"), tr("DSC-plugins not found!"));
    }
    ui->comboBoxDscTypes->addItems(dscPliginNames);
    refreshCodeSubTypesCombobox();
}

void MainWindow::refreshCodeSubTypesCombobox()
{
    if(dscPlugins.size()>0) {
        int idx = ui->comboBoxDscTypes->currentIndex();
        if(idx<dscPlugins.size()) {
            ui->comboBoxDscSubTypes->clear();
            QStringList tmp = dscPlugins.at(idx)->getCodeSubTypes();
            qDebug() << "CodeSubtype: " << tmp;
            ui->comboBoxDscSubTypes->addItems(dscPlugins.at(idx)->getCodeSubTypes());
        }
    }
}

void MainWindow::updateDscListsForLoadedImages()
{
    //TODO: updateDscListsForLoadedImages()
    QSet<QString> tmpSetLoadedImageLists  = setLoadedImageLists;
    on_pushButtonClearImageDB_clicked();
    foreach (QString tmp, tmpSetLoadedImageLists) {
        slot_appendImageDatabaseListFileName(tmp);
    }
}

void MainWindow::readStringsFromCSV(const QString& csvPath, QStringList& lstStrings) {
    lstStrings.clear();
    QFile file(csvPath);
    if(!file.exists()) {
        qDebug() << "Can't find file [" << csvPath << "]";
        return;
    }
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream txtStream(&file);
        QString line;
        while (!txtStream.atEnd()) {
            line    = txtStream.readLine();
            line    = line.trimmed();
            if (line.size()==0 || line.at(0)=='#') {
                // skip
            } else {
                lstStrings.append(line);
            }
        }
        file.close();
    }
}

QString MainWindow::getDscFileNameFromImageList(const QString &fileNameImageList) const
{
    QFileInfo fileInfo(fileNameImageList);
    return (fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".dsc_" + getCurrentDscFilePrefix());
}

QString MainWindow::getCurrentDscCode() const
{
    return ui->comboBoxDscSubTypes->currentText();
}

QString MainWindow::getCurrentDscFilePrefix() const
{
    return QString("%1_%2").arg(getCurrentDscBuilder()->getCodeType()).arg(getCurrentDscCode());
}

DscPluginInterface *MainWindow::getCurrentDscBuilder() const
{
    int numDsc  = dscPlugins.size();
    int indx    = ui->comboBoxDscTypes->currentIndex();
    if(numDsc>0 && (indx<numDsc)) {
        return dscPlugins.at(indx);
    } else {
        return NULL;
    }
}

void MainWindow::on_pushButtonSelectImageCSV_clicked()
{
    QString selectedCSVFileName   = QFileDialog::getOpenFileName(
                    this,
                    tr("Load CSV ImageList"),
                    lastPath,
                    tr("RCPD directory (*.LST)"));
        QFileInfo fileInfo(selectedCSVFileName);
        if(!fileInfo.exists()) {
            return;
        }
        QString selectedDirectory = fileInfo.dir().absolutePath();
        qDebug() << selectedCSVFileName;
        if(!selectedDirectory.isEmpty()) {
            ui->lineEditPathToImagesCSV->setText(selectedCSVFileName);
            lastPath = selectedDirectory;
            buildAndSaveDscListForListImages(selectedCSVFileName);
        }
}

void MainWindow::on_pushButtonAppendImagesFromCSV_clicked()
{
    QString fileNameImageListCSV = ui->lineEditPathToImagesCSV->text();
    if(!setLoadedImageLists.contains(fileNameImageListCSV)) {
        if(QFile::exists(fileNameImageListCSV)) {
            //
            QStringList tmpList;
            readStringsFromCSV(fileNameImageListCSV, tmpList);
            listImagesFullNames.append(tmpList);
            setLoadedImageLists.insert(fileNameImageListCSV);
            ui->labelListStatus->setText(QString("db-size #%1").arg(listImagesFullNames.size()));
            //
            if(dataList!=NULL) {
                delete dataList;
            }
            dataList    = new Model(&listImagesFullNames, this);
            ui->listViewImageDB->setModel(dataList);
            ui->listViewImageDB->update();
            //
            QString fileNameDscList = getDscFileNameFromImageList(fileNameImageListCSV);
            loadDscFromBinFile(fileNameDscList);
        } else {
            QMessageBox::information(this, tr("Warning"), tr("Image list is not loaded"));
        }
    } else {
        QMessageBox::information(this, tr("Warning"), tr("Image list already loaded..."));
    }
}

void MainWindow::slot_alchemyDropDirList(QList<QString> &dirList)
{
    QString dir;
    ui->plainTextEdit->appendPlainText("[DROP] dir list:");
    foreach (dir, dirList) {
        ui->plainTextEdit->appendPlainText(QString("\t%1").arg(dir));
    }
    ui->tabWidget->setEnabled(false);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
    ui->labelStatusInfo->setText(tr("find images..."));
    QApplication::processEvents();
    //
//    alchListFindedImagePath.clear();
    int cnt = 0;
    QFileInfo fileInfo;
    QString sfx;
    foreach (dir, dirList) {
        QDirIterator::IteratorFlags flags   = QDirIterator::NoIteratorFlags;
        if(ui->checkBoxAlchemyRecSearch->isChecked()) {
            flags   = QDirIterator::Subdirectories;
            qDebug() << "Use subdirectories";
        }
        QDirIterator dirIt(dir, flags);
        while (dirIt.hasNext()) {
            fileInfo.setFile(dirIt.next());
            if(fileInfo.isFile()) {
                sfx = fileInfo.suffix().toLower();
                if(sfx=="jpg" || sfx=="png") {
                    alchListFindedImagePath.append(fileInfo.absoluteFilePath());
//                    qDebug() << fileInfo.absoluteFilePath();
                    cnt++;
                    if(cnt%100==0) {
                        ui->labelAlchemyStatus->setText(QString("#images: %1").arg(cnt));
                        QApplication::processEvents();
                    }
                }
            }
        }
    }
    //
    if(alchListFindedImagePath.size()>0) {
        ui->pushButtonSaveGenImageList->setEnabled(true);
        ui->lineEditGenImageList->setReadOnly(false);
    }
    //
    ui->labelAlchemyStatus->setText(QString("finded %1 images").arg(alchListFindedImagePath.size()));
//    alchModelImageList->refreshListOfImages();
    Model* tmpModel   = new Model(&alchListFindedImagePath, this);
    ui->listViewAlchemy->setModel(tmpModel);
    ui->listViewAlchemy->update();
    //
    ui->tabWidget->setEnabled(true);
    ui->progressBar->setMaximum(1);
    ui->labelStatusInfo->setText(tr("..."));
}

void MainWindow::slot_alchemyFileListNotDropped()
{
    ui->lineEditGenImageList->setText("");
}

void MainWindow::slot_alchemyFileListDropped(QString &fileName)
{
    ui->lineEditGenImageList->setText(fileName);
}

void MainWindow::slot_requestDropFileName(QString &fileName)
{
    on_pushButtonClearImageRequest_clicked();
    readStringsFromCSV(fileName, requestImageList);
    ui->listViewRequestImages->setModel(new Model(&requestImageList, this));
    ui->listViewRequestImages->update();
}

void MainWindow::slot_appendImageDatabaseListFileName(QString &fileName)
{
    qDebug() << fileName;
    ui->tabWidget->setEnabled(false);
    QApplication::processEvents();
    //
    ui->lineEditPathToImagesCSV->setText(fileName);
    if(buildAndSaveDscListForListImages(fileName)) {
        on_pushButtonAppendImagesFromCSV_clicked();
    }
    //
    ui->tabWidget->setEnabled(true);
}

void MainWindow::slot_progressFromThread()
{
    threadedProgressCounter++;
    ui->progressBar->setValue(threadedProgressCounter);
//    QApplication::processEvents();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->labelAlchemyStatus->setText(tr("..."));
    ui->lineEditGenImageList->setText(tr(""));
    ui->lineEditGenImageList->setReadOnly(true);
    ui->pushButtonSaveGenImageList->setEnabled(false);
    alchListFindedImagePath.clear();
    ui->listViewAlchemy->setModel(new Model(&alchListFindedImagePath, this));
    ui->listViewAlchemy->update();
}

void MainWindow::on_pushButtonSaveGenImageList_clicked()
{
    QString fileName    = ui->lineEditGenImageList->text();
    if(!fileName.isEmpty() && (alchListFindedImagePath.size()>0) ) {
        ui->tabWidget->setEnabled(false);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(0);
        QApplication::processEvents();
        saveStringListToCSVFile(alchListFindedImagePath, fileName);
        QApplication::processEvents();
        ui->progressBar->setMaximum(1);
        ui->tabWidget->setEnabled(true);
    } else {
        qDebug() << "**ERROR** save-image-list";
    }
}

void MainWindow::on_pushButtonClearImageRequest_clicked()
{
    requestImageList.clear();
    requestIndexData.clear();
    ui->listWidgetRequestResults->clear();
    ui->labelRequestStatus->setText("...");
    ui->listViewRequestImages->setModel(new Model(&requestImageList, this));
    ui->listViewRequestImages->update();
    QApplication::processEvents();
    ui->labelSelectedImageRequest->setPixmap(QPixmap(":/img/img_eipi").scaled(ui->labelSelectedImageRequest->width(), ui->labelSelectedImageRequest->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_listViewRequestImages_clicked(const QModelIndex &index)
{
    qDebug() << "::on_listViewRequestImages_clicked(): row=" << index.row() << ", col=" << index.column();
    ui->labelSelectedImageRequest->setPixmap(QPixmap(requestImageList.at(index.row())).scaled(ui->labelSelectedImageRequest->width(), ui->labelSelectedImageRequest->height(), Qt::KeepAspectRatio));
    on_pushButtonRequest_clicked();
}

void MainWindow::on_pushButtonClearImageDB_clicked()
{
    ui->tabWidget->setEnabled(false);
    QApplication::processEvents();
    listImagesFullNames.clear();
    setLoadedDscLists.clear();
    setLoadedImageLists.clear();
    clearTree();
    /*
    for(int ii=0; ii<listMVPDsc.size(); ii++) {
        MVPDP* tmpDP    = listMVPDsc.at(ii);
        dp_free(tmpDP, NULL);
    }
    */
    listMVPDsc.clear();
    DscPluginInterface* tmpPtr;
    for(int ii=0; ii<listDscIndex.size(); ii++) {
        tmpPtr  = listDscIndex.at(ii);
        tmpPtr->clear();
//        delete tmpPtr;
    }
    listDscIndex.clear();
    ui->listViewImageDB->setModel(new Model(&listImagesFullNames, this));
    ui->listViewImageDB->update();
    ui->tabWidget->setEnabled(true);
    ui->labelListStatus->setText(tr("..."));
}


void MainWindow::on_action_Quit_triggered()
{
    qApp->quit();
}

void MainWindow::on_action_Fullscreen_triggered()
{
    if(!isFullScreen()) {
        showFullScreen();
    } else {
        showNormal();
    }
}

void MainWindow::on_pushButtonRequest_clicked()
{
    if((listImagesFullNames.size()>0) && (requestImageList.size()>0)) {
        int idxRequest  = ui->listViewRequestImages->currentIndex().row();
        qDebug() << "idx = " << idxRequest << "/" << requestImageList.size();
        if( (idxRequest>=0) && (idxRequest<requestImageList.size())) {
            cv::Mat reqMat  = cv::imread(requestImageList.at(idxRequest).toStdString());
            if(reqMat.data) {
                DscPluginInterface*   tmpDsc  = getCurrentDscBuilder()->createObject(getCurrentDscCode().toStdString());
                tmpDsc->buildDscFromPath(requestImageList.at(idxRequest).toStdString());
                MVPDP** results = NULL;
                if(!ui->checkBoxUseRealKNN->isChecked()) {
                    MVPDP* dpDsc        = dp_alloc(MVP_UINT64ARRAY);
                    dpDsc->dscExtPtr    = tmpDsc;
                    unsigned int    nbresults   = 0;
                    MVPError    err;
                    /*MVPDP** */results = mvptree_retrieve(tree,
                                                       dpDsc,
                                                       ui->spinBoxKNNMaxNum->value(),
                                                       ui->doubleSpinBoxKNNDst->value(),
                                                       &nbresults, &err);
                    //
                    ui->labelRequestStatus->setText(QString("#%1").arg(nbresults));
                    requestIndexData.clear();
                    for(uint ii=0; ii<nbresults; ii++) {
                        int cIdx    = results[ii]->idx;
                        qDebug() << ii << " : " << cIdx;
                        requestIndexData.append(SortedIndex(tree->dist(results[ii], dpDsc, tree->extDataPtr), cIdx, listImagesFullNames.at(cIdx)));
                    }
                    qSort(requestIndexData);
                } else {
                    QVector<SortedIndex> tmpSortedDst;
                    tmpSortedDst.resize(listDscIndex.size());
                    workerThreadSorter->setData(&listDscIndex, tmpDsc, &tmpSortedDst);
                    //
                    run_preThreadSorting();
                    QApplication::processEvents();
                    workerThreadSorter->start();
                    //
                    QTime time;
                    time.start();
                    while(true) {
                        bool ret    = workerThreadSorter->wait(10);
                        if(ret) {
                            break;
                        }
                        QApplication::processEvents();
                    }
                    std::cout << "Average time: " << time.elapsed() << std::endl;
                    //
                    run_postThreadSorting();
                    ui->labelStatusInfo->setText("...");
                    QApplication::processEvents();
                    //
                    qSort(tmpSortedDst);
                    //
                    qDebug() << ui->spinBoxKNNMaxNum->value();
                    requestIndexData.clear();
                    int num = ui->spinBoxKNNMaxNum->value();
                    if(tmpSortedDst.size()<num) {
                        num = tmpSortedDst.size();
                    }
                    for(int ii=0; ii<num; ii++) {
                        const SortedIndex& si   = tmpSortedDst.at(ii);
                        requestIndexData.append(SortedIndex(si.val, si.idx, listImagesFullNames.at(si.idx)));
                    }
                }
                //
                ui->listWidgetRequestResults->clear();
                for(int ii=0; ii<requestIndexData.size(); ii++) {
                    ui->listWidgetRequestResults->addItem(
                                new QListWidgetItem(
                                    QIcon(requestIndexData.at(ii).str),
                                    QString("dst: %1").arg(requestIndexData.at(ii).val),
                                    ui->listWidgetRequestResults));
                }
//                dp_free(dpDsc, free);
                delete tmpDsc;
                if(results) free(results);

            } else {
                qDebug() << "FUCK #2 !!!";
            }
        } else {
            qDebug() << "FUCK!!!";
        }
    } else {
        qDebug() << "no-data to search";
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    for(int ii=0; ii<listMVPDsc.size(); ii++) {
        MVPDP* tmp  = listMVPDsc.at(ii);
        std::cout << ii << " : " << tmp->idx << ", " << std::endl;
    }
    std::cout << "----------" << std::cout;
    for(int ii=0; ii<listImagesFullNames.size(); ii++) {
        std::cout << ii << " : " << listImagesFullNames.at(ii).toStdString() << std::endl;
    }
}

void MainWindow::on_spinBoxKNNMaxNum_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    on_pushButtonRequest_clicked();
}

void MainWindow::on_doubleSpinBoxKNNDst_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    on_pushButtonRequest_clicked();
}

//
void MainWindow::run_preThread() {
    ui->pushButtonBreackThread->setEnabled(true);
    ui->tabWidget->setEnabled(false);
    threadedProgressCounter = 0;
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(workerThread->calculator.dscNum/workerThread->stepEmit);
    ui->progressBar->setValue(0);
#ifdef _OPENMP
    ui->checkBoxMultiThreading->setEnabled(false);
#endif
}

void MainWindow::run_postThread() {
    ui->pushButtonBreackThread->setEnabled(false);
    ui->tabWidget->setEnabled(true);
    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(0);
#ifdef _OPENMP
    ui->checkBoxMultiThreading->setEnabled(true);
#endif
}

void MainWindow::run_preThreadSorting() {
    ui->pushButtonBreackThread->setEnabled(true);
    ui->tabWidget->setEnabled(false);
    threadedProgressCounter = 0;
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(workerThreadSorter->getSize()/workerThreadSorter->stepEmit);
    ui->progressBar->setValue(0);
#ifdef _OPENMP
    ui->checkBoxMultiThreading->setEnabled(false);
#endif
}

void MainWindow::run_postThreadSorting() {
    ui->pushButtonBreackThread->setEnabled(false);
    ui->tabWidget->setEnabled(true);
    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(0);
#ifdef _OPENMP
    ui->checkBoxMultiThreading->setEnabled(true);
#endif
}

void MainWindow::on_comboBoxDscTypes_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    refreshCodeSubTypesCombobox();
//    on_pushButtonClearImageDB_clicked();
//    QApplication::processEvents();
    updateDscListsForLoadedImages();
}

void MainWindow::on_comboBoxDscSubTypes_currentIndexChanged(int index)
{
    Q_UNUSED(index)
//    on_pushButtonClearImageDB_clicked();
    // FIXME: This is kostyl!!!!
    if(index>0) {
        updateDscListsForLoadedImages();
    }
}

void MainWindow::on_pushButtonBreackThread_clicked()
{
    if(workerThread->isRunning()) {
        workerThread->markAsNeedToStop();
    }
    if(workerThreadSorter->isRunning()) {
        workerThreadSorter->markAsNeedToStop();
    }
}

void MainWindow::on_listWidgetRequestResults_clicked(const QModelIndex &index)
{
//    qDebug() << requestIndexData.at(index.row()).toQString();
//    ui->listViewImageDB->scrollTo(ui->listViewImageDB->sib);
    ui->listViewImageDB->setCurrentIndex(ui->listViewImageDB->model()->index(requestIndexData.at(index.row()).idx,0));
}


