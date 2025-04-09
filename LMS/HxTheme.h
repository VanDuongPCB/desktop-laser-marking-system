#pragma once
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QFileSystemWatcher>


void MakeTransparentWindow(QWidget* pWidget);
void MakeShadow(QWidget* pWidget, QPoint offset = QPoint(0,0), double radius = 20, QColor color = Qt::black);


struct NxColorPalette
{
    QPalette::ColorGroup group;
    QPalette::ColorRole role;
};

class HxTheme: public QObject
{
    Q_OBJECT
public:
    HxTheme();
    QStringList GetThemes();
    QString GetStyleSheets(const QStringList& names = {});
    QColor GetColor(const QString& profile);

    void SetTheme(const QString& name);
    void Refresh();
private:
    QString m_themeDir;
    QFileSystemWatcher m_themeWatcher;
    std::map<QString, QString> m_styleSheets;
    std::map<QString, QString> m_colorMap;
    std::map<NxColorPalette, QColor> m_colorPalettes;

    void Reset();
    void LoadStyleSheets();
    void LoadColors();
    void LoadPalette();
    void OnThemeChanged(const QString &path);
};

HxTheme* GetTheme();

