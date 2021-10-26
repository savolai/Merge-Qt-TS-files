#ifndef COMBINETRANSLATIONFILES_H
#define COMBINETRANSLATIONFILES_H

#include <QAbstractButton>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QTextBrowser>
#include <QXmlStreamReader>
namespace Ui {
class CombineTranslationFiles;
}

class CombineTranslationFiles : public QMainWindow {
    Q_OBJECT

public:
    explicit CombineTranslationFiles(QWidget* parent = 0);
    ~CombineTranslationFiles();

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);

    void on_targetFilePushButton_clicked();

    void on_sourceFilePushButton_clicked();

    void on_dockWidget_visibilityChanged(bool visible);

    void on_helpPushButton_clicked(bool checked);

    void on_sourceFilePushButton_2_clicked();

private:
    Ui::CombineTranslationFiles* ui;
    void readXML(const QString& sourcePath);
    QXmlStreamReader xmlR;
    QXmlStreamWriter xmlW;
    /**
     * @brief sourceStrings QMap of
     * 1. QString <context> name to Qmap of
     * 2. QString <message> <source> string to Qmap of
     * 3. QString of other <elements> to QString contents
     */
    QMap<QString, QMap<QString, QMap<QString, QString> > > sourceStrings;
    void readMessage(const QString& context);
    void readContext();
    void readTS();
    QString lastFileReadLang;
    int inputFileCount = 0;
    bool writeFile(const QString& targetPath);
    int writeItem(const QString& context, const QMap<QString, QMap<QString, QString> >& content);
};

#endif // COMBINETRANSLATIONFILES_H
