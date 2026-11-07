#include "HierarchyWidget.h"
#include <QVBoxLayout>
HierarchyWidget::HierarchyWidget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	tree = new QTreeWidget(this);
	tree->setColumnCount(1);
	tree->setHeaderLabel("Hierarchy");

	tree->setSelectionMode(QAbstractItemView::SingleSelection);
	tree->setEditTriggers(
		QAbstractItemView::DoubleClicked |
		QAbstractItemView::EditKeyPressed);

	tree->setDragDropMode(QAbstractItemView::InternalMove);
	layout->addWidget(tree);
}

TreeObjectItem* HierarchyWidget::AddObject(const QString& name, TreeObjectItem* parent)
{
	TreeObjectItem* item = new TreeObjectItem(name, parent);
	if (!parent)
	{
		tree->addTopLevelItem(item);
	}

	return item;
}

void HierarchyWidget::RemoveObject(TreeObjectItem* item)
{
	if (!item) return;

	delete item;
}

TreeObjectItem* HierarchyWidget::GetSelectedObject() const
{
	TreeObjectItem* item = reinterpret_cast<TreeObjectItem*>(tree->currentItem());
	if (!item) return nullptr;
	return item;
}
