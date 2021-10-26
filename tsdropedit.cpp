#include "tsdropedit.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>

TSDropEdit::TSDropEdit(QWidget* parent)
    :QLineEdit(parent)
{

}

void TSDropEdit::dragEnterEvent(QDragEnterEvent *event)
{
    QString fileName = getFileFromDropEvent(event);
    if (isTSFile(fileName))
        event->acceptProposedAction();
}

void TSDropEdit::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void TSDropEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void TSDropEdit::dropEvent(QDropEvent *event)
{
    QString fileName = getFileFromDropEvent(event);
    if (isTSFile(fileName))
    {
        setText(fileName);
        event->acceptProposedAction();
    }
}

QString TSDropEdit::getFileFromDropEvent(QDropEvent *event) const
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.empty())
            return urlList.at(0).toLocalFile();
    }

    return QString();
}

bool TSDropEdit::isTSFile(const QString &fileName) const
{
    return !fileName.isEmpty() && QFileInfo(fileName).suffix().compare("ts", Qt::CaseInsensitive) == 0;
}
