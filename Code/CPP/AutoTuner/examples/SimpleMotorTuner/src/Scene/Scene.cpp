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
	


    m_dcMotorProblem = new DCMotorProblem();
	m_scene->addObject(m_dcMotorProblem);

	//m_dcMotorWithMassProblem = new DCMotorWithMassProblem();
	//m_scene->addObject(m_dcMotorWithMassProblem);
	
	//IntegrationTest* test = new IntegrationTest();
	//m_scene->addObject(test);

	//m_motorWithMassIdentification = new MotorWithMassIdentification();
	//m_scene->addObject(m_motorWithMassIdentification);
	//m_motorWithMassIdentification->startOptimization();

	m_scene->start();
	m_scene->applyObjectChanges();
	editor->getCamera()->getCamera()->setPosition({500.0f,500.0f});
}