#pragma once
#include <qwidget.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QLabel>

struct Vector3Editor
{
	QDoubleSpinBox* X = nullptr;
	QDoubleSpinBox* Y = nullptr;
	QDoubleSpinBox* Z = nullptr;
};

class DetailsWidget : public QWidget
{
public:
	DetailsWidget(QWidget* parent = nullptr);

private:
	QWidget* CreateVector3Editor(Vector3Editor& editor, double defaultValue, double step);
private:
	QVBoxLayout* mainLayout;
	QGroupBox* transformGroup;
	QFormLayout* formLayout;

	Vector3Editor locationEditor;
	Vector3Editor rotationEditor;
	Vector3Editor scaleEditor;
};

