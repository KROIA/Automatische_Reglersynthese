#pragma once


#include "AutoTuner.h"
#include "Systems/DCMotorWithMassSystem.h"
#include "scene/Objects/SystemOptimizer.h"
#include "scene/Objects/PIDTuningProblem.h"
#include <QObject>

//#define DYNAMIC_STEP_SEQUENCE
#define GENETIC_USE_MUTATION_RATE_DECAY
#define GENETIC_USE_MINIMIZING_SCORE

// Solver
//#define USE_GENTIC_SOLVER
#define USE_DIFFERENTIAL_EVOLUTION_SOLVER

// Scoring settings
//#define DISABLE_ERROR_INTEGRAL_WHEN_SATURATED


// Tunable parameters
#define PARAMETERLIST_ENABLE_KP 
#define PARAMETERLIST_ENABLE_KI
#define PARAMETERLIST_ENABLE_KD
#define PARAMETERLIST_ENABLE_KN
//#define PARAMETERLIST_ENABLE_INTEGRAL_SATURATION
//#define PARAMETERLIST_ENABLE_ANTI_WINDUP_BACK_CALCULATION_CONSTANT

#define PID_USE_KN


#ifndef PID_USE_KN
#undef PARAMETERLIST_ENABLE_KN
#endif




class DCMotorWithMassProblem: public QObject, public PIDTuningProblem
{
	Q_OBJECT
public:
	// Default parameters
	static constexpr double s_defaultKp = 0.0;
	static constexpr double s_defaultKi = 0.0;
	static constexpr double s_defaultKd = 0.0;
	static constexpr double s_defaultKn = 0.0;
	static constexpr double s_defaultPIDISaturation = 100000000.0;
	static constexpr AutoTuner::PID::AntiWindupMethod s_defaultPIDAntiWindupMethod = AutoTuner::PID::AntiWindupMethod::Clamping;
	static constexpr AutoTuner::PID::IntegrationSolver s_defaultPIDIntegrationSolver = AutoTuner::PID::IntegrationSolver::ForwardEuler;
#ifdef PARAMETERLIST_ENABLE_KN
	static constexpr AutoTuner::PID::DerivativeType s_defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Filtered;
#else
	static constexpr AutoTuner::PID::DerivativeType s_defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Unfiltered;
#endif
	static constexpr double s_defaultPIDAntiWindupBackCalculationConstant = 0.1;

	static constexpr double s_startAreaRange = 1;
	static constexpr size_t s_agentCount = 30;
	//static constexpr size_t s_testRuns = 1;
	static constexpr double s_endTime = 50;
	static constexpr double s_deltaTime = 0.01;
	static constexpr size_t s_targetEpochs = 1000;

	static constexpr double m_nyquistBeginFreq = 0.01;
	static constexpr double m_nyquistEndFreq = 20;
	static constexpr double m_targetPhaseMargin = M_PI/4; // degrees
	static constexpr double m_targetGainMargin = 2; 

#ifdef USE_GENTIC_SOLVER
	static constexpr double s_startLearningRate = 0.1;
	static constexpr double s_learningRateDecay = 0.999; // per generation
#else
	static constexpr double s_startLearningRate = 0.5;
#endif
	DCMotorWithMassProblem(const std::string& name = "DCMotorWithMassProblem",
				  GameObject* parent = nullptr);

	void onAwake() override;
	void precalculatePIDWithZieglerNichols() override;
	void setupPopulation(size_t populationSize, double kp, double ki, double kd, double integralSatturation, double areaRange = 1) override;

	void update() override;

	void createNewStepSequence() override;
	void resetPopulation() override;
	void testBestAgent() override;
	void setLearningAmount(double learningAmount) override;
	double getLearningAmount() const override;
	void setLearningRate(double learningRate) override;
	double getLearningRate() const override;

