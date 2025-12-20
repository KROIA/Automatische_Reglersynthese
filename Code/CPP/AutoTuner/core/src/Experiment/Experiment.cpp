#include "Experiment/Experiment.h"


namespace AutoTuner
{
	Experiment::Experiment(const QString& sceneName)
		: m_experimentTab(nullptr)
		, m_scene(nullptr)
		, m_rootContainer(nullptr)
		, m_sceneName(sceneName)
		, m_logger(sceneName.toStdString())
	{
		m_scope = new Scope("Scope", m_timeData, 1000);
	}

	Experiment::~Experiment()
	{
		disconnect(m_rootContainer, &QTabWidget::currentChanged, this, &Experiment::onTabChanged);
		m_scene->stop();
		delete m_scene;
		m_scene = nullptr;
		delete m_experimentTab;
	}

	void Experiment::setup(QTabWidget* rootContainer) 
	{
		m_rootContainer = rootContainer;
		m_experimentTab = new ExperimentTab(rootContainer, createPropertyWidget(), m_sceneName);


		QSFML::SceneSettings settings;
		settings.layout.fixedSize = sf::Vector2u(300, 100);
		settings.contextSettings.antialiasingLevel = 8;
		settings.timing.frameTime = 0.01;

		settings.timing.physicsFixedDeltaT = 0.0001;
		settings.updateControlls.enableMultithreading = false;
		settings.updateControlls.threadSettings.threadCount = 32;
		settings.updateControlls.threadSettings.objectGroups = 64;

		m_scene = new QSFML::Scene(m_experimentTab->sceneWidget, settings);
		sf::Vector2f editorSize(1000, 1000);
		QSFML::Objects::DefaultEditor* defaultEditor = new QSFML::Objects::DefaultEditor("Editor", editorSize);
		defaultEditor->setPosition(-editorSize * 0.5f);
		m_scene->addObject(defaultEditor);


		m_sceneUpdater = new SceneUpdateObj(this);
		m_scene->addObject(m_sceneUpdater);

		m_scene->applyObjectChanges();

		onSetup(m_scene);
		connect(rootContainer, &QTabWidget::currentChanged, this, &Experiment::onTabChanged);
	}
	void Experiment::setScopePointCount(size_t count)
	{
		m_scope->setPointCount(count);
	}
	void Experiment::setScopeCaptureTickInterval(size_t ticks)
	{
		m_scopeCaptureTickInterval = ticks;
	}

	void Experiment::start()
	{
		m_timeData.clear();
		m_scope->clearData();
		m_startTime = m_sceneUpdater->getAge();
		m_currentTick = 0;
		if (m_scene)
			m_scene->start();
		onStart();
	}
	void Experiment::stop()
	{
		if (m_scene)
			m_scene->stop();
		m_scope->clearData();
		onStop();
		
	}

	void Experiment::reset()
	{
		m_timeData.clear();
		m_scope->clearData();
		m_startTime = m_sceneUpdater->getAge();
		onReset();
	}
	void Experiment::hit()
	{
		onHit();
	}
	void Experiment::disturbance(bool enable)
	{
		m_disturbanceEnabled = enable;
	}

	void Experiment::registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName)
	{
		m_scope->registerSignal(series, plotName, seriesName);
	}
	void Experiment::registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName, const ImVec4& color, float thickness)
	{
		m_scope->registerSignal(series, plotName, seriesName, color, thickness);
	}

	void Experiment::onTabChanged(int index)
	{
		if (m_rootContainer->widget(index) != m_experimentTab)
		{
			if (m_experimentTab && m_experimentTab->sceneWidget)
			{
				stop();
			}
			return;
		}
		else
		{
			if (m_experimentTab && m_experimentTab->sceneWidget)
			{
				start();
			}
		}
	}

	void Experiment::update()
	{
		if (m_disturbanceEnabled)
			onDisturbance();
		onSceneUpdate(m_scene);

		++m_currentTick;
		if (m_currentTick % m_scopeCaptureTickInterval == 0)
		{
			onScopeCapture();
			m_timeData.append(m_sceneUpdater->getAge() - m_startTime);
		}

		
		m_scope->drawPlot();
	}
}