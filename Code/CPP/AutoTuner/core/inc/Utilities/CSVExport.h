#pragma once
#include "AutoTuner_base.h"

namespace AutoTuner
{
	class AUTO_TUNER_API CSVExport
	{
	public:
		enum class LineStyle
		{
			Solid,
			Dashed,
			Dotted,
			DashDotted
		};


		CSVExport();
		~CSVExport();

		/**
		* @brief Sets the name of each column in the CSV file.
		* The first column is always the x-axis (e.g., time).
		*/
		void setHeader(const std::vector<std::string>& header);

		/**
		* @brief Sets the label for the Y-axis.
		* This text gets inserted in the CSV in the second row in the first column.
		* That column is reserved for the line styles but since the x-axis has no line style,
		* this cell can be used for the Y-axis label.
		*/
		void setYAxisLabel(const std::string& label);

		/**
		* @brief Sets the line styles for each data series.
		* The length of this vector should match the number of data series (columns - 1), not including the x-axis.
		*/
		void setLineStyles(const std::vector<LineStyle>& styles);

		/**
		* @brief Sets the line thicknesses for each data series.
		* The length of this vector should match the number of data series (columns - 1), not including the x-axis.
		*/
		void setLineThicknesses(const std::vector<int>& thicknesses);

		/**
		* @brief Sets the line colors for each data series.
		* The length of this vector should match the number of data series (columns - 1), not including the x-axis.
		*/
		void setLineColors(const std::vector<sf::Color>& colors);

		void addRow(const std::vector<std::string>& row);
		void addRow(const std::vector<double>& row);

		void clearData();
		bool exportToFile(std::string filename, char delimiter = ';') const;

		static std::string lineStyleToString(LineStyle style)
		{
			switch (style)
			{
			case LineStyle::Solid:
				return "Solid";
			case LineStyle::Dashed:
				return "Dashed";
			case LineStyle::Dotted:
				return "Dotted";
			case LineStyle::DashDotted:
				return "DashDotted";
			default:
				return "Solid";
			}
		}
		static std::string lineStyleToCSVValue(LineStyle style)
		{
			switch (style)
			{
			case LineStyle::Solid:
				return "-";
			case LineStyle::Dashed:
				return "--";
			case LineStyle::Dotted:
				return ":";
			case LineStyle::DashDotted:
				return "-.";
			default:
				return "-";
			}
		}

		static std::string colorToHexString(const sf::Color& color)
		{
			int rgbValue =  (static_cast<int>(color.r) << 16) |
							(static_cast<int>(color.g) << 8) |
							(static_cast<int>(color.b));
			return intToHexString(rgbValue);
		}
		static std::string intToHexString(int value)
		{
			std::stringstream ss;
			ss << std::hex << std::uppercase << value;
			// Pad with leading zeros if necessary
			while (ss.str().length() < 6)
			{
				ss.str("0" + ss.str());
			}
			return ss.str();
		}
	private:

		std::vector<std::string> m_header;
		std::vector<std::vector<std::string>> m_rows;

		std::string m_yAxisLabel;
		std::vector<LineStyle> m_lineStyles;
		std::vector<int> m_lineThicknesses;
		std::vector<sf::Color> m_lineColors;
	};
}