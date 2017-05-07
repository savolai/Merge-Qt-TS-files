#include "combinetranslationfiles.h"
#include "ui_combinetranslationfiles.h"
#include <QDialogButtonBox>
#include <QLabel>
#include <QTextBrowser>
CombineTranslationFiles::CombineTranslationFiles(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::CombineTranslationFiles)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText("Save translations to file");
    ui->buttonBox->button(QDialogButtonBox::Apply)->setStyleSheet("font-weight:bold;");
    ui->dockWidget->hide();
}

CombineTranslationFiles::~CombineTranslationFiles()
{
    delete ui;
}

void CombineTranslationFiles::on_buttonBox_clicked(QAbstractButton* button)
{
    if ((QPushButton*)button == ui->buttonBox->button(QDialogButtonBox::Open)) {
        readXML(ui->sourceFile->text());

    } else if ((QPushButton*)button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
        writeFile(ui->targetFile->text());
    } else if ((QPushButton*)button == ui->buttonBox->button(QDialogButtonBox::Close)) {
        qApp->quit();
    }
}

void CombineTranslationFiles::on_targetFilePushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Translation File"),
        "",
        tr("Qt Localization ts files (*.ts)"));
    ui->targetFile->setText(fileName);
}

void CombineTranslationFiles::on_sourceFilePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Translation file"), "", tr("Qt Localization ts files (*.ts)"));
    ui->sourceFile->setText(fileName);
}

void CombineTranslationFiles::readXML(QString sourcePath)
{
    QFile sourceFile(sourcePath);
    if (sourceFile.open(QIODevice::ReadOnly)) {

        xmlR.setDevice(&sourceFile);

        if (xmlR.readNextStartElement()) {
            if (xmlR.name() == "TS") {
                readTS();

                ui->textBrowser->append(QString("<br>Completed reading in file %1").arg(ui->sourceFile->text()));
                inputFileCount++;
                if (inputFileCount == 1) {
                    ui->textBrowser->append(QString("<b>Read in at least one more file to combine files.</b>").arg(ui->sourceFile->text()));
                }
                int total = 0;
                int modules = 0;
                QMapIterator<QString, QMap<QString, QMap<QString, QString> > > i(sourceStrings);
                while (i.hasNext()) {
                    i.next();
                    total += i.value().size();
                    modules++;
                }
                ui->textBrowser->append(QString("<br>Total size now %1 in %2 modules").arg(total).arg(modules));

            } else
                xmlR.raiseError(QObject::tr("The file is not a TS file"));
        }
    } else {
        if (ui->sourceFile->text().isEmpty()) {
            ui->textBrowser->append("No file selected for reading.");
        }
    }
}

void CombineTranslationFiles::readTS()
{
    Q_ASSERT(xmlR.isStartElement() && xmlR.name() == "TS");
    QString newlang = xmlR.attributes().value("language").toString();
    if (ui->language->text() != newlang
        && !ui->language->text().isEmpty()
        && ui->language->text() != "[none]") {
        ui->textBrowser->append(QString("<b>Warning! Language of previous source document was %1, new one has %2</b>").arg(ui->language->text()).arg(newlang));
    }
    ui->language->setText(newlang);

    while (xmlR.readNextStartElement()) {
        if (xmlR.name() == "context")
            readContext();
        else {
            qDebug() << "Unexpected element as child of TS" << xmlR.name();
            xmlR.skipCurrentElement();
        }
    }
}

