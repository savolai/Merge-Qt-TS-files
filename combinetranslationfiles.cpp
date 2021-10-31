// Copyright 2017-2021 Olli Savolainen
// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#include "combinetranslationfiles.h"
#include "ui_combinetranslationfiles.h"
#include "aboutdialog.h"
#include "waitcursorscope.h"
#include <QFileDialog>
#include <QLabel>

CombineTranslationFiles::CombineTranslationFiles(QWidget *parent)
    :QMainWindow(parent)
    ,ui(new Ui::CombineTranslationFiles)
{
    ui->setupUi(this);
    ui->dockWidget->hide();
    status = new QLabel(ui->statusBar);
    ui->statusBar->addWidget(status);
    ui->treeView->setRootIsDecorated(false);
    ui->treeView->setModel(&model);
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))    
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);    
#endif    
    connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(onAddClicked()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(onRemoveClicked()));
    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(onClearClicked()));
    connect(ui->actionMerge, SIGNAL(triggered()), this, SLOT(onMergeClicked()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAboutClicked()));
    connect(ui->actionContents, SIGNAL(triggered()), this, SLOT(onContentsClicked()));
    connect(&model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(onDataChange()));
    connect(&model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(onDataChange()));
    connect(&model, SIGNAL(modelReset()), this, SLOT(onDataChange()));
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(onSelectionChange()));
    onDataChange();
}

CombineTranslationFiles::~CombineTranslationFiles()
{
    delete ui;
}

bool CombineTranslationFiles::addSourcePaths(const QStringList &sourcePahts)
{
    return model.add(sourcePahts);
}

bool CombineTranslationFiles::merge(const QString &targetPath)
{
    WaitCursorScope wc;
    TSData data = model.merge();
    TSDataStatistics statictics = data.calcStatistics();
    ui->statusBar->showMessage(tr("%1 contexts and %2 entries exported")
                                   .arg(statictics.contexts)
                                   .arg(statictics.entries), 3000);
    return data.write(targetPath);
}

void CombineTranslationFiles::onAddClicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Add Qt Localization TS Files"), QString(), getFilterText());
    if (!fileNames.isEmpty())
    {
        model.add(fileNames);
    }
}

void CombineTranslationFiles::onRemoveClicked()
{
    model.remove(ui->treeView->selectionModel()->selectedRows());
}

void CombineTranslationFiles::onClearClicked()
{
    model.clear();
}

void CombineTranslationFiles::onMergeClicked()
{
    QString targetPath = QFileDialog::getSaveFileName(this, tr("Save Qt Localization TS File"),  QString(), getFilterText());
    if (!targetPath.isEmpty())
    {
        if (!TSData::isTSFile(targetPath))
            targetPath.append(".ts");
        merge(targetPath);
    }
}

void CombineTranslationFiles::onAboutClicked()
{
    AboutDialog about(this);
    about.exec();
}

void CombineTranslationFiles::onDataChange()
{
    bool hasData = model.hasData();
    ui->actionMerge->setEnabled(hasData);
    ui->actionClear->setEnabled(hasData);
    onSelectionChange();

    TSDataStatistics statistics = model.calcStatistics();
    status->setText(tr("%1 files with %2 contexts and %3 entries loaded")
                           .arg(statistics.files)
                           .arg(statistics.contexts)
                           .arg(statistics.entries));
}

void CombineTranslationFiles::onSelectionChange()
{
    bool hasSelection = model.hasData() && ui->treeView->selectionModel()->hasSelection();
    ui->actionRemove->setEnabled(hasSelection);
}

void CombineTranslationFiles::onContentsClicked()
{
    ui->dockWidget->show();
}

QString CombineTranslationFiles::getFilterText() const
{
    return tr("Qt Localization TS Files (*.ts)");
}