	void saveResultsToFile(const std::string& resultName) override;
	void setResultsLearningHistoryColors(const sf::Color& color) override;
	void setResultsLearningHistoryColors(const sf::Color& averageColor, const sf::Color& bestColor, const sf::Color& worstColor) override;
	void setResultsParameterColors(const sf::Color& color) override;
	void setResultsParameterColors(const std::vector<sf::Color>& colors) override;
	size_t getParameterCount() const override;
	void setResultsParameterEnabled(size_t index, bool enabled) override;
	void setResultsLearningHistoryEnableColumn(bool enableMin, bool enableAverage, bool enableMax) override;
	size_t getResultsStepResponseSingalsCount() const override;
	void setResultsStepResponseSignalColor(size_t index, const sf::Color& color) override;
	void setResultsStepResponseSignalName(size_t index, const std::string& name) override;
	void setResultsStepResponseSignalLineStyle(size_t index, AutoTuner::CSVExport::LineStyle style) override;
	void setResultsStepResponseSignalLineThickness(size_t index, int thickness) override;
	void setResultsStepResponseSignalEnabled(size_t index, bool enabled) override;
	const sf::Color& getResultsStepResponseSignalColor(size_t index) const override;
	const std::string& getResultsStepResponseSignalName(size_t index) const override;
	const AutoTuner::CSVExport::LineStyle getResultsStepResponseSignalLineStyle(size_t index) const override;
	int getResultsStepResponseSignalLineThickness(size_t index) const override;
	bool isResultsStepResponseSignalEnabled(size_t index) const override;

	std::vector<double> getBestParameters() const override;
	size_t getPopulationSize() const override
	{
		return m_populationSize;
	}

	void setTuningGoalParameters(double errorIntegralWeight,
		double actuatorEffortWeight,
		double overshootWeight) override
	{
		m_tuningGoalFactor_errorIntegral = errorIntegralWeight;
		m_tuningGoalFactor_actuatorEffort = actuatorEffortWeight;
		m_tuningGoalFactor_overshoot = overshootWeight;
	}
	void setTuningGoalParameter_ErrorIntegralWeight(double weight) override
	{
		m_tuningGoalFactor_errorIntegral = weight;
	}
	void setTuningGoalParameter_ActuatorEffortWeight(double weight) override
	{
		m_tuningGoalFactor_actuatorEffort = weight;
	}
	void setTuningGoalParameter_OvershootWeight(double weight) override
	{
		m_tuningGoalFactor_overshoot = weight;
	}
	void setTuningGoalParameter_GainMarginWeight(double weight) override 
	{
		m_tuningGoalFactor_gainMargin = weight;
	}
	void setTuningGoalParameter_PhaseMarginWeight(double weight) 
	{
		m_tuningGoalFactor_phaseMargin = weight;
	}
	double getTuningGoalParameter_ErrorIntegralWeight() const override
	{
		return m_tuningGoalFactor_errorIntegral;
	}
	double getTuningGoalParameter_ActuatorEffortWeight() const override
	{
		return m_tuningGoalFactor_actuatorEffort;
	}
	double getTuningGoalParameter_OvershootWeight() const override
	{
		return m_tuningGoalFactor_overshoot;
	}
	double getTuningGoalParameter_GainMarginWeight() const override
	{ 
		return m_tuningGoalFactor_gainMargin;
	}
	double getTuningGoalParameter_PhaseMarginWeight() const override
	{ 
		return m_tuningGoalFactor_phaseMargin;
	}
	void enableLearningRateDecay(bool enable) override
	{
		m_useLearningRateDecay = enable;
	}
	bool isLearningRateDecayEnabled() const override
	{
		return m_useLearningRateDecay;
	}


	void testPID(const std::vector<double>& params, ResultData* resultContainer = nullptr) override;

	std::vector<double> agentTestFunction(const std::vector<double>& parameters, size_t agent) override;

	void printSignalSequenceToConsole(const std::string& name, const std::vector<sf::Vector2<double>>& steps) const override;

	void setTargetEpoch(size_t epoch) override
	{
		m_targetEpochs = epoch;
	}
	signals:

private:
	class TestSystem : public AutoTuner::TunableTimeBasedSystem
	{
	public:
		class FeedForwardPart : public AutoTuner::TimeBasedSystem
		{
		public:
			FeedForwardPart()
				: AutoTuner::TimeBasedSystem()
			{
			}
			FeedForwardPart(const FeedForwardPart& other)
				: AutoTuner::TimeBasedSystem(other),
				m_pidController(other.m_pidController),
				m_motorWithMass(other.m_motorWithMass),
				m_pidOutputValue(other.m_pidOutputValue)
			{
			}
			virtual TimeBasedSystem* clone() override
			{
				return new FeedForwardPart(*this);
			}
			void reset() override {
				m_pidController.reset();
				m_motorWithMass.reset();
				m_pidOutputValue = 0;
			}

