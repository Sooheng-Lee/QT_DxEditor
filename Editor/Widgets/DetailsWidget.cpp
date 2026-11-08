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
    transformGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lightGroup = new QGroupBox("Directional Light");
    lightGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto lightLayout = new QFormLayout(lightGroup);
    auto addLightRow = [&](const QString& label, LightProperty property,
        const QStringList& axes, double minimum, double maximum)
    {
        auto row = new QWidget(lightGroup);
        auto layout = new QHBoxLayout(row);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(3);
        LightEditor editor{ property, {} };
        for (int axis = 0; axis < axes.size(); ++axis)
        {
            if (!axes[axis].isEmpty()) layout->addWidget(new QLabel(axes[axis], row));
            auto spin = new QDoubleSpinBox(row);
            spin->setRange(minimum, maximum);
            spin->setDecimals(4);
            spin->setSingleStep(0.1);
            spin->setLocale(QLocale::c());
            spin->setKeyboardTracking(false);
            spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
            if (property == LightProperty::CameraPosition)
                spin->setToolTip("Updated automatically when the viewport camera moves");
            layout->addWidget(spin, 1);
            editor.fields.push_back(spin);
            connect(spin, &QDoubleSpinBox::valueChanged, this, [this, property, axis](double value)
            {
                if (lightGroup->isEnabled() && lightChanged)
                    lightChanged(property, axis, static_cast<float>(value));
            });
        }
        lightEditors.push_back(std::move(editor));
        lightLayout->addRow(label, row);
    };
    addLightRow("Direction", LightProperty::Direction, { "X", "Y", "Z" }, -100000, 100000);
    addLightRow("Color", LightProperty::Color, { "R", "G", "B", "A" }, 0, 1);
    addLightRow("Ambient Color", LightProperty::AmbientColor, { "R", "G", "B" }, 0, 1);
    addLightRow("Intensity", LightProperty::Intensity, { "" }, 0, 100000);
    addLightRow("Shininess", LightProperty::Shininess, { "" }, 1, 100000);
    addLightRow("Specular Strength", LightProperty::SpecularStrength, { "" }, 0, 100000);
    addLightRow("Camera Position", LightProperty::CameraPosition, { "X", "Y", "Z" }, -100000, 100000);
    mainLayout->addWidget(lightGroup);
    mainLayout->addStretch();
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
    transformGroup->setVisible(object != nullptr);
    lightGroup->setEnabled(false);
    lightGroup->hide();
}

void DetailsWidget::SetDirectionalLight(const DirectionalLightData& data)
{
    transformGroup->setEnabled(false);
    transformGroup->hide();
    lightGroup->setEnabled(true);
    lightGroup->show();
    for (const auto& editor : lightEditors)
    {
        std::vector<float> values;
        switch (editor.property)
        {
        case LightProperty::Direction: values = { data.direction.x, data.direction.y, data.direction.z }; break;
        case LightProperty::Color: values = { data.color.x, data.color.y, data.color.z, data.color.w }; break;
        case LightProperty::AmbientColor: values = { data.ambientColor.x, data.ambientColor.y, data.ambientColor.z }; break;
        case LightProperty::CameraPosition: values = { data.cameraPos.x, data.cameraPos.y, data.cameraPos.z }; break;
        case LightProperty::Shininess: values = { data.shiness }; break;
        case LightProperty::SpecularStrength: values = { data.specularStrength }; break;
        case LightProperty::Intensity: values = { data.intensity }; break;
        }
        for (size_t axis = 0; axis < values.size(); ++axis)
        {
            const QSignalBlocker blocker(editor.fields[axis]);
            editor.fields[axis]->setValue(values[axis]);
        }
    }
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
