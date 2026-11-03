#include <iostream>
#include "DxEngineAPI.h"
#include <qapplication.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qtimer>
#include "Engine.h"

#pragma comment(lib, "Engine.lib")
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QMainWindow window;
	window.setWindowTitle("DxEditor");
	window.resize(800, 600);
	window.show();
	HWND hWnd = reinterpret_cast<HWND>(window.winId());
	Engine engine(hWnd, 800, 600);
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
		engine.Render();
	}

	return 0;
}