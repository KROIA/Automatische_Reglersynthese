#include "SetupSettingsWidget.h"


SetupSettingsWidget::SetupSettingsWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	populateComboBoxes();
}
SetupSettingsWidget::SetupSettingsWidget(const PIDTuningProblem::SetupSettings& settings, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	populateComboBoxes();
	setSettings(settings);
}


SetupSettingsWidget::~SetupSettingsWidget()
{

}

void SetupSettingsWidget::setSettings(const PIDTuningProblem::SetupSettings& settings)
{
	
	//ui.useMinimizingScore_checkBox->setChecked(settings.useMinimizingScore);
	ui.useMinimizingScore_comboBox->setCurrentIndex(ui.useMinimizingScore_comboBox->findData(QVariant::fromValue(static_cast<int>(settings.useMinimizingScore))));

	ui.useGeneticMutationRateDecay_checkBox->setChecked(settings.useGeneticMutationRateDecay);
	ui.useGeneticMutationRateDecay_checkBox->setEnabled(settings.solverType == PIDTuningProblem::SolverType::GeneticAlgorithm);
	ui.learningRateDecay_doubleSpinBox->setValue(settings.learningRateDecay);
	ui.learningRateDecay_doubleSpinBox->setEnabled(settings.solverType == PIDTuningProblem::SolverType::GeneticAlgorithm);

	ui.startLearningRate_doubleSpinBox->setValue(settings.startLearningRate);

	ui.useKn_checkBox->setChecked(settings.useKn);

	ui.solverType_comboBox->setCurrentIndex(ui.solverType_comboBox->findData(QVariant::fromValue(static_cast<int>(settings.solverType))));

	ui.disableErrorIntegrationWhenSaturated_checkBox->setChecked(settings.disableErrorIntegrationWhenSaturated);


	ui.optimizeKp_checkBox->setChecked(settings.optimizeKp);
	ui.optimizeKi_checkBox->setChecked(settings.optimizeKi);
	ui.optimizeKd_checkBox->setChecked(settings.optimizeKd);
	ui.optimizeKn_checkBox->setChecked(settings.optimizeKn);
	ui.optimizeIntegralSaturation_checkBox->setChecked(settings.optimizeIntegralSaturation);
	ui.optimizeAntiWindupBackCalculationConstant_checkBox->setChecked(settings.optimizeAntiWindupBackCalculationConstant);

	ui.defaultKp_doubleSpinBox->setValue(settings.defaultKp);
	ui.defaultKi_doubleSpinBox->setValue(settings.defaultKi);
	ui.defaultKd_doubleSpinBox->setValue(settings.defaultKd);
	ui.defaultKn_doubleSpinBox->setValue(settings.defaultKn);
	ui.defaultPIDISaturation_doubleSpinBox->setValue(settings.defaultPIDISaturation);
	ui.defaultPIDAntiWindupBackCalculationConstant_doubleSpinBox->setValue(settings.defaultPIDAntiWindupBackCalculationConstant);

	ui.defaultPIDAntiWindupMethod_comboBox->setCurrentIndex(ui.defaultPIDAntiWindupMethod_comboBox->findData(QVariant::fromValue(static_cast<int>(settings.defaultPIDAntiWindupMethod))));
	ui.defaultPIDIntegrationSolver_comboBox->setCurrentIndex(ui.defaultPIDIntegrationSolver_comboBox->findData(QVariant::fromValue(static_cast<int>(settings.defaultPIDIntegrationSolver))));
	ui.defaultPIDDerivativeType_comboBox->setCurrentIndex(ui.defaultPIDDerivativeType_comboBox->findData(QVariant::fromValue(static_cast<int>(settings.defaultPIDDerivativeType))));

	ui.startAreaRange_doubleSpinBox->setValue(settings.startAreaRange);
	ui.agentCount_spinBox->setValue(static_cast<int>(settings.agentCount));

	ui.endTime_doubleSpinBox->setValue(settings.endTime);
	ui.deltaTime_doubleSpinBox->setValue(settings.deltaTime);
	ui.targetEpochs_spinBox->setValue(static_cast<int>(settings.targetEpochs));

	ui.nyquistBeginFreq_doubleSpinBox->setValue(settings.nyquistBeginFreq);
	ui.nyquistEndFreq_doubleSpinBox->setValue(settings.nyquistEndFreq);
	ui.targetPhaseMargin_doubleSpinBox->setValue(settings.targetPhaseMargin * (180.0 / M_PI)); // to degrees
	ui.targetGainMargin_doubleSpinBox->setValue(settings.targetGainMargin);

	
}
PIDTuningProblem::SetupSettings SetupSettingsWidget::getSettings() const
{
	PIDTuningProblem::SetupSettings settings;

	settings.useGeneticMutationRateDecay = ui.useGeneticMutationRateDecay_checkBox->isChecked();
	settings.useMinimizingScore = ui.useMinimizingScore_comboBox->currentData().toBool();
	settings.useKn = ui.useKn_checkBox->isChecked();
	settings.solverType = static_cast<PIDTuningProblem::SolverType>(ui.solverType_comboBox->currentData().toInt());
	settings.disableErrorIntegrationWhenSaturated = ui.disableErrorIntegrationWhenSaturated_checkBox->isChecked();
	settings.optimizeKp = ui.optimizeKp_checkBox->isChecked();
	settings.optimizeKi = ui.optimizeKi_checkBox->isChecked();
	settings.optimizeKd = ui.optimizeKd_checkBox->isChecked();
	settings.optimizeKn = ui.optimizeKn_checkBox->isChecked();
	settings.optimizeIntegralSaturation = ui.optimizeIntegralSaturation_checkBox->isChecked();
	settings.optimizeAntiWindupBackCalculationConstant = ui.optimizeAntiWindupBackCalculationConstant_checkBox->isChecked();
	settings.defaultKp = ui.defaultKp_doubleSpinBox->value();
	settings.defaultKi = ui.defaultKi_doubleSpinBox->value();
	settings.defaultKd = ui.defaultKd_doubleSpinBox->value();
	settings.defaultKn = ui.defaultKn_doubleSpinBox->value();
	settings.defaultPIDISaturation = ui.defaultPIDISaturation_doubleSpinBox->value();
	settings.defaultPIDAntiWindupBackCalculationConstant = ui.defaultPIDAntiWindupBackCalculationConstant_doubleSpinBox->value();
	settings.defaultPIDAntiWindupMethod = static_cast<AutoTuner::PID::AntiWindupMethod>(ui.defaultPIDAntiWindupMethod_comboBox->currentData().toInt());
	settings.defaultPIDIntegrationSolver = static_cast<AutoTuner::PID::IntegrationSolver>(ui.defaultPIDIntegrationSolver_comboBox->currentData().toInt());
	settings.defaultPIDDerivativeType = static_cast<AutoTuner::PID::DerivativeType>(ui.defaultPIDDerivativeType_comboBox->currentData().toInt());
	settings.startAreaRange = ui.startAreaRange_doubleSpinBox->value();
	settings.agentCount = static_cast<size_t>(ui.agentCount_spinBox->value());
	settings.endTime = ui.endTime_doubleSpinBox->value();
	settings.deltaTime = ui.deltaTime_doubleSpinBox->value();
	settings.targetEpochs = static_cast<size_t>(ui.targetEpochs_spinBox->value());
	settings.nyquistBeginFreq = ui.nyquistBeginFreq_doubleSpinBox->value();
	settings.nyquistEndFreq = ui.nyquistEndFreq_doubleSpinBox->value();
	settings.targetPhaseMargin = ui.targetPhaseMargin_doubleSpinBox->value() * (M_PI / 180.0); // to radians
	settings.targetGainMargin = ui.targetGainMargin_doubleSpinBox->value();
	settings.startLearningRate = ui.startLearningRate_doubleSpinBox->value();
	settings.learningRateDecay = ui.learningRateDecay_doubleSpinBox->value();


	return settings;
}

