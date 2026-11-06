#include "DetailsWidget.h"

DetailsWidget::DetailsWidget(QWidget* parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	transformGroup = new QGroupBox("Transform");
	formLayout = new QFormLayout(transformGroup);
	formLayout->addRow("Location", CreateVector3Editor(locationEditor, 0.0, 0.1));
	formLayout->addRow("Rotation", CreateVector3Editor(rotationEditor, 0.0, 0.1));
	formLayout->addRow("Scale", CreateVector3Editor(scaleEditor, 0.0, 0.1));
    mainLayout->addWidget(transformGroup);
}

QWidget* DetailsWidget::CreateVector3Editor(Vector3Editor& editor, double defaultValue, double step)
{
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(4);

    auto createAxis = [&](const QString& name)
        {
            QWidget* axisWidget = new QWidget(widget);
            QHBoxLayout* axisLayout = new QHBoxLayout(axisWidget);

            axisLayout->setContentsMargins(0, 0, 0, 0);
            axisLayout->setSpacing(3);

            QLabel* label = new QLabel(name, axisWidget);

            QDoubleSpinBox* spin = new QDoubleSpinBox(axisWidget);
            spin->setRange(-100000.0, 100000.0);
            spin->setDecimals(2);
            spin->setSingleStep(step);
            spin->setValue(defaultValue);
            spin->setButtonSymbols(QAbstractSpinBox::NoButtons);

            axisLayout->addWidget(label);
            axisLayout->addWidget(spin, 1);

            layout->addWidget(axisWidget, 1);

            return spin;
        };

    editor.X = createAxis("X");
    editor.Y = createAxis("Y");
    editor.Z = createAxis("Z");

    return widget;
}
