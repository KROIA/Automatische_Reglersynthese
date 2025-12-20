#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <vector>
#include <memory>

#include "QSFML_EditorWidget.h"
#include "scenes/Scenes.h"




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
	void onDisturbanceClicked();
	void onDisturbanceReleased();
    

private:
    void setupScenes();
    template<typename SC> 
	void addScene()
	{
		std::shared_ptr<SC> scene = std::make_shared<SC>();
		scene->setup(m_tabWidget);
		m_scenes.push_back(scene);
		scene->start();
	}
	std::shared_ptr<AutoTuner::Experiment> getCurrentExperiment() const;

    void closeEvent(QCloseEvent* event) override;


    Ui::MainWindow* ui;
	QToolBar* m_toolBar;
	QAction* m_resetAction;
	QAction* m_hitAction;
	QToolButton* m_disturbanceButton;


	QTabWidget* m_tabWidget;

	std::vector<std::shared_ptr<AutoTuner::Experiment>> m_scenes;
};
