#pragma once
#include "QMainWindow"
#include "QLabel"
#include "QLineEdit"
#include "QStandardItem"

#include "HxModel.h"

namespace Ui
{
    class ModelWindow;
}

class HxModelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxModelWindow( QWidget* parent = 0 );
    ~HxModelWindow();
private:
    Ui::ModelWindow* ui;
    HxModelPtrMap m_models;
    HxModelPtr m_pModel;

    QLineEdit* m_pSearchTextBox = nullptr;

    void showEvent( QShowEvent* );

    void showModels();
    void showModelInfo();
    void showMarkPositions();
    void showMarkBlocks();
    void showComments();

    void OnRefresh();
    void OnFilter(const QString& filter);
    void OnSelect( const QModelIndex& index );

    void OnNew();
    void OnRemove();
    void OnSave();
    void OnAddParam();
    void OnRemoveParam();

    void OnInfoChanged();
    void OnPositionChanged( QStandardItem* item );
    void OnCommentChanged( QStandardItem* item );
};
