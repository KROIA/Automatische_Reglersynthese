#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QLabel>
#include <vector>
#include <memory>

#include "Scene/Scene.h"




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
	void on_reset_pushButton_clicked();
	void on_generateStepSequence_pushButton_clicked();
	void on_learningAmount_lineEdit_editingFinished();
	void on_learningRateDecay_checkBox_stateChanged(int arg1);
	void on_testBestPID_pushButton_clicked();
	void on_autoTestBestPID_checkBox_stateChanged(int arg1);
	void on_parametersColor_pushButton_clicked();
	void on_learnCurveExport_pushButton_clicked();
	void on_multiRun_pushButton_clicked();
	void on_multiRunColor1_pushButton_clicked();
	void on_multiRunColor2_pushButton_clicked();

	void on_errorIntegral_verticalSlider_valueChanged(int value);
	void on_actuatorAction_verticalSlider_valueChanged(int value);
	void on_overshoot_verticalSlider_valueChanged(int value);
	void on_gainMargin_verticalSlider_valueChanged(int value);
	void on_phaseMargin_verticalSlider_valueChanged(int value);


	void onAutoTestTimerFinished();
	void onUpdateTimerFinished();

	void onEpochReached(size_t epoch);
private:
    void setupScene();

    void closeEvent(QCloseEvent* event) override;


    Ui::MainWindow* ui;
	Scene* m_scene;
	QTimer* m_autoTestTimer = nullptr;
	QTimer* m_updateTimer = nullptr;

	bool m_isMultiRunning = false;
	size_t m_multiRunCount = 0;
	size_t m_currentMultiRun = 0;
	sf::Color m_multiRunColor1 = sf::Color::Cyan;
	sf::Color m_multiRunColor2 = sf::Color::Blue;
	std::string m_outputFolder = "";

	std::vector<QLabel*> m_scrollAreaLabels;
};
