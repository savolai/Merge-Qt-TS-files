#include "combinetranslationfiles.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QFont font;
    font.setFamily("Segoe UI");
    font.setPointSize(10);
    app.setFont(font);

    CombineTranslationFiles w;
    w.show();

    return app.exec();
}