void SetupSettingsWidget::populateComboBoxes()
{
	ui.solverType_comboBox->addItem("Genetic Algorithm", QVariant::fromValue(static_cast<int>(PIDTuningProblem::SolverType::GeneticAlgorithm)));
	ui.solverType_comboBox->addItem("Differential Evolution", QVariant::fromValue(static_cast<int>(PIDTuningProblem::SolverType::DifferentialEvolution)));

	ui.useMinimizingScore_comboBox->addItem("Minimieren", QVariant::fromValue(true));
	ui.useMinimizingScore_comboBox->addItem("Maximieren", QVariant::fromValue(false));

	ui.defaultPIDAntiWindupMethod_comboBox->addItem("Kein", QVariant::fromValue(static_cast<int>(AutoTuner::PID::AntiWindupMethod::None)));
	ui.defaultPIDAntiWindupMethod_comboBox->addItem("Clamping", QVariant::fromValue(static_cast<int>(AutoTuner::PID::AntiWindupMethod::Clamping)));
	ui.defaultPIDAntiWindupMethod_comboBox->addItem("Back Calculation", QVariant::fromValue(static_cast<int>(AutoTuner::PID::AntiWindupMethod::BackCalculation)));

	ui.defaultPIDIntegrationSolver_comboBox->addItem("Forward Euler", QVariant::fromValue(static_cast<int>(AutoTuner::PID::IntegrationSolver::ForwardEuler)));
	ui.defaultPIDIntegrationSolver_comboBox->addItem("Backward Euler", QVariant::fromValue(static_cast<int>(AutoTuner::PID::IntegrationSolver::BackwardEuler)));
	ui.defaultPIDIntegrationSolver_comboBox->addItem("Bilinear (Tustin)", QVariant::fromValue(static_cast<int>(AutoTuner::PID::IntegrationSolver::Bilinear)));
	//ui.defaultPIDIntegrationSolver_comboBox->addItem("Rk4", QVariant::fromValue(static_cast<int>(AutoTuner::PID::IntegrationSolver::Rk4)));

	ui.defaultPIDDerivativeType_comboBox->addItem("Unfiltered", QVariant::fromValue(static_cast<int>(AutoTuner::PID::DerivativeType::Unfiltered)));
	ui.defaultPIDDerivativeType_comboBox->addItem("Filtered", QVariant::fromValue(static_cast<int>(AutoTuner::PID::DerivativeType::Filtered)));

}

void SetupSettingsWidget::on_okey_pushButton_clicked()
{
	emit okeyClicked(getSettings());
}
void SetupSettingsWidget::on_cancel_pushButton_clicked()
{
	emit cancelClicked();
}
void SetupSettingsWidget::on_solverType_comboBox_currentIndexChanged(int index)
{
	PIDTuningProblem::SolverType selectedType = static_cast<PIDTuningProblem::SolverType>(ui.solverType_comboBox->itemData(index).toInt());
	ui.useGeneticMutationRateDecay_checkBox->setEnabled(selectedType == PIDTuningProblem::SolverType::GeneticAlgorithm);
	ui.learningRateDecay_doubleSpinBox->setEnabled(selectedType == PIDTuningProblem::SolverType::GeneticAlgorithm);
}
void SetupSettingsWidget::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
	emit cancelClicked();
}
