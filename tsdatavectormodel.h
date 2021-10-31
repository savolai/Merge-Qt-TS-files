// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#ifndef TSDATAVECTORMODEL_H
#define TSDATAVECTORMODEL_H

#include <limits>
#include <QAbstractItemModel>
#include <QStringList>
#include <tsdata.h>

class TSDataVectorModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TSDataVectorModel(QObject *parent = nullptr);

    bool add(const QStringList &sourcePahts);

    bool add(const QString &sourcePaths);

    void remove(const QModelIndexList& indexList);

    void clear();

    bool hasData() const;

    TSData merge() const;

    TSDataStatistics calcStatistics() const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    virtual QModelIndex parent(const QModelIndex &index) const override;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual Qt::DropActions supportedDropActions() const override;

    virtual QStringList mimeTypes() const override;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
#endif

    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    static QStringList getFilesFromMimeData(const QMimeData *data);

private:
    TSDataVector dataCollection;
};

#endif
