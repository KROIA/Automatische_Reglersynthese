#include "scene/Objects/MotorWithMassIdentification.h"
#include <fstream>



MotorWithMassIdentification::MotorWithMassIdentification()
	: QObject()
	, QSFML::Objects::GameObject("MotorWithMassIdentification")
{
	
	m_systemOptimizer = new SystemOptimizer("SystemOptimizer");
	addChild(m_systemOptimizer);

	//m_solverObject = new AutoTuner::DifferentialEvolutionSolver();
	m_solverObject = new AutoTuner::GeneticSolver();
	m_solverObject->setMutationAmount(0.1);
	m_systemOptimizer->setSolverObject(m_solverObject);

	//std::vector<SystemOptimizer::StimulusResponseData> data = loadStimulusResponseDataFromCSV("simoutData4_filtered.csv");
	//std::vector<SystemOptimizer::StimulusResponseData> data = loadStimulusResponseDataFromCSV("simoutData4_filtered.csv");
#ifdef MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION
	std::vector<SystemOptimizer::StimulusResponseData> data = loadStimulusResponseDataFromCSV("../ClosedLoopSignals.csv");
#else
	std::vector<SystemOptimizer::StimulusResponseData> data = loadStimulusResponseDataFromCSV("../simoutData4_filtered.csv");
#endif
	//std::vector<SystemOptimizer::StimulusResponseData> data = loadStimulusResponseDataFromCSV("ramp.csv");
	m_systemOptimizer->setStimulusResponseDataCollection(data);

	m_systemModel = std::make_shared<System>();
	m_systemOptimizer->setModel(m_systemModel);
}

MotorWithMassIdentification::~MotorWithMassIdentification()
{

}



void MotorWithMassIdentification::startOptimization()
{
	double areaMin = 0;
	double areaMax = 1.0;

	std::vector<std::vector<double>> initialPopulation;
	std::vector<double> initialParams = m_systemOptimizer->getInitialParameters();
	for (size_t i = 0; i < 50; ++i)
	{
		std::vector<double> individual;
		for (size_t j=0; j< initialParams.size(); ++j)
		{
			double value = initialParams[j];//  *AutoTuner::Solver::getRandomDouble(areaMin, areaMax);
			//double value = AutoTuner::Solver::getRandomDouble(areaMin, areaMax);

			individual.push_back(value);
		}
		initialPopulation.push_back(individual);
	}
	
	m_systemOptimizer->startOptimization(initialPopulation);
}
void MotorWithMassIdentification::stopOptimization()
{
	m_systemOptimizer->stopOptimization();
}



void MotorWithMassIdentification::System::setParameters(const std::vector<double>& params) 
{ 
	m_parameters = params; 
	//AutoTuner::StatespaceSystem::SSData ssData(2, 2, 4);
	//ssData.matricesData = m_parameters;
	//m_statespaceSystem.setSSData(ssData);
}
void MotorWithMassIdentification::System::update(double deltaTime)
{

	//m_statespaceSystem.setInputSignals(m_inputs);
	//m_statespaceSystem.update(deltaTime);
	//m_outputs = m_statespaceSystem.getOutputs();
#ifdef MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION
	double error = m_inputs[0] - m_outputs[1];

	m_pidController.setInput(error);
	m_pidController.update(deltaTime);
	double pidOutput = m_pidController.getOutput();

	double input1Scaled = pidOutput * m_parameters[0];
#else
	double input1Scaled = m_inputs[0] * m_parameters[0];
#endif
	double input2Scaled = m_inputs[1] * m_parameters[1];

	double angleDifferenceTorque = (m_disk1.angle + m_disk2.angle) * m_parameters[2];

	double torque1 = (input1Scaled  -m_disk1.angularVelocity - angleDifferenceTorque) * m_parameters[3];
	double torque2 = (input2Scaled  -m_disk2.angularVelocity  + angleDifferenceTorque) * m_parameters[4];

	double lastAngularVelocity1 = m_disk1.angularVelocity * m_parameters[5];
	double lastAngularVelocity2 = m_disk2.angularVelocity * m_parameters[6];
	m_disk1.angularVelocity = TimeBasedSystem::getIntegrated_Bilinear(m_disk1.lastPreIntegrationTorque, torque1, m_disk1.angularVelocity, deltaTime);
	m_disk2.angularVelocity = TimeBasedSystem::getIntegrated_Bilinear(m_disk2.lastPreIntegrationTorque, torque2, m_disk2.angularVelocity, deltaTime);

	
	m_disk1.angle = TimeBasedSystem::getIntegrated_Bilinear(lastAngularVelocity1, m_disk1.angularVelocity * m_parameters[5], m_disk1.angle, deltaTime);
	m_disk2.angle = TimeBasedSystem::getIntegrated_Bilinear(lastAngularVelocity2, m_disk2.angularVelocity * m_parameters[6], m_disk2.angle, deltaTime);
	m_outputs[0] = m_disk1.angle;
	m_outputs[1] = m_disk2.angle;


	m_disk1.lastPreIntegrationTorque = torque1;
	m_disk2.lastPreIntegrationTorque = torque2;
}


std::vector<SystemOptimizer::StimulusResponseData> MotorWithMassIdentification::loadStimulusResponseDataFromCSV(const std::string& filePath)
{

    std::vector<SystemOptimizer::StimulusResponseData> data;
	// First column: timestamp
	// Next columns: inputs with "u1", "u2", ... and outputs with "y1", "y2", ...
	const char delimiter = ';';
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return data;
	}

	std::string line;
	// Read header
	std::getline(file, line);
	std::vector<std::string> headers;
	{
		size_t start = 0;
		size_t end = line.find(delimiter);
		while (end != std::string::npos)
		{
			headers.push_back(line.substr(start, end - start));
			start = end + 1;
			end = line.find(delimiter, start);
		}
		headers.push_back(line.substr(start));
	}

	// Identify input and output columns
	std::vector<size_t> inputIndices;
	std::vector<size_t> outputIndices;
	for (size_t i = 1; i < headers.size(); ++i)
	{
		if (headers[i].rfind("u", 0) == 0)
		{
			inputIndices.push_back(i);
		}
		else if (headers[i].rfind("y", 0) == 0)
		{
			outputIndices.push_back(i);
		}
	}
	// Read data
	double previousTime = 0.0;
	double averageDeltaTime = 0.0;
	SystemOptimizer::StimulusResponseData currentData;
	while (std::getline(file, line))
	{
		size_t start = 0;
		size_t end = line.find(delimiter);
		std::vector<std::string> values;
		while (end != std::string::npos)
		{
			values.push_back(line.substr(start, end - start));
			start = end + 1;
			end = line.find(delimiter, start);
		}
		values.push_back(line.substr(start));
		if (values.size() != headers.size())
			continue;
		double currentTime = std::stod(values[0]);
		if (currentData.inputSignal.empty())
		{
			currentData.deltaTime = currentTime - previousTime;
			averageDeltaTime += currentData.deltaTime;
			
		}
		// Read inputs
		for (size_t idx : inputIndices)
		{
			currentData.inputSignal.push_back(std::stod(values[idx]));
		}
		// Read outputs
		for (size_t idx : outputIndices)
		{
			currentData.outputSignal.push_back(std::stod(values[idx]));
		}
		previousTime = currentTime;
		data.push_back(currentData);
		currentData.inputSignal.clear();
		currentData.outputSignal.clear();
	}
	averageDeltaTime /= data.size();
	data[0].deltaTime = averageDeltaTime;

	file.close();
    return data;
}