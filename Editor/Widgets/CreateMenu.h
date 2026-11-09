#pragma once
#include <QMenu>

class HierarchyWidget;
class TreeObjectItem;
enum class PrimitiveType;

class ModelCreateMenu : public QMenu
{
public:
    ModelCreateMenu(const QString& title, HierarchyWidget* hierarchy, QWidget* parent = nullptr);

private:
    TreeObjectItem* CreatePrimitive(PrimitiveType type);
    TreeObjectItem* CreateDirectionalLight();
    HierarchyWidget* hierarchy;
};
