#pragma once
#include <qwidget.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <functional>
#include <utility>
#include <vector>

class BaseObject;
enum class TransformComponent;
enum class LightProperty;
struct DirectionalLightData;

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
	void SetObject(const BaseObject* object);
	void SetDirectionalLight(const DirectionalLightData& data);
	using LightChangedCallback = std::function<void(LightProperty, int, float)>;
	void SetLightChangedCallback(LightChangedCallback callback) { lightChanged = std::move(callback); }
    using TransformChangedCallback = std::function<void(TransformComponent, int, float)>;
    void SetTransformChangedCallback(TransformChangedCallback callback)
    {
        transformChanged = std::move(callback);
    }

private:
	QWidget* CreateVector3Editor(Vector3Editor& editor, TransformComponent component, double defaultValue, double step);
private:
	TransformChangedCallback transformChanged;
	LightChangedCallback lightChanged;
	QGroupBox* lightGroup;
	struct LightEditor { LightProperty property; std::vector<QDoubleSpinBox*> fields; };
	std::vector<LightEditor> lightEditors;
	QVBoxLayout* mainLayout;
	QGroupBox* transformGroup;
	QFormLayout* formLayout;

	Vector3Editor locationEditor;
	Vector3Editor rotationEditor;
	Vector3Editor scaleEditor;
};

