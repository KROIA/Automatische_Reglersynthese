#include "Components/ZieglerNichols.h"
#include <format>

namespace AutoTuner
{
	ZieglerNichols::ZieglerNichols(const std::string& name)
		: ChartViewComponent(name)
	{
       
	}


    void ZieglerNichols::setStepResponse(const std::vector<float>& timeData, const std::vector<float>& responseData)
	{
        if (timeData.size() != responseData.size() && timeData.empty())
            return;
        PlotData plotData;
		//auto& plotData = getPlotData();
		clearPlotData();
		for (size_t i = 0; i < timeData.size(); ++i) {
			plotData.addDataPoint(timeData[i], responseData[i]);
		}
		addPlotData(plotData);

		// Calculate Tu, Tg, Ks, and tangent point
        // Approximative approach
		// Step 1: Find Tangent Point (max slope)
		sf::Vector2f tangentPoint;
        float tangentSlope = 0;
        for (size_t i = 1; i < timeData.size()-1; ++i)
        {
			// Compare slopes of consecutive segments
			float slope1 = (responseData[i] - responseData[i - 1]) / (timeData[i] - timeData[i - 1]);
			float slope2 = (responseData[i + 1] - responseData[i]) / (timeData[i + 1] - timeData[i]);

            if (slope2 < slope1) // Slope is decreasing, we passed the max slope point
            {
				tangentPoint = sf::Vector2f(timeData[i], responseData[i]);
				tangentSlope = slope1;
				break;
            }
        }
        // Tangent function: y(x) = slope * x + q
		float q = tangentPoint.y - tangentSlope * tangentPoint.x;

        // Step 2: Calculate Tu
        m_TuX = -q / tangentSlope;

		// Step 3: Calculate Ks (steady state value), assuming the last value is steady state
        m_KsY = responseData.back();

		// Step 4: Calculate the tangent highest point (intersection with Ks)
		float tangentMaxX = (m_KsY - q) / tangentSlope;
        m_TgX = tangentMaxX - m_TuX;

		// Set parameters to chart component
		setParameters(m_TuX, m_TgX, m_KsY, tangentPoint);
    }


    ZieglerNichols::PIDParameters ZieglerNichols::getPID_Parameters(Method method) const
    {
		PIDParameters params{ 0,0,0 };
        switch (method)
        {
            case Method::ZN_P_controller:
            {
				params.Kp = m_TgX / (m_KsY * m_TuX);
                break;
            }
            case Method::ZN_PI_controller:
            {
				params.Kp = 0.9f * m_TgX / (m_KsY * m_TuX);
				params.Ki = params.Kp / (3.33f * m_TuX);
                break;
            }
            case Method::ZN_PID_controller:
            {
				params.Kp = 1.2f * m_TgX / (m_KsY * m_TuX);
				params.Ki = params.Kp / (2.0f * m_TuX);
				params.Kd = params.Kp * (0.5f * m_TuX);
                break;
            }
        }
        return params;
    }

    void ZieglerNichols::setParameters(float TuX, float TgX, float KsY, const sf::Vector2f& tangentTurningPoint)
    {
        m_TuX = TuX;
        m_TgX = TgX;
        m_KsY = KsY;

        // Calculate turning point
        m_turningPoint = tangentTurningPoint;

        // Calculate tangent slope
        m_tangentSlope = (m_turningPoint.y / (m_turningPoint.x - m_TuX));

        // Calculate tangent line start and end points
        m_tangentStartX = m_TuX; // Extend beyond TuX
        m_tangentEndX = (m_KsY + m_tangentSlope * m_TuX) / m_tangentSlope; // Extend to KsY
    }



	void ZieglerNichols::drawComponent(sf::RenderTarget& target, sf::RenderStates states) const
	{
		if (m_plotData.empty())
			return;
		const PlotData& plotData = *m_plotData.begin();
        ImGui::Begin(getName().c_str());
        if (ImPlot::BeginPlot("Function with Tangent", ImVec2(-1, -1))) {
            // Plot the main curve

            ImPlot::PlotLine(plotData.getName().c_str(), plotData.getXData().data(), plotData.getYData().data(), plotData.getPointCount());


            // Plot tangent line (extended range)
            float tangent_y_start = 0;
            float tangent_y_end = m_KsY;

            float tangent_xs[] = { m_tangentStartX, m_tangentEndX };
            float tangent_ys[] = { tangent_y_start, tangent_y_end };
            ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 0, 0, 1));
            ImPlot::PlotLine("Tangent", tangent_xs, tangent_ys, 2);
            ImPlot::PopStyleColor();

