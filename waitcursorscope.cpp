// Copyright 2021 Thomas Ascher
// SPDX-License-Identifier: Apache-2.0

#include "waitcursorscope.h"
#include <QApplication>

WaitCursorScope::WaitCursorScope()
{
    qApp->setOverrideCursor(Qt::WaitCursor);
}

WaitCursorScope::~WaitCursorScope()
{
    qApp->restoreOverrideCursor();
}
