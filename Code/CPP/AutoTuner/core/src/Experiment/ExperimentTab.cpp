#include "Experiment/ExperimentTab.h"


namespace AutoTuner
{
	ExperimentTab::ExperimentTab(QTabWidget* parent, 
								 QWidget* propertyWidget, 
								 const QString& experimentName)
		: QSplitter(parent)
		, propertiesWidget(propertyWidget)
		, sceneWidget(nullptr)
	{
		QSplitter::setOrientation(Qt::Horizontal);
		QSplitter::setChildrenCollapsible(false);
		QSplitter::setHandleWidth(4);
		QSplitter::setContentsMargins(0, 0, 0, 0);
		QSplitter::setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		sceneWidget = new QWidget(this);
		sceneWidget->setMinimumSize(200, 200);
		sceneWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sceneWidget->setContentsMargins(0, 0, 0, 0);
		sceneWidget->setAttribute(Qt::WA_OpaquePaintEvent);
		sceneWidget->setAttribute(Qt::WA_NoSystemBackground);
		sceneWidget->setUpdatesEnabled(false); // Prevents flickering on resize

		QSplitter::addWidget(propertiesWidget);
		QSplitter::addWidget(sceneWidget);
		QSplitter::setSizes({ 100, 600 });

		parent->addTab(this, experimentName);
	}
	ExperimentTab::~ExperimentTab()
	{}
}