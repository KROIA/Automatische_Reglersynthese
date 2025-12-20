#pragma once

#include "AutoTuner_base.h"
#include <QSplitter>
#include <QString>

namespace AutoTuner
{
	class ExperimentTab : public QSplitter
	{
	public:
		ExperimentTab(QTabWidget* parent, QWidget* propertyWidget, const QString& experimentName);
		virtual ~ExperimentTab();
		QWidget* propertiesWidget;
		QWidget* sceneWidget;
	};
}