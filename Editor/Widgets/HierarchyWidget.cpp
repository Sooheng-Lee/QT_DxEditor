#include "HierarchyWidget.h"
#include "CreateMenu.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QShortcut>

#include "Engine.h"
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
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, &QWidget::customContextMenuRequested,
        this, &HierarchyWidget::ShowContextMenu);
    auto deleteShortcut = new QShortcut(QKeySequence::Delete, tree);
    deleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(deleteShortcut, &QShortcut::activated, this, [this]()
    {
        RemoveObject(GetSelectedObject());
    });
}

void HierarchyWidget::ShowContextMenu(const QPoint& position)
{
    if (QTreeWidgetItem* clickedItem = tree->itemAt(position))
        tree->setCurrentItem(clickedItem);

    TreeObjectItem* selectedItem = tree->selectedItems().isEmpty()
        ? nullptr : GetSelectedObject();
    QMenu menu(this);
    menu.addMenu(new ModelCreateMenu("Create", this, &menu));
    if (selectedItem)
    {
        QAction* deleteAction = menu.addAction("Delete", this, [this, selectedItem]()
        {
            RemoveObject(selectedItem);
        });
        deleteAction->setEnabled(engine != nullptr);
    }
    menu.exec(tree->viewport()->mapToGlobal(position));
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
	if (!item || !engine || item->treeWidget() != tree) return;

    // Drag-and-drop can nest scene items. Remove their engine objects too.
    while (item->childCount() > 0)
        RemoveObject(item->GetChild(0));

    if (item == directionalLightItem)
    {
        engine->RemoveDirectionalLight();
        directionalLightItem = nullptr;
    }
    if (BaseObject* object = item->GetEngineObject())
    {
        engine->RemoveObject(object);
        item->SetEngineObject(nullptr);
    }
    delete item;
}

TreeObjectItem* HierarchyWidget::GetSelectedObject() const
{
	const auto selectedItems = tree->selectedItems();
	return selectedItems.isEmpty() ? nullptr
		: static_cast<TreeObjectItem*>(selectedItems.first());
}

void HierarchyWidget::ClearScene()
{
    if (!engine) return;
    tree->clearSelection();
    tree->setCurrentItem(nullptr);
    directionalLightItem = nullptr;
    tree->clear();
    engine->ResetScene();
}
