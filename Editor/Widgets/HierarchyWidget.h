#pragma once
#include <QWidget>
#include <QTreeWidgetitem>
#include "TreeObjectItem.h"
class Engine;
enum class PrimitiveType;

class HierarchyWidget : public QWidget
{
public:
	HierarchyWidget(QWidget* parent = nullptr);

	void SetEngine(Engine* value) { engine = value; }
	TreeObjectItem* CreatePrimitive(PrimitiveType type);

	TreeObjectItem* AddObject(const QString& name, TreeObjectItem* parent = nullptr);
	void RemoveObject(TreeObjectItem* item);
	TreeObjectItem* GetSelectedObject() const;
	QItemSelectionModel* GetSelectionModel() const { return tree->selectionModel(); }

private:
	void ShowContextMenu(const QPoint& position);
	Engine* engine = nullptr;
	QTreeWidget* tree = nullptr;
};

