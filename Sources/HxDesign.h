#pragma once
#include "memory"
#include "vector"

#include "QMap"
#include "QString"
#include "QObject"

#include "HxBlock.h"
#include "HxObject.h"
#include "HxSettings.h"

class HxDesign;
using HxDesignPtr = std::shared_ptr< HxDesign >;
using HxDesignPtrMap = std::map<QString, HxDesignPtr >;

class HxDesign : public HxObject
{
public:
    enum ModifyType
    {
        eNew = 0x01,
        eSize = 0x02,
        eBlock = 0x04
    };

    HxDesign();
    ~HxDesign();

    QString Name() const;
    double Width() const;
    double Height() const;
    HxBlock Block( int index );
    std::map<int, HxBlock> Blocks();
    int IndexOfBlockCode();

    void SetName( const QString& name );
    void SetWidth( double value );
    void SetHeight( double value );
    void SetBlock( int index, const HxBlock& block );
    
private:
    QString m_name = "0000";
    double m_width = 10;
    double m_height = 10;
    std::map<int, HxBlock> m_blocks;
};

class HxDesignManager : private QObject
{
    Q_OBJECT
public:
    HxDesignManager();
    HxDesignPtr Create();
    HxDesignPtr GetDesign( const QString& name );
    HxDesignPtrMap GetDesigns();
    void Save( HxDesignPtr pDesign );
    void Migration( const QString& dir );
private:
    HxRegistrySetting m_settings;
    bool eventFilter( QObject* watched, QEvent* event );
};

HxDesignManager* DesignManager();