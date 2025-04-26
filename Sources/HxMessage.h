#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QMessageBox>

class HxMessage
{
public:
    static int show( QString content );
    static int show( QString content, QString title );
    static int show( QString content, QString title, QMessageBox::Icon icon );
    static int show( QString content, QString title, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons );
    static int error( QString content );
    static int error( QString content, QString title );
    static int warning( QString content, QString title );
};

#endif // MESSAGE_H
