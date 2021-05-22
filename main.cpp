#include "sms.h"
#include "styleselector.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StyleSelector::setStyle(":/qss/stylesheet/studentMS.qss");

    SMS w;
    w.show();
    if(w.showLoginBox() == false){
        w.close();
        a.closeAllWindows();
        return 0;
    }
    return a.exec();
}
