#pragma once

#include "AutoTuner_base.h"
#include "Utilities/FrequencyResponse.h"


namespace AutoTuner
{
	class AUTO_TUNER_API NyquistPlotComponent : public QSFML::Components::Drawable
	{
	public:
		NyquistPlotComponent(const std::string& name = "NyquistPlotComponent");

		void setFrequencyResponse(const FrequencyResponse::FrequencyResponseData& response);

	protected:
		void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override;
		// Converts a complex gain into a logarithmic Nyquist plot coordinate.
		static std::complex<double> closedLogNyquist(const std::complex<double>& G)
		{
			const double M = std::abs(G);      // linear magnitude
			const double phase = std::arg(G);  // preserve phase
			const double log2_10 = std::log10(2.0);

			double r; // mapped radius in [0, 2]

			if (M <= 1.0)
			{
				// Expand region around the origin
				r = std::pow(M, log2_10);
			}
			else
			{
				// Contract outside region logarithmically
				r = 2.0 - std::pow(M, -log2_10);
			}

			// Convert back to complex coordinate in CLN space
			return std::polar(r, phase);
		}
		static std::complex<double> inverseClosedLogNyquist(const std::complex<double>& cln)
		{
			const double r = std::abs(cln);
			const double phase = std::arg(cln);
			const double log2_10 = std::log10(2.0);
			double M; // original magnitude
			if (r <= 1.0)
			{
				M = std::pow(r, 1.0 / log2_10);
			}
			else
			{
				M = std::pow(2.0 - r, -1.0 / log2_10);
			}
			return std::polar(M, phase);
		}

		struct Circle
		{
			std::vector<double> x_points;
			std::vector<double> y_points;
			ImVec4 color;
			float lineThickness = 1.0f;
			double radius = 0.0;

			Circle(double radius, size_t pointCount, const ImVec4& col, float lineThickness = 1);
			void draw() const;
		};

		struct LinePath
		{
			std::vector<double> x_points;
			std::vector<double> y_points;
			ImVec4 color;
			float lineThickness = 1.0f;
			LinePath(const std::vector<std::complex<double>>& complexPoints, const ImVec4& col, float lineThickness = 1);
			void draw() const;
		};

		struct XYAxis
		{
			std::vector<double> x_points;
			std::vector<double> y_points;
			ImVec4 color;
			float lineThickness = 1.0f;
		
			XYAxis(double length, const ImVec4& col, float lineThickness = 1);
			void draw() const;
		};


		
		const ImVec4 m_dbCirclesColor;
		const size_t m_dbCirclePointCount;
		const float m_dbCircleLintThickness;

		std::complex<double> m_crossoverPoint;
		double m_crossoverFrequency  = 0;

		std::complex<double> m_gainMarginPoint;
		double m_gainMarginFrequency = 0;

		XYAxis m_axis;
		Circle m_m80dbCircle;
		Circle m_m40dbCircle;
		Circle m_m20dbCircle;
		Circle m_0dbCircle;
		Circle m_20dbCircle;
		Circle m_40dbCircle;
		Circle m_80dbCircle;
		Circle m_InfdbCircle;


		LinePath m_nyquistPath;
	};
}