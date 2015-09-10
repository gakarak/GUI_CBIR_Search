#include "draglistview.h"

#include <QList>
#include <QUrl>

#include <QDebug>

DragListView::DragListView(QWidget *parent) :
    QListView(parent)
{
    setAcceptDrops(true);
    isGoodDragged   = false;
}

void DragListView::dragEnterEvent(QDragEnterEvent *ev)
{
    qDebug() << "::dragEnterEvent()";
    if(checkCorrectFileList(ev->mimeData())) {
        isGoodDragged   = true;
        ev->acceptProposedAction();
    }
}

void DragListView::dragLeaveEvent(QDragLeaveEvent *ev)
{
    Q_UNUSED(ev)
    qDebug() << "::dragLeaveEvent()";
}

void DragListView::dragMoveEvent(QDragMoveEvent *ev)
{
    Q_UNUSED(ev)
//    qDebug() << "::dragMoveEvent()";
}

void DragListView::dropEvent(QDropEvent *ev)
{
    Q_UNUSED(ev)
    qDebug() << "::dropEvent()";
    if(isGoodDragged) {
        emit signalDropFileName(fileNameList);
        fileNameList    = "";
        isGoodDragged   = false;
    }
}

bool DragListView::checkCorrectFileList(const QMimeData *mimeData)
{
    Q_UNUSED(mimeData)
    bool ret    = false;
    if(mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        QList<QString> fileNames;
        QUrl tmpUrl;
        QFileInfo fileInfo;
        foreach (tmpUrl, urls) {
            fileInfo.setFile(tmpUrl.toLocalFile());
            if(fileInfo.isFile()) {
                if(fileInfo.suffix().toLower()=="lst") {
                    fileNames.append(fileInfo.absoluteFilePath());
                }
            }
        }
        QString tmpStr;
        foreach (tmpStr, fileNames) {
            qDebug() << tmpStr;
        }
        if(fileNames.size()>0) {
            fileNameList    = fileNames.at(0);
            ret = true;
        }
    }
    return ret;
}



