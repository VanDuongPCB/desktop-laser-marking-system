#pragma once
#include "QDialog"
namespace Ui
{
	class LOTSelector;
}

class HxLOTSelector : public QDialog
{
public:
	HxLOTSelector(QWidget* parent  = nullptr);
	~HxLOTSelector();

private:
	Ui::LOTSelector* ui;
	void OnShow();
	void OnSelected( const QModelIndex& index );
};
