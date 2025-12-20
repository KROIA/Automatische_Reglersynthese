#include "Components/ChartViewComponent.h"

namespace AutoTuner
{
	ChartViewComponent::ChartViewComponent(const std::string& name)
		: QSFML::Components::Drawable(name)
	{
	}

	void ChartViewComponent::drawComponent(sf::RenderTarget& target, sf::RenderStates states) const
	{
		ImGui::Begin(getName().c_str());
        if (ImPlot::BeginPlot(getName().c_str(), ImVec2(-1, -1))) {
            // Plot the main curve
			for (const auto& plot : m_plotData)
			{
				ImPlot::PlotLine(plot.getName().c_str(), plot.getXData().data(), plot.getYData().data(), plot.getPointCount());
			}
            ImPlot::EndPlot();
        }
		ImGui::End();
	}

}