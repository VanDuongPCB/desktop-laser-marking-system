#ifndef UISTYLE_H
#define UISTYLE_H
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QProxyStyle>
#include <QStyleFactory>

class UIStyle : public QProxyStyle {
    Q_OBJECT
private:
    bool lightMode = true;
public:
    UIStyle(bool light = true);
    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;

private:
    QStyle *styleBase(QStyle *style = Q_NULLPTR) const;
};

#endif // DARKSTYLE_H
