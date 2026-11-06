#include "DetailsWidget.h"

DetailsWidget::DetailsWidget(QWidget* parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	transformGroup = new QGroupBox("Transform");
	formLayout = new QFormLayout(transformGroup);
	formLayout->addRow("Position", new QWidget());
	formLayout->addRow("Rotation", new QWidget());
	formLayout->addRow("Scale", new QWidget());
}
