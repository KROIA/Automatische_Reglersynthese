#ifdef QT_ENABLED
#include <QApplication>
#endif
#include <iostream>
#include <string>

#include "AutoTuner.h"

#ifdef QT_WIDGETS_ENABLED
#include <QWidget>
#endif

#include "MainWindow.h"
#include "MatlabEngine.hpp"


int main(int argc, char* argv[])
{
	
	std::cout << "Test\n";
#ifdef QT_WIDGETS_ENABLED
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#ifdef QT_ENABLED
	QApplication app(argc, argv);
#endif

	Log::UI::createConsoleView(Log::UI::nativeConsoleView);
#ifdef AT_DEBUG
	Log::UI::getConsoleView<Log::UI::NativeConsoleView>()->show();
#else
	Log::UI::getConsoleView<Log::UI::NativeConsoleView>()->hide();
#endif


	AutoTuner::Profiler::start();
	AutoTuner::LibraryInfo::printInfo();

	MainWindow w;
	w.show();

	int ret = 0;
#ifdef QT_ENABLED
	ret = app.exec();
#endif
	AutoTuner::Profiler::stop((std::string(AutoTuner::LibraryInfo::name) + ".prof").c_str());
	//MatlabAPI::MatlabEngine::terminate();
	return ret;
}



