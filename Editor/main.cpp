#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include "Widgets/ViewportWidget.h"
#include "Widgets/DetailsWidget.h"
#include "Widgets/HierarchyWidget.h"
#include "Engine.h"

#pragma comment(lib, "Engine.lib")

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QMainWindow window;
    window.setWindowTitle("DxEditor");
    window.resize(1400, 800);
    window.setDockNestingEnabled(true);
    QMenu* viewMenu = window.menuBar()->addMenu("View");

    // Keep the native DirectX surface separate from the dock's title bar.
    auto addDock = [&](const QString& title, const QString& name,
        QWidget* widget, Qt::DockWidgetArea area)
    {
        QDockWidget* dock = new QDockWidget(title, &window);
        dock->setObjectName(name);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setFeatures(QDockWidget::DockWidgetClosable |
            QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        dock->setWidget(widget);
        window.addDockWidget(area, dock);
        viewMenu->addAction(dock->toggleViewAction());
        return dock;
    };

    ViewportWidget* viewport = new ViewportWidget(&window);
    QDockWidget* viewportDock = addDock("Viewport", "ViewportDock",
        viewport, Qt::LeftDockWidgetArea);
    HierarchyWidget* hierarchy = new HierarchyWidget(&window);
    QDockWidget* hierarchyDock = addDock("Hierarchy", "HierarchyDock",
        hierarchy, Qt::RightDockWidgetArea);
    DetailsWidget* details = new DetailsWidget(&window);
    QDockWidget* detailsDock = addDock("Details", "DetailsDock",
        details, Qt::RightDockWidgetArea);
    QObject::connect(hierarchy->GetSelectionModel(), &QItemSelectionModel::selectionChanged,
        details, [hierarchy, details]()
        {
            TreeObjectItem* item = hierarchy->GetSelectedObject();
            details->SetObject(item ? item->GetEngineObject() : nullptr);
        });
    window.splitDockWidget(hierarchyDock, detailsDock, Qt::Vertical);
    window.resizeDocks({ viewportDock, hierarchyDock }, { 1000, 400 }, Qt::Horizontal);
    window.resizeDocks({ hierarchyDock, detailsDock }, { 300, 500 }, Qt::Vertical);

    window.show();
    Engine engine(viewport->GetHWND(), viewport->width(), viewport->height());
    if (!engine.InitEngine())
        return 1;
    viewport->SetEngine(&engine);
    hierarchy->SetEngine(&engine);
    details->SetTransformChangedCallback([hierarchy, &engine](TransformComponent component, int axis, float value)
    {
        // Resolve the current selection on every edit instead of retaining an object pointer.
        TreeObjectItem* item = hierarchy->GetSelectedObject();
        if (item)
            engine.SetTransformComponent(item->GetEngineObject(), component, axis, value);
    });

    QTimer renderTimer;
    QObject::connect(&renderTimer, &QTimer::timeout, &window, [&]()
    {
        engine.Update();
        if (viewport->isVisible() && !viewport->window()->isMinimized())
            engine.Render();
    });
    renderTimer.start(16);

    const int result = app.exec();
    renderTimer.stop();
    details->SetTransformChangedCallback({});
    details->SetObject(nullptr);
    hierarchy->SetEngine(nullptr);
    viewport->SetEngine(nullptr);
    return result;
}
