#include "HxTheme.h"

#include "QCoreApplication"
#include "QSettings"
#include "QDir"
#include "QGraphicsDropShadowEffect"
#include "QToolTip"
#include "QJsonDocument"
#include "QJsonObject"

#include "HxFileManager.h"


namespace
{
HxTheme s_instance;
}


void MakeTransparentWindow(QWidget* pWidget)
{
    if(!pWidget)
        return;
    pWidget->setAttribute(Qt::WA_TranslucentBackground);
    auto flags = pWidget->windowFlags();
    flags |= Qt::FramelessWindowHint;
    pWidget->setWindowFlags(flags);
}

void MakeShadow(QWidget* pWidget, QPoint offset, double radius, QColor color)
{
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setOffset(offset);  // Đặt độ dịch chuyển bóng
    shadowEffect->setBlurRadius(radius); // Đặt bán kính làm mờ
    shadowEffect->setColor(color); // Đặt màu bóng
    pWidget->setGraphicsEffect(shadowEffect);
}

HxTheme::HxTheme()
{
}

HxTheme::~HxTheme()
{
    if(m_pThemeWatcher)
    {
        delete m_pThemeWatcher;
        m_pThemeWatcher = nullptr;
    }
}

QStringList HxTheme::GetThemes()
{
    Reset();
    std::map<QString, QString> itemsMap
        {
            {"default","Default"}
        };

    auto settings = GetFileManager()->GetSettings(HxFileManager::eSettingTheme);
    QString rootDir = settings->value("RootThemeDir").toString();
    QFileInfoList dirs = QDir(rootDir).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(auto &it : dirs)
    {
        QString name = it.fileName();
        QString key = name.toLower();
        auto itFind = itemsMap.find(key);
        if(itFind != itemsMap.end())
            itemsMap[key] = name;
    }

    QStringList items;
    for(auto &[key, name] : itemsMap)
        items.push_back(name);
    return items;
}

QString HxTheme::GetStyleSheets(const QStringList& names)
{
    Reset();
    QStringList items;
    if(names.isEmpty())
    {
        for(auto &[name, css] : m_styleSheets)
            items.push_back(css);
    }
    else
    {
        for(auto &name : names)
        {
            items.push_back(m_styleSheets[name]);
        }
    }
    return items.join("\n");
}

QColor HxTheme::GetColor(const QString& profile)
{
    QString textColor = m_colorMap[profile];
    return QColor(textColor);
}

void HxTheme::SetTheme(const QString& name)
{
    Reset();
    auto settings = GetFileManager()->GetSettings(HxFileManager::eSettingTheme);
    settings->setValue("Current", name);
    Refresh();
}

void HxTheme::Refresh()
{
    Reset();
    auto settings = GetFileManager()->GetSettings(HxFileManager::eSettingTheme);
    QString rootDir = settings->value("RootThemeDir").toString();
    QString current = settings->value("Current").toString();
    QString themeDir = rootDir + "/" + current;

    if(!QDir(themeDir).exists())
        return;

    disconnect(m_pThemeWatcher, &QFileSystemWatcher::directoryChanged, this, &HxTheme::OnThemeChanged);
    disconnect(m_pThemeWatcher, &QFileSystemWatcher::fileChanged, this, &HxTheme::OnThemeChanged);
    m_themeDir = themeDir;
    QStringList currentDirs = m_pThemeWatcher->files();
    if(!currentDirs.empty())
        m_pThemeWatcher->removePaths(currentDirs);

    QStringList items = GetFileManager()->GetFilesAndFolders(m_themeDir);
    m_pThemeWatcher->addPaths(items);
    OnThemeChanged(m_themeDir);
    connect(m_pThemeWatcher, &QFileSystemWatcher::directoryChanged, this, &HxTheme::OnThemeChanged);
    connect(m_pThemeWatcher, &QFileSystemWatcher::fileChanged, this, &HxTheme::OnThemeChanged);
}

void HxTheme::Reset()
{
    if(!m_pThemeWatcher)
        m_pThemeWatcher = new QFileSystemWatcher();
    auto settings = GetFileManager()->GetSettings(HxFileManager::eSettingTheme);
    QString rootDir = settings->value("RootThemeDir").toString();
    if(rootDir.isEmpty() || !QDir(rootDir).exists())
    {
        rootDir = QCoreApplication::applicationDirPath() + "/Themes";
        settings->setValue("RootThemeDir", rootDir);
    }

    QString current = settings->value("Current").toString();
    if(current.isEmpty())
    {
        settings->setValue("Current", "Default");
    }
}

void HxTheme::LoadStyleSheets()
{
    QFileInfoList fileLists = QDir(m_themeDir + "/css").entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QString imageDir = QString("%1/images").arg( m_themeDir);

    for(auto &fileInfo : fileLists)
    {
        QString extend = fileInfo.suffix().toLower();
        if(extend != "css")
            continue;

        QString baseName = fileInfo.baseName().toLower();
        QFile file(fileInfo.absoluteFilePath());
        if(!file.open(QIODevice::ReadOnly))
            continue;

        QString css = file.readAll();
        file.close();
        if(css.isEmpty())
            continue;

        m_styleSheets[baseName] = css.replace("#image-dir", imageDir);
    }
}

void HxTheme::LoadColors()
{
    QString fileName = QString("%1/data/color.json").arg( m_themeDir);
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
        return;

    QByteArray json = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject obj = doc.object();
    for(auto &key : obj.keys())
    {
        m_colorMap[key] = obj.value(key).toString();
    }
}

void HxTheme::LoadPalette()
{
    const std::map<QString, QPalette::ColorGroup> colorGroup ={
        {"Active",QPalette::Active},
        {"Disabled",QPalette::Disabled},
        {"Inactive",QPalette::Inactive},
        {"NColorGroups",QPalette::NColorGroups},
        {"Current",QPalette::Current},
        {"All",QPalette::All},
        {"Normal",QPalette::Normal}
    };

    const std::map<QString, QPalette::ColorRole> colorRoles = {
        {"WindowText",QPalette::WindowText},
        {"Button",QPalette::Button},
        {"Light",QPalette::Light},
        {"Midlight",QPalette::Midlight},
        {"Dark",QPalette::Dark},
        {"Mid",QPalette::Mid},
        {"Text",QPalette::Text},
        {"BrightText",QPalette::BrightText},
        {"ButtonText",QPalette::ButtonText},
        {"Base",QPalette::Base},
        {"Shadow",QPalette::Shadow},
        {"Highlight",QPalette::Highlight},
        {"HighlightedText",QPalette::HighlightedText},
        {"Link",QPalette::Link},
        {"LinkVisited",QPalette::LinkVisited},
        {"AlternateBase",QPalette::AlternateBase},
        {"NoRole",QPalette::NoRole},
        {"ToolTipBase",QPalette::ToolTipBase},
        {"ToolTipText",QPalette::ToolTipText},
        {"PlaceholderText",QPalette::PlaceholderText},
        {"Accent",QPalette::Accent},
        {"NColorRoles",QPalette::NColorRoles}
    };
}

void HxTheme::OnThemeChanged(const QString &)
{
    LoadStyleSheets();
    LoadColors();
    LoadPalette();
    qApp->setStyleSheet(GetStyleSheets());
}

HxTheme* GetTheme()
{
    return &s_instance;
}
