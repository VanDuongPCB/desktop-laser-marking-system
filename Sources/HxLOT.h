#pragma once
#include "vector"
#include "memory"

#include "QString"
#include "QMap"
#include "QDate"

#include "HxDefines.h"
#include "HxObject.h"
#include "HxSettings.h"

class HxLOT;
using HxLOTPtr = std::shared_ptr<HxLOT>;
using HxLOTPtrArray = std::vector<HxLOTPtr>;
using HxLOTPtrMap = std::map<QString, HxLOTPtr>;

class HxLOT : public HxObject
{
public:
    enum ProductStatus
    {
        eProduct = 0,
        ePending = 1,
        eCompleted = 2
    };

    enum ModifyFlag : uint64_t
    {
        eNew = 0x01,
        eInfo = 0x02,
        eProgress = 0x04,
        eComment = 0x08
    };

    HxLOT();
    ~HxLOT();

    QString Name() const;
    QString CounterStart() const;
    QString CounterEnd() const;
    QString Counter( int shift = 0 ) const;
    QString MACStart() const;
    QString MACEnd() const;
    QString MAC( int shift = 0 ) const;
    int Quantity() const;
    int Progress() const;
    QString Model() const;
    bool IsRePrint() const;
    QString Value( QString paramName ) const;
    std::map<QString, QString> Comments() const;
    bool IsCompleted();
    ProductStatus Status() const;
    void Evaluate();

    void SetName( const QString& value );
    void SetCounterStart( const QString& value );
    void SetMACStart( const QString& value );
    void SetMACEnd( const QString& value );
    void SetQuantity( int value );
    void SetProgress( int value );
    void SetModel( const QString& value );
    void SetRePrint( bool bIsEnabled );
    void SetValue( const QString& name, const QString& value );

    bool NextItem();
    HxLOTPtr Clone() const;

private:
    QString m_name;
    QString m_macStart = "";
    QString m_macEnd = "";
    QString m_counterStart = "";
    int m_quantity = 1;
    int m_progress = 0;
    bool m_isRePrint = false;
    QString m_modelName;
    std::map<QString, QString> m_comments;
    ProductStatus m_status = ePending;
};

class HxLOTManager: public QObject
{
    Q_OBJECT
public:
    explicit HxLOTManager();
    HxLOTPtr Create();
    HxLOTPtr GetLOT( const QString& lotName );
    HxLOTPtrMap GetLOTs( const QDate& fromTime = QDate( 2000, 1, 1 ) );
    ReturnCode Save( HxLOTPtr pLOT );
    ReturnCode Remove( HxLOTPtr pLOT );
    ReturnCode Remove( const QString& name );
    ReturnCode Removes( const std::set<QString>& names );
    QStringList Parameters();
    ReturnCode DeleteAll();
    void ReloadSetting();

private:
    HxRegistrySetting m_settings;
    QStringList m_paramNames;
};

HxLOTManager* LOTManager();
