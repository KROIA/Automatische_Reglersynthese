#pragma once


#include "AutoTuner.h"
#include <QObject>



class PIDTuningProblem : public QObject, public QSFML::Objects::GameObject
{
	Q_OBJECT
public:
	struct ResultData
	{
		struct ColumnData
		{
			std::string name;
			AutoTuner::CSVExport::LineStyle lineStyle = AutoTuner::CSVExport::LineStyle::Solid;
			int lineThickness = 1;
			sf::Color lineColor = sf::Color::Black;
			std::vector<double> data;
			bool isEnabled = true;
			ColumnData()
			{

			}
			ColumnData(const std::string& name,
				AutoTuner::CSVExport::LineStyle style,
				int thickness,
				const sf::Color& color)
				: name(name),
				lineStyle(style),
				lineThickness(thickness),
				lineColor(color)
			{
			}
		};

		struct LearningHistory
		{
			ColumnData worstScore;
			ColumnData averageScore;
			ColumnData bestScore;

			LearningHistory()
			{
				worstScore.name = "Min";
				averageScore.name = "Durchschnitt";
				bestScore.name = "Max";

				worstScore.lineStyle = AutoTuner::CSVExport::LineStyle::Solid;
				averageScore.lineStyle = AutoTuner::CSVExport::LineStyle::Solid;
				bestScore.lineStyle = AutoTuner::CSVExport::LineStyle::Solid;

				worstScore.lineColor = sf::Color(200, 50, 50);
				averageScore.lineColor = sf::Color(50, 50, 200);
				bestScore.lineColor = sf::Color(50, 200, 50);
			}
		};

		struct StepResponseData
		{
			ColumnData time;
			std::vector<ColumnData> responseSignals;

			StepResponseData()
			{
				time.name = "Time";
				time.lineStyle = AutoTuner::CSVExport::LineStyle::Solid;
				time.lineColor = sf::Color::Black;
			}
		};

		struct ParameterData
		{
			std::string name;
			double value = 0.0;
		};
		struct ParameterChangeData
		{
			std::vector<ColumnData> parameters;
		};

		std::string resultName;

		LearningHistory learningHistory;
		StepResponseData stepResponse;

		std::vector<ParameterData> parameters;
		ParameterChangeData parameterChanges;

		void clearData()
		{
			learningHistory.worstScore.data.clear();
			learningHistory.averageScore.data.clear();
			learningHistory.bestScore.data.clear();
			stepResponse.time.data.clear();
			for (auto& signal : stepResponse.responseSignals)
			{
				signal.data.clear();
			}
			//parameters.clear();
			for (auto& param : parameterChanges.parameters)
			{
				param.data.clear();
			}
		}

		void setParameterNames(const std::vector<std::string>& paramNames)
		{
			parameters.clear();
			parameterChanges.parameters.clear();
			for (const auto& name : paramNames)
			{
				parameters.push_back({ name, 0.0 });
				ColumnData colData;
				colData.name = name;
				parameterChanges.parameters.push_back(colData);
			}

		}
	};
	
	enum SolverType
	{
		GeneticAlgorithm,
		DifferentialEvolution
	};
	struct SetupSettings
	{
		bool useGeneticMutationRateDecay = true;
		bool useMinimizingScore = true;
		bool useKn = true;

		SolverType solverType = SolverType::GeneticAlgorithm;
		bool disableErrorIntegrationWhenSaturated = true;

		// Optimization parameters
		bool optimizeKp = true;
		bool optimizeKi = true;
		bool optimizeKd = true;
		bool optimizeKn = true;
		bool optimizeIntegralSaturation = false;
		bool optimizeAntiWindupBackCalculationConstant = false;



		double defaultKp = 1.0;
		double defaultKi = 1.0;
		double defaultKd = 1.0;
		double defaultKn = 1.0;
		double defaultPIDISaturation = 10;
		double defaultPIDAntiWindupBackCalculationConstant = 0.1;

		AutoTuner::PID::AntiWindupMethod defaultPIDAntiWindupMethod = AutoTuner::PID::AntiWindupMethod::Clamping;
		AutoTuner::PID::IntegrationSolver defaultPIDIntegrationSolver = AutoTuner::PID::IntegrationSolver::ForwardEuler;

		AutoTuner::PID::DerivativeType defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Filtered;
		//AutoTuner::PID::DerivativeType defaultPIDDerivativeType = AutoTuner::PID::DerivativeType::Unfiltered;

		

		double startAreaRange = 10;
		size_t agentCount = 30;
		
		double endTime = 20;
		double deltaTime = 0.01;
		size_t targetEpochs = 5000;

		double nyquistBeginFreq = 0.1;
		double nyquistEndFreq = 1000;
		double targetPhaseMargin = M_PI / 2; // degrees
		double targetGainMargin = 2;


		double startLearningRate = 1;
		double learningRateDecay = 0.999; // per generation
		