			void setInputSignals(double u) override {
				m_pidController.setInput(u);
				m_motorWithMass.setInputs(1, 0);
			}
			void setInputSignal(size_t input, double value) override {
				if (input == 0)
					m_pidController.setInput(value);
				else if (input == 1)
					m_motorWithMass.setInputs(1, value);
			}
			void setInputSignals(const std::vector<double>& u) override {
				if (u.size() >= 2)
				{
					m_pidController.setInput(u[0]);
					m_motorWithMass.setInputs(1, u[1]);
				}
			}

			void update(double deltaTime) override {
				m_pidController.update(deltaTime);
				m_pidOutputValue = m_pidController.getOutput();
				m_motorWithMass.setInputSignal(0, m_pidOutputValue);
				m_motorWithMass.update(deltaTime);
			}

			std::vector<double> getInputs() const override {
				return { m_pidController.getInput(0), m_motorWithMass.getInput(1) };
			}
			std::vector<double> getOutputs() const override {
				return m_motorWithMass.getOutputs();
			}

			double getOutput(size_t index) const override { return m_motorWithMass.getOutput(index); }
			double getInput(size_t index) const override { 
				if(index==0)
					return m_pidController.getInput(0);
				return m_motorWithMass.getInput(index);
			}


			AutoTuner::PID m_pidController;
			DCMotorWithMassSystem m_motorWithMass;
			double m_pidOutputValue = 0.0;
		private:

		};
		TestSystem()
			: AutoTuner::TunableTimeBasedSystem()
		{
			m_feedForwardPart.m_motorWithMass.setIntegrationSolver(AutoTuner::TimeBasedSystem::IntegrationSolver::Bilinear);
			m_feedForwardPart.m_pidController.setIntegrationSolver(s_defaultPIDIntegrationSolver); // Matlab uses forward euler: 1/s = Ts/(z-1)
			m_feedForwardPart.m_pidController.setOutputSaturationLimits(-m_actuatorInputLimit, m_actuatorInputLimit);
			m_feedForwardPart.m_pidController.setAntiWindupMethod(s_defaultPIDAntiWindupMethod);
			m_feedForwardPart.m_pidController.setDerivativeType(s_defaultPIDDerivativeType);
		
			reset();
		}
		TestSystem(const TestSystem& other)
			: AutoTuner::TunableTimeBasedSystem(other),
			m_referenceValue(other.m_referenceValue),
			m_disturbanceValue(other.m_disturbanceValue),
			m_errorValue(other.m_errorValue),
			m_feedForwardPart(other.m_feedForwardPart),
			m_systemInputLimit(other.m_systemInputLimit),
			m_actuatorInputLimit(other.m_actuatorInputLimit)
		{
		}
		virtual TimeBasedSystem* clone() override
		{
			return new TestSystem(*this);
		}

