// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#include "tsdatavectormodel.h"
#include "waitcursorscope.h"
#include <QMimeData>
#include <QUrl>
#include <QDir>

TSDataVectorModel::TSDataVectorModel(QObject *parent)
    :QAbstractItemModel(parent)
{

}

bool TSDataVectorModel::add(const QStringList &sourcePahts)
{
    bool result = true;
    for (const auto& sourcePath : sourcePahts)
        result &= add(sourcePath);
    return result;
}

bool TSDataVectorModel::add(const QString &sourcePath)
{
    if (!TSData::isTSFile(sourcePath))
        return false;

    WaitCursorScope wc;

    TSData data;
    bool result = data.read(sourcePath);
    if (result)
    {
        int size = dataCollection.size();
        beginInsertRows(QModelIndex(), size, size);
        dataCollection.append(data);
        endInsertRows();
    }
    return result;
}

void TSDataVectorModel::remove(const QModelIndexList &indexList)
{
    if (indexList.isEmpty())
        return;

    // @todo: currently only contigous selections are supported
    int firstRow = indexList.first().row();
    int lastRow = indexList.last().row();
    beginRemoveRows(QModelIndex(), firstRow, lastRow);
    dataCollection.erase(dataCollection.begin() + firstRow, dataCollection.begin() + lastRow + 1);
    endRemoveRows();
}

void TSDataVectorModel::clear()
{
    beginResetModel();
    dataCollection.clear();
    endResetModel();
}

bool TSDataVectorModel::hasData() const
{
    return !dataCollection.empty();
}

TSData TSDataVectorModel::merge() const
{
    return ::merge(dataCollection);
}

TSDataStatistics TSDataVectorModel::calcStatistics() const
{
    return ::calcStatistics(dataCollection);
}

QVariant TSDataVectorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case 0:
        return tr("Source Path");
    case 1:
        return tr("Language");
    case 2:
        return tr("Contexts");
    case 3:
        return tr("Entries");
    default:
        return QVariant();
    }
}

int TSDataVectorModel::columnCount(const QModelIndex &) const
{
    return 4;
}

int TSDataVectorModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid())
        return dataCollection.size();
    else
        return 0;
}

QModelIndex TSDataVectorModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column);
}

QModelIndex TSDataVectorModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

QVariant TSDataVectorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const TSData &data = dataCollection.at(index.row());
    switch (index.column())
    {
    case 0:
        return QDir::toNativeSeparators(data.getSourcePath());
    case 1:
    {
        QString lang = data.getLang();
        if (lang.isEmpty())
        {
            lang = "-";
        }
        return lang;
    }
    case 2:
        return data.calcStatistics().contexts;
    case 3:
        return data.calcStatistics().entries;
    default:
        return QVariant();
    }
}

Qt::ItemFlags TSDataVectorModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;
}

Qt::DropActions TSDataVectorModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList TSDataVectorModel::mimeTypes() const
{
    QStringList types = QAbstractItemModel::mimeTypes();
    types << "text/uri-list";
    return types;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool TSDataVectorModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    return QAbstractItemModel::canDropMimeData(data, action, row, column, parent) && !getFilesFromMimeData(data).isEmpty();
}
#endif

bool TSDataVectorModel::dropMimeData(const QMimeData *data, Qt::DropAction, int, int, const QModelIndex &)
{
    QStringList sourceFiles = getFilesFromMimeData(data);
    add(sourceFiles);
    return !sourceFiles.isEmpty();
}

QStringList TSDataVectorModel::getFilesFromMimeData(const QMimeData *data)
{
    QStringList sourceFiles;
    if (data->hasUrls())
    {
        QList<QUrl> urls = data->urls();
        for (auto it = urls.begin(), endIt = urls.end(); it != endIt; ++it)
        {
            QString localFile = it->toLocalFile();
            if (TSData::isTSFile(localFile))
                sourceFiles.append(localFile);
        }

    }
    return sourceFiles;
}
