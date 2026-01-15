#pragma once

#include <QWidget>
#include "ui_SetupSettingsWidget.h"
#include "Scene/Objects/PIDTuningProblem.h"


class SetupSettingsWidget : public QWidget
{
	Q_OBJECT
public:
	SetupSettingsWidget(QWidget* parent = nullptr);
	SetupSettingsWidget(const PIDTuningProblem::SetupSettings& settings, QWidget* parent = nullptr);
	~SetupSettingsWidget();

	void setSettings(const PIDTuningProblem::SetupSettings& settings);
	PIDTuningProblem::SetupSettings getSettings() const;

signals:
	void okeyClicked(const PIDTuningProblem::SetupSettings& settings);
	void cancelClicked();

private slots:
	void on_okey_pushButton_clicked();
	void on_cancel_pushButton_clicked();

	void on_solverType_comboBox_currentIndexChanged(int index);

protected:
	void closeEvent(QCloseEvent* event) override;
private:
	void populateComboBoxes();
	Ui::SetupSettingsWidget ui;


};
