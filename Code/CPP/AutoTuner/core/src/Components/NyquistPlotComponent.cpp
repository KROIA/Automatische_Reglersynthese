#include "Components/NyquistPlotComponent.h"
#include <format>

namespace AutoTuner
{
	NyquistPlotComponent::NyquistPlotComponent(const std::string& name)
		: QSFML::Components::Drawable(name)
		, m_dbCirclesColor(ImVec4(0.6f, 0.6f, 0.6f, 0.6f))
		, m_dbCirclePointCount(30)
		, m_dbCircleLintThickness(1)
		, m_axis(2.0, ImVec4(0.8f, 0.8f, 0.8f, 0.8f), 1.0f)
		, m_m80dbCircle(0.125, m_dbCirclePointCount, m_dbCirclesColor, m_dbCircleLintThickness)
		, m_m40dbCircle(0.25, m_dbCirclePointCount, m_dbCirclesColor, m_dbCircleLintThickness)
		, m_m20dbCircle(0.5, m_dbCirclePointCount, m_dbCirclesColor, m_dbCircleLintThickness)
		, m_0dbCircle(1, m_dbCirclePointCount, ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f)
		, m_20dbCircle(1.5, m_dbCirclePointCount, m_dbCirclesColor, m_dbCircleLintThickness)
		, m_40dbCircle(1.75, m_dbCirclePointCount, m_dbCirclesColor, m_dbCircleLintThickness)
		, m_80dbCircle(1.825, m_dbCirclePointCount, m_dbCirclesColor, m_dbCircleLintThickness)
		, m_InfdbCircle(2.0, m_dbCirclePointCount, ImVec4(0.9f, 0.9f, 0.9f, 0.9f), m_dbCircleLintThickness*2)
		, m_nyquistPath({}, ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 2.0f)
	{
		float colorAlpha = 0.6f;
		float alphaDecayFactor = 0.5f;
		m_m20dbCircle.color.w = colorAlpha;
		m_20dbCircle.color.w = colorAlpha;
		colorAlpha *= alphaDecayFactor;
		m_m40dbCircle.color.w = colorAlpha;
		m_40dbCircle.color.w = colorAlpha;
		colorAlpha *= alphaDecayFactor;
		m_m80dbCircle.color.w = colorAlpha;
		m_80dbCircle.color.w = colorAlpha;
	}


	void NyquistPlotComponent::setFrequencyResponse(const FrequencyResponse::FrequencyResponseData& response)
	{
		ImVec4 nyquistColor = m_nyquistPath.color;
		float nyquistLineThickness = m_nyquistPath.lineThickness;

		std::vector<std::complex<double>> mappedPoints(response.responsePoints.size());
		for(size_t i= 0; i < response.responsePoints.size(); i++)
		{
			mappedPoints[i] = closedLogNyquist(response.responsePoints[i].gain);
		}

		m_nyquistPath = LinePath(mappedPoints, nyquistColor, nyquistLineThickness);

		m_crossoverPoint  = closedLogNyquist(std::polar(1.0, M_PI + response.phaseMargin));
		m_gainMarginPoint = closedLogNyquist(std::polar(1.0 / response.gainMargin, M_PI));

		m_crossoverFrequency = response.crossingFrequency;
		m_gainMarginFrequency = response.gainCrossoverFrequency;
	}

