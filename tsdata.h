// Copyright 2017-2021 Olli Savolainen
// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#ifndef TSDATA_H
#define TSDATA_H

#include <limits>
#include <QMap>
#include <QString>
#include <QVector>
#include <QSharedPointer>
class QXmlStreamReader;
class QXmlStreamWriter;

struct TSDataStatistics
{
    void merge(const TSDataStatistics& other);

    int files = 0;

    int contexts = 0;

    int entries = 0;
};

class TSData
{
public:
    TSData();

    void merge(const TSData &tsData);

    bool read(const QString &sourcePath);

    bool write(const QString &targetPath);

    inline QString getSourcePath() const
    {
        return sourcePath;
    }

    inline QString getLang() const
    {
        return lang;
    }

    TSDataStatistics calcStatistics() const;

    static bool isTSFile(const QString &sourcePath);

private:
    void readTS(QXmlStreamReader &xmlR);

    void readContext(QXmlStreamReader &xmlR);

    void readMessage(QXmlStreamReader &xmlR, const QString &context);

    void writeItem(QXmlStreamWriter &xmlW, const QString &context, const QMap<QString, QMap<QString, QString>> &content);

    QMap<QString, QMap<QString, QMap<QString, QString>>> data;

    QString sourcePath;

    QString lang;
};

typedef QVector<TSData> TSDataVector;

TSData merge(const TSDataVector &data);

TSDataStatistics calcStatistics(const TSDataVector &data);

#endif
