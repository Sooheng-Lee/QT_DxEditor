#include <iostream>
#include "Engine/DxEngineAPI.h"
#include <qapplication.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qtimer>
#include <qdockwidget.h>
#include <qsplitter.h>
#include "Widgets/ViewportWidget.h"
#include "Widgets/DetailsWidget.h"
#include "Engine/Engine.h"

#pragma comment(lib, "Engine.lib")
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QMainWindow window;
	window.setWindowTitle("DxEditor");
	window.setFixedSize(1000, 600);
	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	ViewportWidget* viewport = new ViewportWidget(splitter);
	viewport->setFixedSize(800, 600);
	Engine engine(viewport->GetHWND(), 800, 600);
	viewport->SetEngine(&engine);
	splitter->addWidget(viewport);

	DetailsWidget* details = new DetailsWidget(splitter);
	details->setFixedSize(200, 600);
	splitter->addWidget(details);
	splitter->setSizeIncrement({ 1000, 600 });

	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 0);
	splitter->setChildrenCollapsible(false);
	window.setCentralWidget(splitter);
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