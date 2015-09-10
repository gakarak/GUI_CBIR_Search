#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QLabel>
#include <QList>

class QMimeData;

class DragLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DragLabel(QWidget *parent = 0);
    
    void dragEnterEvent(QDragEnterEvent *ev);
    void dragMoveEvent(QDragMoveEvent *ev);
    void dragLeaveEvent(QDragLeaveEvent *ev);
    void dropEvent(QDropEvent *ev);

signals:
    void signalDirectorisDropped(QList<QString>& dirList);
    void signalFileNameListNotDropped();
    void signalFileNameListDropped(QString& fileName);

public slots:

private:
    bool checkDragDataIsDirectory(const QMimeData *mimeData);
    bool isGoodContent;
    QList<QString>  listDirPath;
    QString         fileNameLst;
    
};

#endif // DRAGLABEL_H
