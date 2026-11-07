#pragma once
#include <qtreewidget.h>
#include <cstdint>

class BaseObject;
class TreeObjectItem : public QTreeWidgetItem
{
public:
	TreeObjectItem(const QString& name, TreeObjectItem* parent = nullptr);
	~TreeObjectItem() override = default;

	void SetName(const QString& name);
	QString GetName() const;
	TreeObjectItem* GetParent() const;
	TreeObjectItem* GetChild(int idx) const;

	uint64_t GetID() const {
		return id;
	};
private:
	uint64_t id = 0;
	BaseObject* object = nullptr;
};

