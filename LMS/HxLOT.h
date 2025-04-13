#pragma once
#include "QString"
#include "QMap"
#include "vector"
#include "memory"

#include "HxObject.h"

class HxLOT;
using HxLOTPtr = std::shared_ptr<HxLOT>;
using HxLOTPtrArray = std::vector<HxLOTPtr>;
using HxLOTPtrMap = std::map<QString,HxLOTPtr>;

class HxLOT : public HxObject
{
public:
    enum ProductStatus
    {
        ePending = 1,
        eProduct = 0,
        eCompleted = 2
    };

    QString m_name;
    QString m_macStart = "";
    QString m_macEnd = "";
    QString m_counterStart = "";
    int m_quantity = 1;
    int m_progress = 0;
    bool m_isRePrint = false;
    QString m_modelName;
    QMap<QString, QString> m_comments;

    ProductStatus m_status = ePending;

    QString Name();
    QString CounterStart();
    QString CounterEnd();
    QString Counter();
    QString MACStart();
    QString MACEnd();
    QString MAC();
    int Quantity();
    int Progress();
    QString Model();
    QString Value( QString paramName );
    ProductStatus Status();
    void Evaluate();

    void SetName(const QString& value);
    void SetCounterStart(const QString& value);
    void SetMACStart(const QString& value);
    void SetMACEnd(const QString& value);
    void SetQuantity(int value);
    void SetProgress(int value);
    void SetModel(const QString& value);
    void SetValue( const QString& name, const QString& value );

    HxLOTPtr Clone();

public:
    HxLOT();
    ~HxLOT();
    bool nextItem();
    bool isCompleted();





public:
    // static std::vector<std::shared_ptr<HxLOT>> items;
    // static void remove( int index );
    // static void saveAll();
    // static void saveLot( std::shared_ptr<HxLOT> data );
    static QStringList paramNames();
};

class HxLOTManager
{
public:
    HxLOTPtr Create();
    HxLOTPtr GetLOT(const QString& lotName);
    HxLOTPtrMap GetLOTs();
    void Save(HxLOTPtr pLOT);
    void Removes(const QStringList& names);
    QStringList Parameters();
};

HxLOTManager* GetLOTManager();