		void reset() override
		{
			m_referenceValue = 0;
			m_disturbanceValue = 0;
			m_errorValue = 0;
			m_feedForwardPart.reset();
			
		}
		//void setPIDParameters(double kp, double ki, double kd)
		//{
		//	m_pidController.setParameters(kp, ki, kd);
		//}
		//void setPIDParameters(double kp, double ki, double kd, double kn)
		//{
		//	m_pidController.setParameters(kp, ki, kd, kn);
		//}
		//void setPIDIntegralSatturation(double limit)
		//{
		//	m_pidController.setIntegralSatturationLimit(limit);
		//}
		double getPIDKp() const
		{
			return m_feedForwardPart.m_pidController.getKp();
		}
		double getPIDKi() const
		{
			return m_feedForwardPart.m_pidController.getKi();
		}
		double getPIDKd() const
		{
			return m_feedForwardPart.m_pidController.getKd();
		}
		double getPIDKn() const
		{
			return m_feedForwardPart.m_pidController.getKn();
		}
		void setParameters(const std::vector<double>& params) override
		{
			//if (params.size() >= 3)
			//{
			//	m_pidController.setParameters(params[0], params[1], params[2]);
			//	m_pidController.setIntegralSatturationLimit(10);
			//}
			size_t counter = 0;
#ifdef PARAMETERLIST_ENABLE_KP
			double p = params[counter++];
#else
			double p = s_defaultKp;
#endif	
#ifdef PARAMETERLIST_ENABLE_KI
			double i = params[counter++];
#else
			double i = s_defaultKi;
#endif
#ifdef PARAMETERLIST_ENABLE_KD
			double d = params[counter++];
#else
			double d = s_defaultKd;
#endif
#ifdef PID_USE_KN
#ifdef PARAMETERLIST_ENABLE_KN
			double n = params[counter++];
#else
			double n = s_defaultKn;
#endif
			m_feedForwardPart.m_pidController.setParameters(p, i, d, n);
#else
			m_feedForwardPart.m_pidController.setParameters(p, i, d);
#endif
			
			

#ifdef PARAMETERLIST_ENABLE_INTEGRAL_SATURATION
			m_feedForwardPart.m_pidController.setIntegralSatturationLimit(params[counter++]);
#else
			m_feedForwardPart.m_pidController.setIntegralSatturationLimit(s_defaultPIDISaturation);
#endif

#ifdef PARAMETERLIST_ENABLE_ANTI_WINDUP_BACK_CALCULATION_CONSTANT
			m_feedForwardPart.m_pidController.setAntiWindupBackCalculationConstant(params[counter++]);
#else
			m_feedForwardPart.m_pidController.setAntiWindupBackCalculationConstant(s_defaultPIDAntiWindupBackCalculationConstant);
#endif
			
		}

		std::vector<double> getParameters() const override
		{
			std::vector<double> params;
#ifdef PARAMETERLIST_ENABLE_KP
			params.push_back(m_feedForwardPart.m_pidController.getKp());
#endif
			#ifdef PARAMETERLIST_ENABLE_KI
			params.push_back(m_feedForwardPart.m_pidController.getKi());
#endif
			#ifdef PARAMETERLIST_ENABLE_KD
			params.push_back(m_feedForwardPart.m_pidController.getKd());
#endif
			#ifdef PARAMETERLIST_ENABLE_KN
			params.push_back(m_feedForwardPart.m_pidController.getKn());
#endif
			#ifdef PARAMETERLIST_ENABLE_INTEGRAL_SATURATION
			params.push_back(m_feedForwardPart.m_pidController.getIntegralSatturationLimit());
#endif
			#ifdef PARAMETERLIST_ENABLE_ANTI_WINDUP_BACK_CALCULATION_CONSTANT
			params.push_back(m_feedForwardPart.m_pidController.getAntiWindupBackCalculationConstant());
#endif
			return params;
		}


		void setInputSignals(double u) override
		{
			m_referenceValue = u;
			m_disturbanceValue = u;
		}
		void setInputSignals(const std::vector<double>& u) override
		{
			if (u.size() >= 2)
			{
				m_referenceValue = u[0];
				m_disturbanceValue = u[1];
			}
		}
		void setInputSignal(size_t input, double value) override
		{
			if (input == 0)
				m_referenceValue = value;
			else if (input == 1)
				m_disturbanceValue = value;
		}
		void setInputSignals(double referenceValue, double disturbanceValue)
		{
			m_referenceValue = referenceValue;
			m_disturbanceValue = disturbanceValue;
		}
		void setReferenceInput(double referenceValue)
		{
			m_referenceValue = std::min(std::max(0.0, referenceValue), m_systemInputLimit);
		}
		double getReferenceInput() const
		{
			return m_referenceValue;
		}
		void setDisturbanceInput(double disturbanceValue)
		{
			m_disturbanceValue = disturbanceValue;
		}
		double getDisturbanceInput() const
		{
			return m_disturbanceValue;
		}

		double getOutput(size_t index) const override
		{
			return m_feedForwardPart.m_motorWithMass.getOutputs()[index];
		}
		double getInput(size_t index) const override
		{
			if(index == 0)
				return m_referenceValue;
			else if(index == 1)
				return m_disturbanceValue;
			return 0.0;
		}


