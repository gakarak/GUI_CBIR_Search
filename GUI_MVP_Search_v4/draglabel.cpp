#include "draglabel.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <QMimeData>
#include <QDir>
#include <QFileInfo>

#include <QList>
#include <QUrl>

DragLabel::DragLabel(QWidget *parent) :
    QLabel(parent)
{
    setAcceptDrops(true);
    isGoodContent   = false;
}

void DragLabel::dragEnterEvent(QDragEnterEvent *ev)
{
    qDebug() << "::dragEnterEvent()";
    if(checkDragDataIsDirectory(ev->mimeData())) {
        ev->acceptProposedAction();
        setPixmap(QPixmap(QString(":/img/dnd_accept")));
        isGoodContent   = true;
        emit signalFileNameListDropped(fileNameLst);
    } else {
        ev->acceptProposedAction();
        setPixmap(QPixmap(QString(":/img/dnd_disable")));
        isGoodContent   = false;
    }
}

void DragLabel::dragMoveEvent(QDragMoveEvent *ev)
{
    Q_UNUSED(ev)
//    qDebug() << "::dragMoveEvent()";
}

void DragLabel::dragLeaveEvent(QDragLeaveEvent *ev)
{
    Q_UNUSED(ev)
    qDebug() << "::dragLeaveEvent()";
    isGoodContent   = false;
    fileNameLst     = "";
    emit signalFileNameListNotDropped();
    setPixmap(QPixmap(":/img/dnd_wait"));
}

void DragLabel::dropEvent(QDropEvent *ev)
{
    Q_UNUSED(ev)
    qDebug() << "::dropEvent()";
    if(isGoodContent) {
        emit signalDirectorisDropped(listDirPath);
    }
    setPixmap(QPixmap(":/img/dnd_wait"));
}

bool DragLabel::checkDragDataIsDirectory(const QMimeData* mimeData)
{
    QFileInfo fileInfo;
    bool ret    = false;
    if(mimeData->hasUrls()) {
        QList<QUrl> urls    = mimeData->urls();
        listDirPath.clear();
        QUrl tmp;
        foreach (tmp, urls) {
            if(tmp.isLocalFile()) {
                fileInfo.setFile(tmp.toLocalFile());
                if(fileInfo.isDir()) {
                    listDirPath.append(fileInfo.absoluteFilePath());
                    ret = true;
                }
            }
        }
        if(listDirPath.size()>0) {
            fileNameLst = QString("%1.lst").arg(listDirPath.at(0));
        } else {
            fileNameLst = "";
        }
    }
    return ret;
}
