#pragma once

#include "AutoTuner.h"
#include "scene/Objects/SystemOptimizer.h"

//#define MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION

class MotorWithMassIdentification : public QObject, public QSFML::Objects::GameObject
{
	Q_OBJECT
public:
	MotorWithMassIdentification();

	virtual ~MotorWithMassIdentification();


	void startOptimization();
	void stopOptimization();

	void setLearningRate(double rate)
	{
		if (m_solverObject)
		{
			m_solverObject->setMutationAmount(rate);
		}
	}

private:
	std::vector<SystemOptimizer::StimulusResponseData> loadStimulusResponseDataFromCSV(const std::string& filePath);
	class System : public AutoTuner::TunableTimeBasedSystem
	{
	public:
		System()
			: AutoTuner::TunableTimeBasedSystem()
		{
			
			//m_parameters = { 0.01, -0.01, -10, 1, 1, 1, 1 }; //, 0.5, 0.5, 0.5 };
			//m_parameters = { 0.049, -0.023, 64.93, -0.244, 10.4036, -1.055, -0.1875}; //, 0.5, 0.5, 0.5 };
#ifdef MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION
			m_parameters = { 0.0637851, -0.0232653, 189.219, -0.207233, 18.8808, -0.809018, -0.228862 };
			m_pidController.setParameters(-217, -0.1, -0.1);
			m_pidController.setOutputSaturationLimits(-3000, 3000);
			m_pidController.setIntegrationSolver(TimeBasedSystem::IntegrationSolver::Bilinear);
#else
			m_parameters = { 0.047655253638114625, -0.022366074453890505, 168.00561053145208, -0.22794969836288803, 20.287156326403448, -1.1410678184395731, -0.19561222218307175 };
#endif
			
			
			//m_parameters = { 0, 0, -1.14106781843957, 0, 0, 0, 0, -0.195612222183072, 38.2968282439173, 38.2968282439173, 0.227949698362888, 0,3408.35608456442, 3408.35608456442, 0, -20.2871563264034,
			//0,0,0,0, -0.0108630006922152, 0, 0, -0.453744048854055,
			//1,0,0,0,0,1,0,0,
			//0,0,0,0};

			m_inputs = { 0.0, 0.0};
			m_outputs = { 0.0, 0.0 };


			/*MatlabAPI::Matrix A({{0, 0, -1.14106781843957, 0},
								 {0, 0, 0, -0.195612222183072},
								 {38.2968282439173, 38.2968282439173, 0.227949698362888, 0},
								 {3408.35608456442, 3408.35608456442, 0, -20.2871563264034} });

			MatlabAPI::Matrix B({{0, 0},
								 {0, 0},
								 {-0.0108630006922152, 0},
								 {0, -0.453744048854055} });

			MatlabAPI::Matrix C({{1, 0, 0, 0},
								 {0, 1, 0, 0} });

			MatlabAPI::Matrix D({{0, 0},
								 {0, 0} });

			m_statespaceSystem.setMatrixA(A);
			m_statespaceSystem.setMatrixB(B);
			m_statespaceSystem.setMatrixC(C);
			m_statespaceSystem.setMatrixD(D);*/

			//AutoTuner::StatespaceSystem::SSData ssData(2, 2, 4);
			//
			//if(ssData.matricesData.size() == m_parameters.size())
			//	ssData.matricesData = m_parameters;
			//m_statespaceSystem.setSSData(ssData);
			//
			//m_statespaceSystem.setIntegrationSolver(IntegrationSolver::Bilinear);
		}
		System(const System& other)
			: AutoTuner::TunableTimeBasedSystem(other)
		{
			m_parameters = other.m_parameters;
			m_inputs = other.m_inputs;
			m_outputs = other.m_outputs;
			m_disk1 = other.m_disk1;
			m_disk2 = other.m_disk2;
			m_statespaceSystem = other.m_statespaceSystem;
#ifdef MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION
			m_pidController = other.m_pidController;
#endif
		}
		TimeBasedSystem* clone() override
		{
			return new System(*this);
		}

		void reset() override
		{
			for (size_t i = 0; i < m_inputs.size(); ++i)
				m_inputs[i] = 0;
			for (size_t i = 0; i < m_outputs.size(); ++i)
				m_outputs[i] = 0;
			m_disk1 = Disk();
			m_disk2 = Disk();
#ifdef MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION
			m_pidController.reset();
#endif
		}

		void setParameters(const std::vector<double>& params) override;
		std::vector<double> getParameters() const override { return m_parameters; }
		void setInputSignals(double u) override { m_inputs = { u, u }; }
		void setInputSignals(const std::vector<double>& u) override { m_inputs = u; }
		void setInputSignal(size_t input, double value) override { m_inputs[input] = value; }
		std::vector<double> getInputs() const override { return m_inputs; }
		std::vector<double> getOutputs() const override { return m_outputs; }
		double getOutput(size_t index) const override { return m_outputs[index]; }
		double getInput(size_t index) const override { return m_inputs[index]; }

		void update(double deltaTime) override;

	private:
		std::vector<double>	m_parameters;
		std::vector<double> m_inputs;
		std::vector<double> m_outputs;

		struct Disk
		{
			double angle = 0;
			double angularVelocity = 0;
			double lastPreIntegrationTorque = 0;
		};

		Disk m_disk1;
		Disk m_disk2;

		AutoTuner::StatespaceSystem m_statespaceSystem;
#ifdef MOTOR_WITH_SPRING_USE_CLOSED_LOOP_IDENTIFICATION
		AutoTuner::PID m_pidController;
#endif
	}; 



	SystemOptimizer* m_systemOptimizer = nullptr;
	AutoTuner::Solver* m_solverObject = nullptr;
	std::shared_ptr<System> m_systemModel;
};
