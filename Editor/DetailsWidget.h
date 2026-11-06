#pragma once
#include <qwidget.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QLabel>

class DetailsWidget : public QWidget
{
public:
	DetailsWidget(QWidget* parent = nullptr);

private:
	QVBoxLayout* mainLayout;
	QGroupBox* transformGroup;
	QFormLayout* formLayout;
};

