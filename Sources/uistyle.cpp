#include "uistyle.h"
#include <QToolTip>

UIStyle::UIStyle(bool light) : QProxyStyle(QStyleFactory::create("fusion")) {
    this->lightMode = light;
}

void UIStyle::polish(QPalette &palette) {
    if(lightMode){
        palette.setColor(QPalette::Window, QColor(240, 240, 240));
        palette.setColor(QPalette::WindowText, QColor(48,48,48));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        palette.setColor(QPalette::Base, QColor(250, 250, 250));
        palette.setColor(QPalette::AlternateBase, QColor(240,240,240));
        palette.setColor(QPalette::ToolTipBase, QColor(240,240,240));
        palette.setColor(QPalette::ToolTipText, QColor(48, 48, 48));
        palette.setColor(QPalette::Text, QColor(48,48,48));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        palette.setColor(QPalette::Light, QColor(35, 35, 35));
        palette.setColor(QPalette::Midlight, QColor(35, 35, 35));
        palette.setColor(QPalette::Mid, QColor(35, 35, 35));
        palette.setColor(QPalette::Dark, QColor(35, 35, 35));
        palette.setColor(QPalette::Shadow, QColor(127, 127, 127));
        palette.setColor(QPalette::Button, QColor(255, 255, 255));
        palette.setColor(QPalette::ButtonText, QColor(48,48,48));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    }
    else{
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        palette.setColor(QPalette::Base, QColor(42, 42, 42));
        palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        palette.setColor(QPalette::ToolTipBase, QColor(53,53,53));
        palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        palette.setColor(QPalette::Dark, QColor(35, 35, 35));
        palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));


    }
    QToolTip::setPalette(palette);
}

void UIStyle::polish(QApplication *app) {
    if (!app) return;
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(10);
    app->setFont(defaultFont);

    app->setStyleSheet("QToolTip{border:1px solid #888; padding:7px 7px;}");
}
