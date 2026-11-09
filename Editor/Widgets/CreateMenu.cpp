#include "CreateMenu.h"
#include "HierarchyWidget.h"
#include "Engine.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QByteArray>
#include <QFileInfo>
#include <exception>

ModelCreateMenu::ModelCreateMenu(const QString& title, HierarchyWidget* hierarchyWidget, QWidget* parent)
    : QMenu(title, parent), hierarchy(hierarchyWidget)
{
    addAction("Cube", this, [this]()
    {
        CreatePrimitive(PrimitiveType::Cube);
    });
    addAction("Sphere", this, [this]()
    {
        CreatePrimitive(PrimitiveType::Sphere);
    });
    addAction("Cylinder", this, [this]()
    {
        CreatePrimitive(PrimitiveType::Cylinder);
    });

    addAction("Load Asset", this, [this]() {
        if (!hierarchy->engine)
            return;

        const QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("Load Asset"),
            QString(),
            tr("3D Models (*.fbx *.obj);;FBX (*.fbx);;OBJ (*.obj)")
        );

        if (filePath.isEmpty()) return;

        try
        {
            const QByteArray utf8Path = filePath.toUtf8();
            BaseObject* object = hierarchy->engine->LoadAsset(utf8Path.constData());
            TreeObjectItem* item = hierarchy->AddObject(QFileInfo(filePath).completeBaseName());
            item->SetEngineObject(object);
            hierarchy->tree->setCurrentItem(item);
            hierarchy->tree->scrollToItem(item);
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(
                this,
                tr("Load Asset Failed"),
                QString::fromUtf8(e.what())
            );
        }
    });

    QAction* lightAction = addAction("Directional Light", this, [this]()
    {
        CreateDirectionalLight();
    });
    connect(this, &QMenu::aboutToShow, this, [this, lightAction]()
    {
        for (QAction* action : this->actions())
            action->setEnabled(hierarchy->engine != nullptr);
        lightAction->setEnabled(hierarchy->engine && !hierarchy->engine->HasDirectionalLight()
            && !hierarchy->directionalLightItem);
    });
}

TreeObjectItem* ModelCreateMenu::CreatePrimitive(PrimitiveType type)
{
    if (!hierarchy->engine)
        return nullptr;

    BaseObject* object = hierarchy->engine->CreatePrimitive(type);
    if (!object)
        return nullptr;

    QString name;
    switch (type)
    {
    case PrimitiveType::Cube: name = "Cube"; break;
    case PrimitiveType::Sphere: name = "Sphere"; break;
    case PrimitiveType::Cylinder: name = "Cylinder"; break;
    default: name = "Object"; break;
    }
    // The engine stores a flat scene, so new objects are root items.
    TreeObjectItem* item = hierarchy->AddObject(name);
    item->SetEngineObject(object);
    hierarchy->tree->setCurrentItem(item);
    hierarchy->tree->scrollToItem(item);
    return item;
}

TreeObjectItem* ModelCreateMenu::CreateDirectionalLight()
{
    if (!hierarchy->engine || hierarchy->directionalLightItem || !hierarchy->engine->CreateDirectionalLight())
        return nullptr;

    hierarchy->directionalLightItem = hierarchy->AddObject("Directional Light");
    hierarchy->tree->setCurrentItem(hierarchy->directionalLightItem);
    hierarchy->tree->scrollToItem(hierarchy->directionalLightItem);
    return hierarchy->directionalLightItem;
}

