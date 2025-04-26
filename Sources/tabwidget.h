#ifndef TABWIDGET_H
#define TABWIDGET_H
#include <QTabWidget>

class TabWidget : public QTabWidget{
private:

public:
    TabWidget(QWidget *parent = nullptr);
    ~TabWidget();
};

#endif // TABWIDGET_H