void CombineTranslationFiles::readContext()
{
    Q_ASSERT(xmlR.isStartElement() && xmlR.name() == "context");
    QString context;

    while (xmlR.readNextStartElement()) {
        if (xmlR.name() == "name") {
            context = xmlR.readElementText();

        } else if (xmlR.name() == "message") {
            readMessage(context);
        } else {
            xmlR.skipCurrentElement();
        }
    }
}
void CombineTranslationFiles::readMessage(QString context)
{
    Q_ASSERT(xmlR.isStartElement() && xmlR.name() == "message");
    QString source;
    QString translation;
    QString translationtype;
    QString translatorcomment;
    QString location;
    QMap<QString, QString> targetStrings;
    bool addNewTranslation = false;

    while (xmlR.readNextStartElement()) {

        if (xmlR.name() == "source") {
            source = xmlR.readElementText();
        } else if (xmlR.name() == "translation") {
            // check if non-empty translation already exists; don't overwrite it
            if (sourceStrings[context].contains(source)) {
                QMap<QString, QString> existingTargetStrings = sourceStrings[context].value(source);
                if (existingTargetStrings.contains("translation")) {
                    if (existingTargetStrings.value("translation").isEmpty()) {
                        addNewTranslation = true;
                    }
                } else {
                    addNewTranslation = true;
                }
            } else {
                addNewTranslation = true;
            }

            translationtype = xmlR.attributes().value("type").toString();

            translation = xmlR.readElementText();

            targetStrings.insert("translationtype", translationtype);
            targetStrings.insert("translation", translation);

        } else if (xmlR.name() == "translatorcomment") {
            translatorcomment = xmlR.readElementText();
            targetStrings.insert("translatorcomment", translatorcomment);
        } else if (xmlR.name() == "location") {
            /// @todo there can be multiple locations in the file, and we should still
            /// keep corresponding filenames and lines together.
            targetStrings.insert("locationFilename", xmlR.attributes().value("filename").toString());
            targetStrings.insert("locationLine", xmlR.attributes().value("line").toString());
            location = xmlR.readElementText();
        } else {
            xmlR.skipCurrentElement();
        }
    }
    if (!source.isEmpty() && addNewTranslation) {
        // insert string to dict
        sourceStrings[context].insert(source, targetStrings);
    }
}

bool CombineTranslationFiles::writeFile(QString targetPath)
{
    QFile targetFile(targetPath);
    if (targetFile.open(QIODevice::WriteOnly)) {
        xmlW.setAutoFormatting(true);
        xmlW.setDevice(&targetFile);

        xmlW.writeStartDocument();
        xmlW.writeDTD("<!DOCTYPE TS>");
        xmlW.writeStartElement("TS");
        xmlW.writeAttribute("version", "2.1");
        xmlW.writeAttribute("language", ui->language->text());
        int total = 0;
        int modules = 0;
        QMapIterator<QString, QMap<QString, QMap<QString, QString> > > i(sourceStrings);
        while (i.hasNext()) {
            i.next();
            total += writeItem(i.key(), i.value());
            modules++;
        }

        xmlW.writeEndDocument();
        ui->textBrowser->append(QString("<br><b>%1 translations in %2 modules written to %3</b>").arg(total).arg(modules).arg(targetPath));
    } else {
        ui->textBrowser->append(QString("<br><b>Failed to write to file: \"%3\"</b>").arg(targetPath));
    }
    return true;
}
int CombineTranslationFiles::writeItem(QString context, QMap<QString, QMap<QString, QString> > content)
{
    xmlW.writeStartElement("context");

    xmlW.writeTextElement("name", context);
    int total = 0;

    QMapIterator<QString, QMap<QString, QString> > i(content);
    while (i.hasNext()) {
        i.next();

        xmlW.writeStartElement("message");

        xmlW.writeTextElement("source", i.key());

        xmlW.writeStartElement("translation");
        QString translationtype = i.value().value("translationtype");
        if (!translationtype.isEmpty()) {
            xmlW.writeAttribute("type", translationtype);
        }
        QString translation = i.value().value("translation");
        xmlW.writeCharacters(translation);
        xmlW.writeEndElement();

        xmlW.writeTextElement("translatorcomment", i.value().value("translatorcomment"));
        QString filename = i.value().value("locationFilename");
        if (!filename.isEmpty()) {
            xmlW.writeStartElement("location");
            xmlW.writeAttribute("filename", filename);
            xmlW.writeAttribute("line", i.value().value("locationLine"));
            xmlW.writeEndElement();
        }

        //message
        xmlW.writeEndElement();
        total++;
    }

    //context
    xmlW.writeEndElement();
    return total;
}

void CombineTranslationFiles::on_dockWidget_visibilityChanged(bool visible)
{
    ui->helpPushButton->setChecked(visible);
}

void CombineTranslationFiles::on_helpPushButton_clicked(bool checked)
{
    ui->dockWidget->setVisible(checked);
}

void CombineTranslationFiles::on_sourceFilePushButton_2_clicked()
{
    readXML(ui->sourceFile->text());
}
