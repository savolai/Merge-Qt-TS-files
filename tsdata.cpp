// Copyright 2017-2021 Olli Savolainen
// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#include "tsdata.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

void TSDataStatistics::merge(const TSDataStatistics &other)
{
    files += other.files;
    contexts += other.contexts;
    entries += other.entries;
}

TSData::TSData() = default;

void TSData::merge(const TSData &tsData)
{
    if (lang.isEmpty())
        lang = tsData.lang;

    QMapIterator<QString, QMap<QString, QMap<QString, QString>>> i(tsData.data);
    while (i.hasNext())
    {
        i.next();

        auto currentContext = data.find(i.key());
        if (currentContext != data.end())
        {
            QMapIterator<QString, QMap<QString, QString>> j(i.value());
            while (j.hasNext())
            {
                j.next();

                bool addNewTranslation = true;
                auto currentSource = currentContext->find(j.key());
                // check if non-empty translation already exists; don't overwrite it
                if (currentSource != currentContext->end()) 
                    addNewTranslation = currentSource->value("translation").isEmpty();

                if (addNewTranslation)
                    currentContext->insert(j.key(), j.value());
            }
        }
        else
        {
            data.insert(i.key(), i.value());
        }
    }
}

bool TSData::read(const QString &sourcePath)
{
    const QString TAG_TS = "TS";

    this->sourcePath = sourcePath;
    QFile sourceFile(sourcePath);
    if (sourceFile.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlR;
        xmlR.setDevice(&sourceFile);

        if (xmlR.readNextStartElement())
        {
            if (xmlR.name() == TAG_TS)
            {
                readTS(xmlR);
                return true;
            }
        }
    }
    return false;
}

bool TSData::write(const QString &targetPath)
{
    QFile targetFile(targetPath);
    if (targetFile.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlW;
        xmlW.setAutoFormatting(true);
        xmlW.setDevice(&targetFile);
        xmlW.writeStartDocument();
        xmlW.writeDTD("<!DOCTYPE TS>");
        xmlW.writeStartElement("TS");
        xmlW.writeAttribute("version", "2.1");
        xmlW.writeAttribute("language", lang);
        QMapIterator<QString, QMap<QString, QMap<QString, QString>>> i(data);
        while (i.hasNext())
        {
            i.next();
            writeItem(xmlW, i.key(), i.value());
        }
        xmlW.writeEndDocument();
        return true;
    }
    else
    {
        return false;
    }
}

TSDataStatistics TSData::calcStatistics() const
{
    TSDataStatistics statistics;
    QMapIterator<QString, QMap<QString, QMap<QString, QString>>> i(data);
    while (i.hasNext())
    {
        i.next();
        statistics.entries += i.value().size();
        ++statistics.contexts;
    }
    ++statistics.files;
    return statistics;
}

bool TSData::isTSFile(const QString &sourcePath)
{
    return !sourcePath.isEmpty() && QFileInfo(sourcePath).suffix().compare("ts", Qt::CaseInsensitive) == 0;
}

void TSData::readTS(QXmlStreamReader &xmlR)
{
    const QString TAG_CONTEXT = "context";

    lang = xmlR.attributes().value("language").toString();

    while (xmlR.readNextStartElement())
    {
        if (xmlR.name() == TAG_CONTEXT)
        {
            readContext(xmlR);
        }
        else
        {
            xmlR.skipCurrentElement();
        }
    }
}

void TSData::readContext(QXmlStreamReader &xmlR)
{
    const QString TAG_NAME = "name";
    const QString TAG_MESSAGE = "message";
    QString context;

    while (xmlR.readNextStartElement())
    {
        if (xmlR.name() == TAG_NAME)
        {
            context = xmlR.readElementText();
        }
        else if (xmlR.name() == TAG_MESSAGE)
        {
            readMessage(xmlR, context);
        }
        else
        {
            xmlR.skipCurrentElement();
        }
    }
}

void TSData::readMessage(QXmlStreamReader &xmlR, const QString &context)
{
    QString source;
    QString translation;
    QString translationtype;
    QString translatorcomment;
    QString location;
    QMap<QString, QString> targetStrings;

    const QString TAG_SOURCE = "source";
    const QString TAG_TRANSLATION = "translation";
    const QString TAG_TRANSLATOR_COMMENT = "translatorcomment";
    const QString TAG_LOCATION = "location";

    while (xmlR.readNextStartElement())
    {
        if (xmlR.name() == TAG_SOURCE)
        {
            source = xmlR.readElementText();
        }
        else if (xmlR.name() == TAG_TRANSLATION)
        {
            translationtype = xmlR.attributes().value("type").toString();
            translation = xmlR.readElementText();
            targetStrings.insert("translationtype", translationtype);
            targetStrings.insert(TAG_TRANSLATION, translation);
        }
        else if (xmlR.name() == TAG_TRANSLATOR_COMMENT)
        {
            translatorcomment = xmlR.readElementText();
            targetStrings.insert(TAG_TRANSLATOR_COMMENT, translatorcomment);
        }
        else if (xmlR.name() == TAG_LOCATION)
        {
            /// @todo there can be multiple locations in the file, and we should still
            /// keep corresponding filenames and lines together.
            targetStrings.insert("locationFilename", xmlR.attributes().value("filename").toString());
            targetStrings.insert("locationLine", xmlR.attributes().value("line").toString());
            location = xmlR.readElementText();
        }
        else
        {
            xmlR.skipCurrentElement();
        }
    }

    if (!source.isEmpty())
        data[context].insert(source, targetStrings);
}

void TSData::writeItem(QXmlStreamWriter &xmlW, const QString &context, const QMap<QString, QMap<QString, QString>> &content)
{
    xmlW.writeStartElement("context");
    xmlW.writeTextElement("name", context);

    QMapIterator<QString, QMap<QString, QString>> i(content);
    while (i.hasNext())
    {
        i.next();

        xmlW.writeStartElement("message");
        xmlW.writeTextElement("source", i.key());
        xmlW.writeStartElement("translation");
        QString translationtype = i.value().value("translationtype");
        if (!translationtype.isEmpty())
            xmlW.writeAttribute("type", translationtype);
        QString translation = i.value().value("translation");
        xmlW.writeCharacters(translation);
        xmlW.writeEndElement();
        xmlW.writeTextElement("translatorcomment", i.value().value("translatorcomment"));
        QString filename = i.value().value("locationFilename");
        if (!filename.isEmpty())
        {
            xmlW.writeStartElement("location");
            xmlW.writeAttribute("filename", filename);
            xmlW.writeAttribute("line", i.value().value("locationLine"));
            xmlW.writeEndElement();
        }

        xmlW.writeEndElement();
    }

    xmlW.writeEndElement();
}

TSData merge(const TSDataVector &data)
{
    TSData mergedData;
    for (const auto& item : data)
        mergedData.merge(item);
    return mergedData;
}

TSDataStatistics calcStatistics(const TSDataVector &data)
{
    TSDataStatistics statistics;
    for (const auto& item : data)
        statistics.merge(item.calcStatistics());
    return statistics;
}
