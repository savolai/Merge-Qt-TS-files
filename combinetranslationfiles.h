// Copyright 2017-2021 Olli Savolainen
// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#ifndef COMBINETRANSLATIONFILES_H
#define COMBINETRANSLATIONFILES_H

#include <limits>
#include <QMainWindow>
#include "tsdatavectormodel.h"
class QLabel;

namespace Ui
{
class CombineTranslationFiles;
}

class CombineTranslationFiles : public QMainWindow
{
    Q_OBJECT

public:
    explicit CombineTranslationFiles(QWidget *parent = nullptr);

    ~CombineTranslationFiles();

    bool addSourcePaths(const QStringList &sourcePahts);

    bool merge(const QString &targetPath);

private slots:
    void onAddClicked();

    void onRemoveClicked();

    void onClearClicked();

    void onMergeClicked();

    void onAboutClicked();

    void onDataChange();

    void onSelectionChange();

    void onContentsClicked();

private:
    QString getFilterText() const;

    Ui::CombineTranslationFiles *ui;

    QLabel *status;

    TSDataVectorModel model;
};

#endif
