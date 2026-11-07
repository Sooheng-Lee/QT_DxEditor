#include "HierarchyWidget.h"
#include <QVBoxLayout>
#include <QMenu>
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
}

void HierarchyWidget::ShowContextMenu(const QPoint& position)
{
    if (QTreeWidgetItem* clickedItem = tree->itemAt(position))
        tree->setCurrentItem(clickedItem);

    TreeObjectItem* selectedItem = tree->selectedItems().isEmpty()
        ? nullptr : GetSelectedObject();
    QMenu menu(this);
    QMenu* createMenu = menu.addMenu("Create");
    createMenu->setEnabled(engine != nullptr);
    createMenu->addAction("Cube", this, [this]()
    {
        CreatePrimitive(PrimitiveType::Cube);
    });
    createMenu->addAction("Sphere", this, [this]()
    {
        CreatePrimitive(PrimitiveType::Sphere);
    });
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

TreeObjectItem* HierarchyWidget::CreatePrimitive(PrimitiveType type)
{
    if (!engine)
        return nullptr;

    BaseObject* object = engine->CreatePrimitive(type);
    if (!object)
        return nullptr;

    const QString name = type == PrimitiveType::Cube ? "Cube" : "Sphere";
    // The engine stores a flat scene, so new objects are root items.
    TreeObjectItem* item = AddObject(name);
    item->SetEngineObject(object);
    tree->setCurrentItem(item);
    tree->scrollToItem(item);
    return item;
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
