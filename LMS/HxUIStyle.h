#pragma once
#include "QApplication"
#include "QFile"
#include "QFont"
#include "QProxyStyle"
#include "QStyleFactory"

class HxUIStyle : public QProxyStyle
{
    Q_OBJECT
private:
    bool lightMode = true;
public:
    HxUIStyle( bool light = true );
    void polish( QPalette& palette ) override;
    void polish( QApplication* app ) override;

private:
    QStyle* styleBase( QStyle* style = Q_NULLPTR ) const;
};