            // CUSTOM DRAWING: Use ImDrawList to add custom graphics
            // Convert plot coordinates to pixel coordinates
            ImVec2 point_px = ImPlot::PlotToPixels(m_turningPoint.x, m_turningPoint.y);
            ImDrawList* draw_list = ImPlot::GetPlotDrawList();

            // Draw a circle at the tangent point
            draw_list->AddCircleFilled(point_px, 6.0f, IM_COL32(255, 255, 0, 255));
            draw_list->AddCircle(point_px, 6.0f, IM_COL32(0, 0, 0, 255), 0, 2.0f);

            // Draw custom text annotation
            ImVec2 text_pos = ImVec2(point_px.x + 10, point_px.y - 10);
            char label[64];
            snprintf(label, sizeof(label), "(%.3f, %.3f)", m_turningPoint.x, m_turningPoint.y);
            draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), label);


            float yCenter = (*(plotData.getYData().end()-1) - *plotData.getYData().begin())/2.f;

			drawHorizontalMarker(0.0f, m_TuX, yCenter, IM_COL32(0, 255, 0, 200), 2.0f, "Tu=" + std::format("{:.2f}s", m_TuX));
			drawHorizontalMarker(m_TuX, m_TuX+m_TgX, yCenter, IM_COL32(0, 255, 0, 200), 2.0f, "Tg=" + std::format("{:.2f}s", m_TgX));


			drawVerticalMarker(0.0f, m_KsY, m_TuX + m_TgX +0.1, IM_COL32(0, 255, 0, 200), 2.0f, "Ks=" + std::format("{:.2f}", m_KsY));

			/*// Draw horizintal line from x=0 to x=Tu at y= yCenter with a label next to it with text "Tu={value}"
			ImVec2 tu_start = ImPlot::PlotToPixels(0, yCenter);
			ImVec2 tu_end = ImPlot::PlotToPixels(m_TuX, yCenter);
			draw_list->AddLine(tu_start, tu_end, IM_COL32(0, 255, 0, 200), 2.0f);
			char tu_label[64];
			snprintf(tu_label, sizeof(tu_label), "Tu=%.3f", m_TuX);
			// Label pos centered above the line
            ImVec2 labelPos = ImPlot::PlotToPixels(m_TuX / 2, yCenter);
			labelPos.y -= 15;
            // Get text width to calculate x offset
			float textWidth = ImGui::CalcTextSize(tu_label).x;
			labelPos.x -= textWidth / 2;

			draw_list->AddText(labelPos, IM_COL32(0, 255, 0, 255), tu_label);*/



            // Draw a dashed line from point to axis (custom visualization)
            //ImVec2 axis_point = ImPlot::PlotToPixels(m_turningPoint.x, 0);
            //for (float t = 0; t < 1.0f; t += 0.05f) {
            //    if (((int)(t * 20)) % 2 == 0) {
            //        ImVec2 p1 = ImVec2(
            //            point_px.x,
            //            point_px.y + (axis_point.y - point_px.y) * t
            //        );
            //        ImVec2 p2 = ImVec2(
            //            point_px.x,
            //            point_px.y + (axis_point.y - point_px.y) * (t + 0.025f)
            //        );
            //        draw_list->AddLine(p1, p2, IM_COL32(150, 150, 150, 200), 1.5f);
            //    }
            //}

            ImPlot::EndPlot();
        }
        ImGui::End();
	}

    void ZieglerNichols::drawHorizontalMarker(float startX, float endX, float yValue, ImU32 color, float thickness, const std::string &textLabel) const
    {
        float markerEdgeHight = 5;
        ImDrawList* draw_list = ImPlot::GetPlotDrawList();
        // Draw horizintal line from x=0 to x=Tu at y= yCenter with a label next to it with text "Tu={value}"
        ImVec2 tu_start = ImPlot::PlotToPixels(startX, yValue);
        ImVec2 tu_end = ImPlot::PlotToPixels(endX, yValue);
        draw_list->AddLine(tu_start, tu_end, color, 2.0f);
        draw_list->AddLine(ImVec2(tu_start.x, tu_start.y - markerEdgeHight), ImVec2(tu_start.x, tu_start.y + markerEdgeHight), color, 2.0f);
        draw_list->AddLine(ImVec2(tu_end.x, tu_end.y - markerEdgeHight), ImVec2(tu_end.x, tu_end.y + markerEdgeHight), color, 2.0f);
        // Label pos centered above the line
        ImVec2 labelPos = ImPlot::PlotToPixels(endX/2, yValue);
        labelPos.y -= 15;
        // Get text width to calculate x offset
        float textWidth = ImGui::CalcTextSize(textLabel.c_str()).x;
        labelPos.x -= textWidth / 2;

        draw_list->AddText(labelPos, color, textLabel.c_str());


        // Draw a dashed line from point to axis (custom visualization)
        ImVec2 point_px1 = ImPlot::PlotToPixels(startX, yValue);
        ImVec2 point_px2 = ImPlot::PlotToPixels(endX, yValue);
        bool needStartDashedLine = startX != 0;
        ImVec2 axis_point = ImPlot::PlotToPixels(0, 0);
        for (float t = 0; t < 1.0f; t += 0.05f) {
            if (((int)(t * 20)) % 2 == 0) {
                if (needStartDashedLine)
                {
                    ImVec2 p11 = ImVec2(
                        point_px1.x,
                        point_px1.y + (axis_point.y - point_px1.y) * t
                    );
                    ImVec2 p21 = ImVec2(
                        point_px1.x,
                        point_px1.y + (axis_point.y - point_px1.y) * (t + 0.025f)
                    );
                    draw_list->AddLine(p11, p21, IM_COL32(150, 150, 150, 200), 1.5f);
                }
                
                ImVec2 p12 = ImVec2(
                    point_px2.x,
                    point_px2.y + (axis_point.y - point_px2.y) * t
                );
                ImVec2 p22 = ImVec2(
                    point_px2.x,
                    point_px2.y + (axis_point.y - point_px2.y) * (t + 0.025f)
                );
                
                draw_list->AddLine(p12, p22, IM_COL32(150, 150, 150, 200), 1.5f);
            }
        }
    }

    void ZieglerNichols::drawVerticalMarker(float startY, float endY, float xValue, ImU32 color, float thickness, const std::string& textLabel) const
    {
		float markerEdgeHight = 5;
		ImDrawList* draw_list = ImPlot::GetPlotDrawList();
		// Draw horizintal line from x=0 to x=Tu at y= yCenter with a label next to it with text "Tu={value}"
		ImVec2 tu_start = ImPlot::PlotToPixels(xValue, startY);
		ImVec2 tu_end = ImPlot::PlotToPixels(xValue, endY);
		draw_list->AddLine(tu_start, tu_end, color, 2.0f);
		draw_list->AddLine(ImVec2(tu_start.x - markerEdgeHight, tu_start.y), ImVec2(tu_start.x + markerEdgeHight, tu_start.y), color, 2.0f);
		draw_list->AddLine(ImVec2(tu_end.x - markerEdgeHight, tu_end.y), ImVec2(tu_end.x + markerEdgeHight, tu_end.y), color, 2.0f);
		// Label pos centered above the line
		ImVec2 labelPos = ImPlot::PlotToPixels(xValue, endY);
		labelPos.x += 10;
		// Get text width to calculate x offset
		//float textWidth = ImGui::CalcTextSize(textLabel.c_str()).x;
		//labelPos.x -= textWidth / 2;

		draw_list->AddText(labelPos, color, textLabel.c_str());

		// Draw a dashed line from point to axis (custom visualization)
		ImVec2 point_px1 = ImPlot::PlotToPixels(xValue, startY);
		ImVec2 point_px2 = ImPlot::PlotToPixels(xValue, endY);
		ImVec2 axis_point = ImPlot::PlotToPixels(0, 0);
        bool needStartDashedLine = startY != 0;
        for (float t = 0; t < 1.0f; t += 0.05f) {
            if (((int)(t * 20)) % 2 == 0) {
                if (needStartDashedLine)
                {
                    ImVec2 p11 = ImVec2(
                        point_px1.x + (axis_point.x - point_px1.x) * t,
                        point_px1.y
                    );
                    ImVec2 p21 = ImVec2(
                        point_px1.x + (axis_point.x - point_px1.x) * (t + 0.025f),
                        point_px1.y
                    );
                    draw_list->AddLine(p11, p21, IM_COL32(150, 150, 150, 200), 1.5f);
                }
                
                ImVec2 p12 = ImVec2(
                    point_px2.x + (axis_point.x - point_px2.x) * t,
                    point_px2.y
                );
                ImVec2 p22 = ImVec2(
                    point_px2.x + (axis_point.x - point_px2.x) * (t + 0.025f),
                    point_px2.y
                );
                
                draw_list->AddLine(p12, p22, IM_COL32(150, 150, 150, 200), 1.5f);

            }
        }


    }

}