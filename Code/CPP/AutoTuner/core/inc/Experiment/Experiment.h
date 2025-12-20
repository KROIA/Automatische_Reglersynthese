#pragma once

#include "AutoTuner_base.h"
#include "Scope.h"
#include "ExperimentTab.h"
#include <QSplitter>
#include <QTabWidget>

namespace AutoTuner
{
	class AUTO_TUNER_API Experiment : public QObject
	{
		Q_OBJECT
	public:
		Experiment(const QString& sceneName);
		virtual ~Experiment();

		void setup(QTabWidget* rootContainer);

		/**
		 * @brief Amount of points per signal in the scope
		 * @param count 
		 */
		void setScopePointCount(size_t count);

		/**
		 * @brief count of physics ticks between each scope capture
		 *        If set to 1, scope captures data every physics tick
		 * @param ticks 
		 */
		void setScopeCaptureTickInterval(size_t ticks);

		void start();
		void stop();
		void reset();
		void hit();
		void disturbance(bool enable);

		double getPhysicsDeltaT() const
		{
			if (m_scene)
			{
				const auto& settings = m_scene->getSettings();
				return settings.timing.physicsFixedDeltaT * settings.timing.physicsDeltaTScale;
			}
			return 0.01;
		}

	protected:

		void registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName);
		void registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName, const ImVec4& color, float thickness);

		virtual QWidget* createPropertyWidget() = 0;
		virtual void onSetup(QSFML::Scene* scene) = 0;
		virtual void onStart() = 0;
		virtual void onStop() = 0;
		virtual void onReset() = 0;
		virtual void onHit() = 0;
		virtual void onDisturbance() = 0;

		virtual void onSceneUpdate(QSFML::Scene* scene) = 0;
		virtual void onScopeCapture() {}


		void info(const std::string& msg)
		{
			m_logger.info(msg);
		}
		void trace(const std::string& msg)
		{
			m_logger.trace(msg);
		}
		void debug(const std::string& msg)
		{
			m_logger.debug(msg);
		}		
		void warning(const std::string& msg)
		{
			m_logger.warning(msg);
		}
		void error(const std::string& msg)
		{
			m_logger.error(msg);
		}
		void custom(const std::string& msg)
		{
			m_logger.custom(msg);
		}

	private slots:
		void onTabChanged(int index);

	private:
		void update();



		

		class SceneUpdateObj : public QSFML::Objects::GameObject
		{
		public:
			SceneUpdateObj(Experiment* parent)
				: QSFML::Objects::GameObject("Experiment Updater")
				, m_parent(parent)
			{
			}
			virtual ~SceneUpdateObj() {}
			void update() override
			{
				if (m_parent)
					m_parent->update();
			}

		private:
			Experiment* m_parent = nullptr;
		};

		QString m_sceneName;
		ExperimentTab* m_experimentTab = nullptr;
		QTabWidget* m_rootContainer = nullptr;


		QSFML::Scene* m_scene = nullptr;
		SceneUpdateObj* m_sceneUpdater = nullptr;
		double m_startTime;


		Scope* m_scope = nullptr;
		size_t m_scopeCaptureTickInterval = 1;
		size_t m_currentTick = 0;

		QVector<double> m_timeData;
		Log::LogObject m_logger;
		bool m_disturbanceEnabled = false;
	};
}