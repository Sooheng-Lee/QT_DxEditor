#include "TreeObjectItem.h"
#include <atomic>

static std::atomic<uint64_t> NextGameObjectID{ 1 };

TreeObjectItem::TreeObjectItem(const QString& name, TreeObjectItem* parent)
	: QTreeWidgetItem(parent)
{
	id = NextGameObjectID.fetch_add(1);
	SetName(name);
	setFlags(
		flags() |
		Qt::ItemIsEditable |
		Qt::ItemIsDragEnabled |
		Qt::ItemIsDropEnabled
	);
}

void TreeObjectItem::SetName(const QString& name)
{
	setText(0, name);
}

QString TreeObjectItem::GetName() const
{
	return text(0);
}

TreeObjectItem* TreeObjectItem::GetParent() const
{
	return static_cast<TreeObjectItem*>(parent());
}

TreeObjectItem* TreeObjectItem::GetChild(int idx) const
{
	if (idx < 0 || idx >= childCount())
		return nullptr;

	return static_cast<TreeObjectItem*>(child(idx));
}
