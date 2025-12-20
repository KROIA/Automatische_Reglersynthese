#pragma once

#include "AutoTuner_base.h"
#include "Components/ChartViewComponent.h"

namespace AutoTuner
{
	class AUTO_TUNER_API ZieglerNichols : public ChartViewComponent
	{
	public:
		enum class Method
		{
			ZN_P_controller,
			ZN_PI_controller,
			ZN_PID_controller
		};
		/**
		 * @brief PID parameters for the form:
		 * u(t) = Kp*e(t) + Ki*∫e(t)dt + Kd*de(t)/dt
		 */
		struct PIDParameters
		{
			float Kp;
			float Ki;
			float Kd;
		};
		ZieglerNichols(const std::string& name = "ZieglerNichols");


		/*ChartViewComponent::PlotData& getPlotContainer()
		{
			return m_chartViewComponent->getPlotData();
		}*/

		void setStepResponse(const std::vector<float>& timeData, const std::vector<float>& responseData);
		PIDParameters getPID_Parameters(Method method) const;

		void setParameters(float TuX, float TgX, float KsY, const sf::Vector2f& tangentTurningPoint);

	protected:
		void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override;

		void drawHorizontalMarker(float startX, float endX, float yValue, ImU32 color, float thickness, const std::string& textLabel) const;
		void drawVerticalMarker(float startY, float endY, float xValue, ImU32 color, float thickness, const std::string& textLabel) const;


	private:
		float m_TuX;
		float m_TgX;
		float m_KsY;

		sf::Vector2f m_turningPoint;
		float m_tangentSlope;
		float m_tangentStartX;
		float m_tangentEndX;
	};
}