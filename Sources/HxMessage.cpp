#include "HxMessage.h"
#include <QMessageBox>

int HxMessage::show( QString content )
{
    QMessageBox msgBox;
    msgBox.setText( content );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setDefaultButton( QMessageBox::Ok );
    return msgBox.exec();
}

int HxMessage::show( QString content, QString title )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle( title );
    msgBox.setText( content );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setDefaultButton( QMessageBox::Ok );
    return msgBox.exec();
}

int HxMessage::show( QString content, QString title, QMessageBox::Icon icon )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle( title );
    msgBox.setText( content );
    msgBox.setIcon( icon );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setDefaultButton( QMessageBox::Ok );
    return msgBox.exec();
}

int HxMessage::show( QString content, QString title, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle( title );
    msgBox.setText( content );
    msgBox.setIcon( icon );
    msgBox.setStandardButtons( buttons );
    msgBox.setDefaultButton( QMessageBox::Default );
    return msgBox.exec();
}

int HxMessage::error( QString content )
{
    return HxMessage::show( content, "Error", QMessageBox::Icon::Critical );
}

int HxMessage::error( QString content, QString title )
{
    return HxMessage::show( content, title, QMessageBox::Icon::Critical );
}

int HxMessage::warning( QString content, QString title )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle( title );
    msgBox.setText( content );
    msgBox.setIcon( QMessageBox::Icon::Warning );
    msgBox.setStandardButtons( QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No );
    msgBox.setDefaultButton( QMessageBox::StandardButton::No );
    return msgBox.exec();
}