		void update(double deltaTime) override
		{
			double y = m_feedForwardPart.m_motorWithMass.getOutputs()[1];	// y(t)
			double measurementNoise = AutoTuner::Solver::getRandomDouble(-1, 1) * 0.1;
			m_errorValue = m_referenceValue - y + measurementNoise;				// e(t) = r(t) - y(t)

			m_feedForwardPart.setInputSignal(0, m_errorValue);
			m_feedForwardPart.setInputSignal(1, m_disturbanceValue);
			m_feedForwardPart.update(deltaTime);
			//m_pidController.setInput(m_errorValue);
			//m_pidController.update(deltaTime);
			//m_pidOutputValue = m_pidController.getOutput();	    // u(t)

			//m_motorWithMass.setInputs(m_pidOutputValue, m_disturbanceValue);
			//m_motorWithMass.update(deltaTime);
		}

		std::vector<double> getInputs() const override
		{
			return { m_referenceValue };
		}
		std::vector<double> getOutputs() const override
		{
			return m_feedForwardPart.m_motorWithMass.getOutputs();
		}
		double getError() const
		{
			return m_errorValue;
		}
		double getPIDOutput() const
		{
			return m_feedForwardPart.m_pidOutputValue;
		}
		double getOutput() const
		{
			return m_feedForwardPart.m_motorWithMass.getOutputs()[1];
		}

		double getActuatorInputLimit() const
		{
			return m_actuatorInputLimit;
		}
		void setActuatorInputLimit(double limit)
		{
			m_actuatorInputLimit = limit;
		}
		double getSystemInputLimit() const
		{
			return m_systemInputLimit;
		}
		void setSystemInputLimit(double limit)
		{
			m_systemInputLimit = limit;
		}

		const AutoTuner::PID& getPIDController() const
		{
			return m_feedForwardPart.m_pidController;
		}
		const DCMotorWithMassSystem& getDCMotorSystem() const
		{
			return m_feedForwardPart.m_motorWithMass;
		}
		FeedForwardPart& getFeedForwardPart()
		{
			return m_feedForwardPart;
		}
	private:

		double m_referenceValue = 0.0;
		double m_disturbanceValue = 0.0;

		double m_errorValue = 0.0;
		

		double m_actuatorInputLimit = 10;
		double m_systemInputLimit = 10;
	

		FeedForwardPart m_feedForwardPart;
	};

	std::vector<sf::Vector2<double>> generateRandomStepSequence(double stepAmplitude, double maxTime, double minStepDuration, double maxStepDuration, size_t stepCount) override;
	
	void setCSVHeader() override;
	void logCSVData() override;
	void testCustomPID() override;


	AutoTuner::ZieglerNichols* m_zieglerNicholsComponent = nullptr;
	AutoTuner::ChartViewComponent* m_chartViewComponent = nullptr;
	AutoTuner::Solver* m_solverObject = nullptr;
	AutoTuner::NyquistPlotComponent* m_nyquistPlotComponent = nullptr;

	size_t m_populationSize = s_agentCount;
	
	//AutoTuner::CSVExport m_csvExport;
	size_t m_epochCounter = 0;
	size_t m_targetEpochs = s_targetEpochs;
	TestSystem m_testSystem;

	AutoTuner::FrequencyResponse m_frequencyResponse;

	std::vector<sf::Vector2<double>> m_stepData;
	std::vector<sf::Vector2<double>> m_disturbanceData;

	std::vector<sf::Vector2<double>> m_learningStepData;
	std::vector<sf::Vector2<double>> m_learningDisturbanceData;

	double m_tuningGoalFactor_errorIntegral = 2.8;
	double m_tuningGoalFactor_actuatorEffort = 0.1;
	double m_tuningGoalFactor_overshoot = 111;
	double m_tuningGoalFactor_gainMargin = 0;
	double m_tuningGoalFactor_phaseMargin = 99;
	bool m_useLearningRateDecay = true;
	double m_learningRate = s_startLearningRate;


	
	ResultData m_resultData;
	void saveResultDataToFile(const ResultData& resultData, std::string folderPath) const override;
};