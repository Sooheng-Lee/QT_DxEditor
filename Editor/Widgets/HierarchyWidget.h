#pragma once
#include <QWidget>
#include <QTreeWidgetitem>
#include "TreeObjectItem.h"
class HierarchyWidget : public QWidget
{
public:
	HierarchyWidget(QWidget* parent = nullptr);

	TreeObjectItem* AddObject(const QString& name, TreeObjectItem* parent = nullptr);
	void RemoveObject(TreeObjectItem* item);
	TreeObjectItem* GetSelectedObject() const;

private:
	QTreeWidget* tree = nullptr;
};