		SetupSettings() {}
		SetupSettings(const SetupSettings& other) = default;
	};


	PIDTuningProblem(const std::string& name = "PIDTuningProblem", QSFML::Objects::GameObject* parent = nullptr)
		: QSFML::Objects::GameObject(name, parent)
	{
	}

	//void onAwake() override;
	virtual void precalculatePIDWithZieglerNichols() = 0;
	virtual void setupPopulation(size_t populationSize, double kp, double ki, double kd, double integralSatturation, double areaRange = 1) = 0;

	//void update() override;

	virtual void createNewStepSequence() = 0;
	virtual void resetPopulation() = 0;
	virtual void testBestAgent() = 0;
	virtual void setLearningAmount(double learningAmount) = 0;
	virtual double getLearningAmount() const = 0;
	virtual void setLearningRate(double learningRate) = 0;
	virtual double getLearningRate() const = 0;

	virtual void saveResultsToFile(const std::string& resultName)= 0;
	virtual void setResultsLearningHistoryColors(const sf::Color& color)= 0;
	virtual void setResultsLearningHistoryColors(const sf::Color& averageColor, const sf::Color& bestColor, const sf::Color& worstColor)= 0;
	virtual void setResultsParameterColors(const sf::Color& color)= 0;
	virtual void setResultsParameterColors(const std::vector<sf::Color>& colors)= 0;
	virtual size_t getParameterCount() const= 0;
	virtual void setResultsParameterEnabled(size_t index, bool enabled)= 0;
	virtual void setResultsLearningHistoryEnableColumn(bool enableMin, bool enableAverage, bool enableMax)= 0;
	virtual size_t getResultsStepResponseSingalsCount() const= 0;
	virtual void setResultsStepResponseSignalColor(size_t index, const sf::Color& color)= 0;
	virtual void setResultsStepResponseSignalName(size_t index, const std::string& name)= 0;
	virtual void setResultsStepResponseSignalLineStyle(size_t index, AutoTuner::CSVExport::LineStyle style)= 0;
	virtual void setResultsStepResponseSignalLineThickness(size_t index, int thickness)= 0;
	virtual void setResultsStepResponseSignalEnabled(size_t index, bool enabled)= 0;
	virtual const sf::Color& getResultsStepResponseSignalColor(size_t index) const= 0;
	virtual const std::string& getResultsStepResponseSignalName(size_t index) const= 0;
	virtual const AutoTuner::CSVExport::LineStyle getResultsStepResponseSignalLineStyle(size_t index) const= 0;
	virtual int getResultsStepResponseSignalLineThickness(size_t index) const= 0;
	virtual bool isResultsStepResponseSignalEnabled(size_t index) const= 0;

	virtual std::vector<double> getBestParameters() const = 0;
	virtual std::vector<QString> getParameterLabels() const = 0;
	virtual size_t getPopulationSize() const = 0;

	virtual void setTuningGoalParameters(double errorIntegralWeight,
		double actuatorEffortWeight,
		double overshootWeight) = 0;
	virtual void setTuningGoalParameter_ErrorIntegralWeight(double weight) = 0;
	virtual void setTuningGoalParameter_ActuatorEffortWeight(double weight) = 0;
	virtual void setTuningGoalParameter_OvershootWeight(double weight) = 0;
	virtual void setTuningGoalParameter_GainMarginWeight(double weight) {}
	virtual void setTuningGoalParameter_PhaseMarginWeight(double weight) {}
	virtual double getTuningGoalParameter_ErrorIntegralWeight() const = 0;
	virtual double getTuningGoalParameter_ActuatorEffortWeight() const = 0;
	virtual double getTuningGoalParameter_OvershootWeight() const = 0;
	virtual double getTuningGoalParameter_GainMarginWeight() const { return 0.0; }
	virtual double getTuningGoalParameter_PhaseMarginWeight() const { return 0.0; }
	virtual void enableLearningRateDecay(bool enable) = 0;
	virtual bool isLearningRateDecayEnabled() const = 0;

	virtual void testPID(const std::vector<double>& params, ResultData* resultContainer = nullptr) = 0;

	virtual std::vector<double> agentTestFunction(const std::vector<double>& parameters, size_t agent) = 0;

	virtual void printSignalSequenceToConsole(const std::string& name, const std::vector<sf::Vector2<double>>& steps) const = 0;

	virtual void setTargetEpoch(size_t epoch) = 0;
signals:
	void targetEpochReached(size_t epoch);
protected:
	
	virtual std::vector<sf::Vector2<double>> generateRandomStepSequence(double stepAmplitude, double maxTime, double minStepDuration, double maxStepDuration, size_t stepCount) = 0;

	virtual void setCSVHeader() = 0;
	virtual void logCSVData() = 0;
	virtual void testCustomPID() = 0;

	virtual void saveResultDataToFile(const ResultData& resultData, std::string folderPath) const = 0;
};