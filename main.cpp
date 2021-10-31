// Copyright 2017-2021 Olli Savolainen
// SPDX-License-Identifier: Apache-2.0

#include "combinetranslationfiles.h"
#include <QApplication>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
#include <QCommandLineParser>
#endif

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFont font;
    font.setFamily("Segoe UI");
    font.setPointSize(10);
    app.setFont(font);

    CombineTranslationFiles w;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    {
        QCommandLineParser parser;
        QCommandLineOption outputOption(QStringList() << "o" << "output", "Target path for merged TS data.", "file");
        parser.addOption(outputOption);
        parser.addPositionalArgument("files", "Source paths of TS data.", "[files...]");
        parser.process(app);

        QStringList sourcePahts = parser.positionalArguments();
        if (!sourcePahts.empty())
        {
            w.addSourcePaths(sourcePahts);
            if (parser.isSet(outputOption))
            {
                QString targetPath = parser.value(outputOption);
                if (!targetPath.isEmpty())
                {
                    if (!w.merge(targetPath))
                        return 1;
                    else
                        return 0;
                }
            }
        }
    }
#endif

    w.show();

    return app.exec();
}
