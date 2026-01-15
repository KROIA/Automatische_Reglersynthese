#include "Scene/Scene.h"

using QSFML::Objects::GameObject;

Scene::Scene()
{

}
Scene::~Scene()
{

}

void Scene::setupScene(QWidget* parent)
{
	QSFML::SceneSettings settings;
	settings.timing.frameTime = 0;
	settings.timing.physicsFixedDeltaT = 0.01;
	m_scene = new QSFML::Scene(parent, settings);

	QSFML::Objects::DefaultEditor* editor = new QSFML::Objects::DefaultEditor("DefaultEditor", {1000,1000});
	m_scene->addObject(editor);
	


//    m_dcMotorProblem = new DCMotorProblem();
//	m_scene->addObject(m_dcMotorProblem);
	createDCMotorProblem();
	//createDCMotorWithMassProblem();
	
	//IntegrationTest* test = new IntegrationTest();
	//m_scene->addObject(test);

	//m_motorWithMassIdentification = new MotorWithMassIdentification();
	//m_scene->addObject(m_motorWithMassIdentification);
	//m_motorWithMassIdentification->startOptimization();

	m_scene->start();
	m_scene->applyObjectChanges();
	editor->getCamera()->getCamera()->setPosition({500.0f,500.0f});
}


void Scene::createDCMotorWithMassProblem()
{
	createDCMotorWithMassProblem(getDefaultDCMotorWithMassSettings());
}
void Scene::createDCMotorWithMassProblem(PIDTuningProblem::SetupSettings settings)
{
	destroyDCMotorWithMassProblem();
	destroyDCMotorProblem();
	m_dcMotorWithMassProblem = new DCMotorWithMassProblem(settings);
	m_scene->addObject(m_dcMotorWithMassProblem);
}
PIDTuningProblem::SetupSettings Scene::getDefaultDCMotorWithMassSettings() const
{
	PIDTuningProblem::SetupSettings settings;
	settings.useGeneticMutationRateDecay = true;
	settings.useMinimizingScore = true;
	settings.useKn = true;

	settings.solverType = PIDTuningProblem::SolverType::DifferentialEvolution;
	settings.disableErrorIntegrationWhenSaturated = false; // 

	// Optimization parameters
	settings.optimizeKp = true;
	settings.optimizeKi = true;
	settings.optimizeKd = true;
	settings.optimizeKn = true;
	settings.optimizeIntegralSaturation = false;
	settings.optimizeAntiWindupBackCalculationConstant = false;



	settings.defaultKp = 0.0;
	settings.defaultKi = 0.0;
	settings.defaultKd = 0.0;
	settings.defaultKn = 0.0;
	settings.defaultPIDISaturation = 100000000;
	settings.defaultPIDAntiWindupMethod = AutoTuner::PID::AntiWindupMethod::Clamping;
	settings.defaultPIDIntegrationSolver = AutoTuner::PID::IntegrationSolver::ForwardEuler;

	settings.defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Filtered;
	//AutoTuner::PID::DerivativeType defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Unfiltered;

	settings.defaultPIDAntiWindupBackCalculationConstant = 0.1;

	settings.startAreaRange = 1;
	settings.agentCount = 30;

	settings.endTime = 50;
	settings.deltaTime = 0.01;
	settings.targetEpochs = 1000;

	settings.nyquistBeginFreq = 0.01;
	settings.nyquistEndFreq = 20;
	settings.targetPhaseMargin = M_PI / 4; // degrees
	settings.targetGainMargin = 2;


	if (settings.solverType == PIDTuningProblem::SolverType::GeneticAlgorithm)
	{
		settings.startLearningRate = 0.1;
		settings.learningRateDecay = 0.999; // per generation
	}
	else
	{
		settings.startLearningRate = 0.5;
		settings.learningRateDecay = 0.999; // per generation
	}
	return settings;
}
void Scene::createDCMotorProblem()
{
	createDCMotorProblem(getDefaultDCMotorSettings());
}
void Scene::createDCMotorProblem(PIDTuningProblem::SetupSettings settings)
{
	destroyDCMotorProblem();
	destroyDCMotorWithMassProblem();
	m_dcMotorProblem = new DCMotorProblem(settings);
	m_scene->addObject(m_dcMotorProblem);
}
PIDTuningProblem::SetupSettings Scene::getDefaultDCMotorSettings() const
{
	PIDTuningProblem::SetupSettings settings;
	settings.useGeneticMutationRateDecay = true;
	settings.useMinimizingScore = true;
	settings.useKn = true;

	settings.solverType = PIDTuningProblem::SolverType::GeneticAlgorithm;
	settings.disableErrorIntegrationWhenSaturated = true;

	// Optimization parameters
	settings.optimizeKp = true;
	settings.optimizeKi = true;
	settings.optimizeKd = true;
	settings.optimizeKn = true;
	settings.optimizeIntegralSaturation = false;
	settings.optimizeAntiWindupBackCalculationConstant = false;



	settings.defaultKp = 1.0;
	settings.defaultKi = 1.0;
	settings.defaultKd = 1.0;
	settings.defaultKn = 1.0;
	settings.defaultPIDISaturation = 10;
	settings.defaultPIDAntiWindupMethod = AutoTuner::PID::AntiWindupMethod::Clamping;
	settings.defaultPIDIntegrationSolver = AutoTuner::PID::IntegrationSolver::ForwardEuler;

	settings.defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Filtered;
	//AutoTuner::PID::DerivativeType defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Unfiltered;

	settings.defaultPIDAntiWindupBackCalculationConstant = 0.1;

	settings.startAreaRange = 10;
	settings.agentCount = 30;

	settings.endTime = 20;
	settings.deltaTime = 0.01;
	settings.targetEpochs = 5000;

	settings.nyquistBeginFreq = 0.1;
	settings.nyquistEndFreq = 1000;
	settings.targetPhaseMargin = M_PI / 2; // degrees
	settings.targetGainMargin = 2;

	if (settings.solverType == PIDTuningProblem::SolverType::GeneticAlgorithm)
	{
		settings.startLearningRate = 0.1;
		settings.learningRateDecay = 0.999; // per generation
	}
	else
	{
		settings.startLearningRate = 0.5;
		settings.learningRateDecay = 0.999; // per generation
	}
	return settings;
}
void Scene::destroyDCMotorProblem()
{
	if (m_dcMotorProblem)
	{
		m_scene->deleteObject(m_dcMotorProblem);
		//delete m_dcMotorProblem;
		m_dcMotorProblem = nullptr;
	}
}
void Scene::destroyDCMotorWithMassProblem()
{
	if (m_dcMotorWithMassProblem)
	{
		m_scene->deleteObject(m_dcMotorWithMassProblem);
		//delete m_dcMotorWithMassProblem;
		m_dcMotorWithMassProblem = nullptr;
	}
}