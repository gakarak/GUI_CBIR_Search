#ifndef DRAGLISTVIEW_H
#define DRAGLISTVIEW_H

#include <QListView>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <QMimeData>
#include <QFileInfo>

class DragListView : public QListView
{
    Q_OBJECT
public:
    explicit DragListView(QWidget *parent = 0);
    

//    QString     fileNameLstImages;
//    QStringList listFileNamesOfImages;
    void dragEnterEvent(QDragEnterEvent *ev);
    void dragLeaveEvent(QDragLeaveEvent *ev);
    void dragMoveEvent(QDragMoveEvent *ev);
    void dropEvent(QDropEvent *ev);
signals:
    void signalDropFileName(QString& fileName);
public slots:
    
private:
    bool checkCorrectFileList(const QMimeData *mimeData);
    bool isGoodDragged;
    QString fileNameList;
};

#endif // DRAGLISTVIEW_H
