#ifndef TSDROPEDIT_H
#define TSDROPEDIT_H

#include <QLineEdit>

class TSDropEdit : public QLineEdit
{
    Q_OBJECT
public:
    TSDropEdit(QWidget* parent = nullptr);

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event) override;

    virtual void dragMoveEvent(QDragMoveEvent* event) override;

    virtual void dragLeaveEvent(QDragLeaveEvent* event) override;

    virtual void dropEvent(QDropEvent* event) override;

    QString getFileFromDropEvent(QDropEvent* event) const;

    bool isTSFile(const QString& fileName) const;
};

#endif // TSDROPEDIT_H
