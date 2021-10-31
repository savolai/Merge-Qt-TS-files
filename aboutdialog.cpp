// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent, Qt::WindowFlags f)
    :QDialog(parent, f)
    ,ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
