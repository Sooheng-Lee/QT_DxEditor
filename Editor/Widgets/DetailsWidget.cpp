#include "DetailsWidget.h"
#include "Engine.h"
#include <QSignalBlocker>
#include <QLocale>

DetailsWidget::DetailsWidget(QWidget* parent) : QWidget(parent)
{
	mainLayout = new QVBoxLayout(this);
	transformGroup = new QGroupBox("Transform");
	formLayout = new QFormLayout(transformGroup);
	formLayout->addRow("Location", CreateVector3Editor(locationEditor, TransformComponent::Location, 0.0, 0.1));
	formLayout->addRow("Rotation", CreateVector3Editor(rotationEditor, TransformComponent::Rotation, 0.0, 0.1));
	formLayout->addRow("Scale", CreateVector3Editor(scaleEditor, TransformComponent::Scale, 1.0, 0.1));
    mainLayout->addWidget(transformGroup);
    SetObject(nullptr);
}

void DetailsWidget::SetObject(const BaseObject* object)
{
    auto setVector = [](Vector3Editor& editor, const XMFLOAT3& value)
    {
        const QSignalBlocker blockX(editor.X);
        const QSignalBlocker blockY(editor.Y);
        const QSignalBlocker blockZ(editor.Z);
        editor.X->setValue(value.x);
        editor.Y->setValue(value.y);
        editor.Z->setValue(value.z);
    };

    // Copy values only; do not retain a pointer to an object that may be deleted.
    setVector(locationEditor, object ? object->GetPosition() : XMFLOAT3(0, 0, 0));
    setVector(rotationEditor, object ? object->GetRotation() : XMFLOAT3(0, 0, 0));
    setVector(scaleEditor, object ? object->GetScale() : XMFLOAT3(1, 1, 1));
    transformGroup->setEnabled(object != nullptr);
}

QWidget* DetailsWidget::CreateVector3Editor(Vector3Editor& editor, TransformComponent component, double defaultValue, double step)
{
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(4);

    auto createAxis = [&](const QString& name, int axis)
        {
            QWidget* axisWidget = new QWidget(widget);
            QHBoxLayout* axisLayout = new QHBoxLayout(axisWidget);

            axisLayout->setContentsMargins(0, 0, 0, 0);
            axisLayout->setSpacing(3);

            QLabel* label = new QLabel(name, axisWidget);

            QDoubleSpinBox* spin = new QDoubleSpinBox(axisWidget);
            spin->setRange(-100000.0, 100000.0);
            spin->setDecimals(4);
            QLocale locale = QLocale::c();
            locale.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);
            spin->setLocale(locale);
            if (component == TransformComponent::Rotation)
                spin->setToolTip("Rotation in radians");
            spin->setSingleStep(step);
            spin->setValue(defaultValue);
            spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
            connect(spin, &QDoubleSpinBox::valueChanged, this,
                [this, component, axis](double value)
                {
                    if (transformGroup->isEnabled() && transformChanged)
                        transformChanged(component, axis, static_cast<float>(value));
                });

            axisLayout->addWidget(label);
            axisLayout->addWidget(spin, 1);

            layout->addWidget(axisWidget, 1);

            return spin;
        };

    editor.X = createAxis("X", 0);
    editor.Y = createAxis("Y", 1);
    editor.Z = createAxis("Z", 2);

    return widget;
}
