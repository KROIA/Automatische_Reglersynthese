#pragma once

#include "AutoTuner_base.h"


namespace AutoTuner
{
	class AUTO_TUNER_API ChartViewComponent : public QSFML::Components::Drawable
	{
	public:
		class PlotData
		{
		public:
			PlotData(const std::string& name = "PlotData")
				: m_name(name)
			{
			}
			void addDataPoint(double x, double y)
			{
				x_data.push_back(x);
				y_data.push_back(y);
			}
			void clear()
			{
				x_data.clear();
				y_data.clear();
			}
			size_t getPointCount() const
			{
				return x_data.size();
			}
			void setName(const std::string& name) { m_name = name; }
			const std::string& getName() const { return m_name; }

			const std::vector<double>& getXData() const { return x_data; }
			const std::vector<double>& getYData() const { return y_data; }

		private:
			std::string m_name;
			std::vector<double> x_data, y_data;
		};

		ChartViewComponent(const std::string& name = "ChartViewComponent");
		//PlotData& getPlotData() { return m_plotData; }

		void addPlotData(const PlotData& plotData)
		{
			m_plotData.push_back(plotData);
		}
		void clearPlotData()
		{
			m_plotData.clear();
		}

	protected:
		void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override;


		std::vector<PlotData> m_plotData;
	private:

	};
}