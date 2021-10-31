// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <limits>
#include <QDialog>

namespace Ui
{
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif
