#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <iostream>
#include <QCloseEvent>
#include <QDebug>
#include <windows.h>
//#include "MatlabEngine.hpp"
//#define ENABLE_SCREEN_CAPTURE

using namespace QSFML;
using namespace QSFML::Objects;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	// Init resources
	Q_INIT_RESOURCE(AutoTunerIcons);
    //MatlabApiLoader::instance(); // ensures DLLs are loaded
    ui->setupUi(this);
	
	//MatlabAPI::MatlabEngine::instantiate(u"MySession");

	m_toolBar = new QToolBar(this);
	m_toolBar->setMovable(false);
	m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addToolBar(Qt::TopToolBarArea, m_toolBar);

	m_resetAction = new QAction(QIcon(":/icons/reset.png"), "Reset", this);
	m_toolBar->addAction(m_resetAction);
	
	connect(m_resetAction, &QAction::triggered, [this]() {
		std::shared_ptr<AutoTuner::Experiment> experiment = getCurrentExperiment();
		if (experiment)
			experiment->reset();
		});

	m_hitAction = new QAction(QIcon(":/icons/hit.png"), "Hit", this);
	m_toolBar->addAction(m_hitAction);
	connect(m_hitAction, &QAction::triggered, [this]() {
		std::shared_ptr<AutoTuner::Experiment> experiment = getCurrentExperiment();
		if (experiment)
			experiment->hit();
		});


	
	m_disturbanceButton = new QToolButton();
	m_disturbanceButton->setText("Disturbance");
	m_disturbanceButton->setToolTip("Disturbance");
	m_disturbanceButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_disturbanceButton->setIcon(QIcon(":/icons/wind.png"));	
	m_toolBar->addWidget(m_disturbanceButton);
	connect(m_disturbanceButton, &QToolButton::pressed, this, &MainWindow::onDisturbanceClicked);
	connect(m_disturbanceButton, &QToolButton::released, this, &MainWindow::onDisturbanceReleased);


    
	m_tabWidget = new QTabWidget(this);
	setCentralWidget(m_tabWidget);




    setupScenes();
    
}

MainWindow::~MainWindow()
{
    delete ui;
	MatlabAPI::MatlabEngine::terminate();
}

void MainWindow::setupScenes()
{
    addScene<BallOnBeam>();
}
std::shared_ptr<AutoTuner::Experiment> MainWindow::getCurrentExperiment() const
{
	int index = m_tabWidget->currentIndex();
	if (index < 0 || index >= static_cast<int>(m_scenes.size()))
		return nullptr;
	return m_scenes[index];
}
void MainWindow::closeEvent(QCloseEvent* event)
{
	m_scenes.clear();
    event->accept();
}
void MainWindow::onDisturbanceClicked()
{
	std::shared_ptr<AutoTuner::Experiment> experiment = getCurrentExperiment();
	if (experiment)
		experiment->disturbance(true);
}
void MainWindow::onDisturbanceReleased()
{
	std::shared_ptr<AutoTuner::Experiment> experiment = getCurrentExperiment();
	if (experiment)
		experiment->disturbance(false);
}