	void NyquistPlotComponent::drawComponent(sf::RenderTarget& target, sf::RenderStates states) const
	{
		ImGui::Begin("Nyquist");
		ImPlot::PushStyleColor(ImPlotCol_AxisGrid, ImVec4(0, 0, 0, 0));
		// Create plot
		if (ImPlot::BeginPlot("##Circles", ImVec2(-1, -1), ImPlotFlags_NoLegend)) {
			// Push current style and modify it
			


			

			m_axis.draw();
			m_m80dbCircle.draw();
			m_m40dbCircle.draw();
			m_m20dbCircle.draw();
			m_0dbCircle.draw();
			m_20dbCircle.draw();
			m_40dbCircle.draw();
			m_80dbCircle.draw();
			m_InfdbCircle.draw();

			m_nyquistPath.draw();
			if (ImPlot::IsPlotHovered()) {
				ImPlotPoint p = ImPlot::GetPlotMousePos(); // linear coordinates

				// Convert to your custom domain (example using closedLogNyquist)
				std::complex<double> linear(p.x, p.y);
				std::complex<double> cln = inverseClosedLogNyquist(linear);

				std::string label = std::format("r={:.3f}\nphi={:.1f}", std::abs(cln), std::arg(cln) * 180.0 / M_PI);
	
				// 4. Get mouse pixel coordinates
				ImVec2 mouse = ImGui::GetIO().MousePos;
				mouse.x += 30;

				// 5. Draw on top of the plot at mouse position
				ImDrawList* draw_list = ImPlot::GetPlotDrawList();
				draw_list->AddText(mouse, IM_COL32(255, 255, 255, 255), label.c_str());

				// Optional: add small background box
				ImVec2 text_size = ImGui::CalcTextSize(label.c_str());
				draw_list->AddRectFilled(
					ImVec2(mouse.x - 2, mouse.y - 2),
					ImVec2(mouse.x + text_size.x + 2, mouse.y + text_size.y + 2),
					IM_COL32(0, 0, 0, 200), 3.0f
				);
				draw_list->AddText(ImVec2(mouse.x, mouse.y), IM_COL32(255, 255, 255, 255), label.c_str());
			}

			double px = m_crossoverPoint.real();
			double py = m_crossoverPoint.imag();
			ImPlot::PlotScatter("CrossoverPoint", &px, &py, 1);   // draws the point
			std::string label = std::format("f={:.3f}", m_crossoverFrequency);
			ImPlot::PlotScatter("CrossoverPoint", &px, &py, 1);
			ImPlot::PlotText(label.c_str(), px + 0.1, py + 0.1);

			px = m_gainMarginPoint.real();
			py = m_gainMarginPoint.imag();
			ImPlot::PlotScatter("GainMarginPoint", &px, &py, 1);   // draws the point
			label = std::format("f={:.3f}", m_gainMarginFrequency);
			ImPlot::PlotText(label.c_str(), px + 0.1, py + 0.1);

			

			ImPlot::EndPlot();
			
		}
		ImPlot::PopStyleColor();
		ImGui::End();
	}



	NyquistPlotComponent::Circle::Circle(double radius, size_t pointCount, const ImVec4& col, float lineThickness)
		: color(col)
		, lineThickness(lineThickness)
		, radius(radius)
	{
		x_points.reserve(pointCount);
		y_points.reserve(pointCount);
		for (size_t i = 0; i < pointCount; i++)
		{
			double angle = (static_cast<double>(i) / static_cast<double>(pointCount-1)) * 2.0 * 3.14159265358979323846;
			x_points.push_back(radius * cos(angle));
			y_points.push_back(radius * sin(angle));
		}
	}

	void NyquistPlotComponent::Circle::draw() const
	{
		ImPlot::SetNextLineStyle(color, lineThickness);
		ImPlot::PlotLine("Circle", x_points.data(), y_points.data(), static_cast<int>(x_points.size()));		
	}
	NyquistPlotComponent::LinePath::LinePath(const std::vector<std::complex<double>>& complexPoints, const ImVec4& col, float lineThickness)
		: color(col)
		, lineThickness(lineThickness)
	{
		x_points.reserve(complexPoints.size());
		y_points.reserve(complexPoints.size());
		for (const auto& point : complexPoints)
		{
			x_points.push_back(point.real());
			y_points.push_back(point.imag());
		}
	}

	void NyquistPlotComponent::LinePath::draw() const
	{
		ImPlot::SetNextLineStyle(color, lineThickness);
		ImPlot::PlotLine("LinePath", x_points.data(), y_points.data(), static_cast<int>(x_points.size()));
	}

	NyquistPlotComponent::XYAxis::XYAxis(double length, const ImVec4& col, float lineThickness)
		: color(col)
		, lineThickness(lineThickness)
	{
		x_points = { -length, length, 0.0, 0.0 };
		y_points = { 0.0, 0.0, -length, length };
	}
	void NyquistPlotComponent::XYAxis::draw() const
	{
		ImPlot::SetNextLineStyle(color, lineThickness);
		ImPlot::PlotLine("X-Axis", x_points.data(), y_points.data(), static_cast<int>(x_points.size()), ImPlotLineFlags_Segments);
	}
}