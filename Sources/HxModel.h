#pragma once
#include "vector"
#include "vector"
#include "memory"

#include "QString"
#include "QMap"
#include "QJsonObject"

#include "HxDefines.h"
#include "HxPosition.h"
#include "HxObject.h"
#include "HxSettings.h"

class HxModel;
using HxModelPtr = std::shared_ptr<HxModel>;
using HxModelPtrArray = std::vector<HxModelPtr>;
using HxModelPtrMap = std::map<QString, HxModelPtr>;

class HxModel : public HxObject
{
public:
    enum ModifyType
    {
        eNew = 0x01,
        eInfo = 0x02,
        ePosition = 0x04,
        eComment = 0x08
    };
public:
    HxModel();
    ~HxModel();

    QString Code() const;
    QString Name() const;
    bool IsPrintLo() const;
    QString kNo() const;
    QString IVProgram() const;
    QString Design() const;
    double CvWidth() const;
    int Stopper() const;
    std::map<int, HxPosition>& Positions();
    HxPosition Position( int index ) const;
    std::map<QString, QString>& Comments();
    QString Comment( const QString& name ) const;
    QString Value( QString paramName ) const;

    void SetCode( const QString& value );
    void SetName( const QString& value );
    void SetPrintLo( bool bIsEnable );
    void SetkNo( const QString& value );
    void SetIVProgram( const QString& value );
    void SetDesign( const QString& value );
    void SetDesign( size_t value );
    void SetCvWidth( double value );
    void SetStopper( int value );
    void SetPositions( const std::map<int, HxPosition>& value );
    void SetPosition( int index, const HxPosition& value );
    void RemovePosition( int index );
    void SetComments( const std::map<QString, QString>& comments );
    void SetComment( const QString& key, const QString& value );
    void AddComments( const QStringList& keys);
    void RemoveComments( const QStringList& keys );
    void SetValue( const QString& key, const QString& value );

private:
    QString m_code;
    QString m_name;
    bool m_bIsPrintLo = false;
    QString m_kNo;
    QString m_ivProgram;
    QString m_design;
    double m_cvWidth = 0;
    int m_stopper = 1;
    std::map<int, HxPosition> m_positions;
    std::map<QString, QString> m_comments;

public:
    static QStringList paramNames();
};


class HxModelManager : private QObject
{
    Q_OBJECT
public:
    HxModelManager();
    HxModelPtr Create( const QString& code = QString(), const QString& name = QString() );
    QStringList Names();
    QStringList ParamNames();
    HxModelPtrMap GetModels();
    HxModelPtr GetModel( const QString& code );
    ReturnCode Save( HxModelPtr pModel );
    ReturnCode Removes( const QStringList& names );
    void AddComments( const QStringList& keys );
    void RemoveComments( const QStringList& keys );
    ReturnCode DeleteAll();
    void ReloadSetting();

private:
    HxRegistrySetting m_settings;
};

HxModelManager* ModelManager();
