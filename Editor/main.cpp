#include <iostream>
#include "DxEngineAPI.h"
#include <qapplication.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qtimer>
#include <qdockwidget.h>
#include "ViewportWidget.h"
#include "DetailsWidget.h"
#include "Engine.h"

#pragma comment(lib, "Engine.lib")
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QMainWindow window;
	window.setWindowTitle("DxEditor");
	window.resize(1280, 720);
	
	ViewportWidget* viewport = new ViewportWidget(&window);
	Engine engine(viewport->GetHWND(), 800, 600);
	window.setCentralWidget(viewport);

	DetailsWidget* details = new DetailsWidget();
	QDockWidget* detailsDock = new QDockWidget("Dtails", &window);
	detailsDock->setWidget(details);
	detailsDock->setAllowedAreas(
		Qt::LeftDockWidgetArea |
		Qt::RightDockWidgetArea
	);

	window.addDockWidget(
		Qt::RightDockWidgetArea,
		detailsDock
	);

	window.resizeDocks(
		{ detailsDock },
		{ 320 },
		Qt::Horizontal
	);

	window.show();
	engine.InitEngine();

	
	//QTimer renderTimer;
	//QObject::connect(&renderTimer, &QTimer::timeout, [&]()
	//	{
	//		engine.Render();
	//	});
	// renderTimer.start(16);
	while (window.isVisible())
	{
		QApplication::processEvents();
		engine.Update();
		engine.Render();
	}

	return 0;
}