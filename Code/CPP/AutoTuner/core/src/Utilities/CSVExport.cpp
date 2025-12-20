#include "Utilities/CSVExport.h"
#include <fstream>
#include <filesystem>

namespace AutoTuner
{
	CSVExport::CSVExport()
	{
	}

	CSVExport::~CSVExport()
	{

	}

	void CSVExport::setHeader(const std::vector<std::string>& header)
	{
		m_header = header;
	}
	void CSVExport::setYAxisLabel(const std::string& label)
	{
		m_yAxisLabel = label;
	}
	void CSVExport::setLineStyles(const std::vector<LineStyle>& styles)
	{
		m_lineStyles = styles;
	}
	void CSVExport::setLineThicknesses(const std::vector<int>& thicknesses)
	{
		m_lineThicknesses = thicknesses;
	}
	void CSVExport::setLineColors(const std::vector<sf::Color>& colors)
	{
		m_lineColors = colors;
	}

	void CSVExport::addRow(const std::vector<std::string>& row)
	{
		m_rows.push_back(row);
	}
	void CSVExport::addRow(const std::vector<double>& row)
	{
		std::vector<std::string> stringRow;
		for (const auto& value : row)
		{
			stringRow.push_back(std::to_string(value));
		}
		m_rows.push_back(stringRow);
	}

	void CSVExport::clearData()
	{
		m_rows.clear();
	}
	bool CSVExport::exportToFile(std::string filename, char delimiter) const
	{
		// Add .csv extension if not present
		if (filename.find_last_of('.') == std::string::npos || filename.substr(filename.find_last_of('.') + 1) != "csv")
		{
			filename += ".csv";
		}

		// Get path if exists
		if(filename.find_last_of("/\\") != std::string::npos)
		{
			std::string path = filename.substr(0, filename.find_last_of("/\\"));
			// Create directories if they don't exist
			std::filesystem::create_directories(path);
		}

		// Check if file already exists, if so add a number to the filename
		std::string newFilename = filename;
		int fileIndex = 1;
		while (std::filesystem::exists(newFilename))
		{
			size_t dotPosition = filename.find_last_of('.');
			if (dotPosition != std::string::npos)
			{
				newFilename = filename.substr(0, dotPosition) + "_" + std::to_string(fileIndex) + filename.substr(dotPosition);
			}
			else
			{
				newFilename = filename + "_" + std::to_string(fileIndex);
			}
			++fileIndex;
		}

		std::ofstream file(newFilename);
		if (!file.is_open())
			return false;

		size_t dataColumnsCount = m_header.size() - 1;

		// Write header
		for (size_t i = 0; i < m_header.size(); ++i)
		{
			file << m_header[i];
			if (i < m_header.size() - 1)
				file << delimiter;
		}
		file << "\n";

		// Write Y-axis label in the second row, first column
		file << m_yAxisLabel << delimiter;

		// Write line styles
		for(size_t i=0; i< dataColumnsCount; ++i)
		{
			if (i < m_lineStyles.size())
				file << lineStyleToCSVValue(m_lineStyles[i]);
			else
				file << lineStyleToCSVValue(LineStyle::Solid);
			if (i < dataColumnsCount - 1)
				file << delimiter;
		}

		file << "\n";

		// Write line thicknesses
		file << "0" << delimiter; // No color for x-axis
		for(size_t i=0; i< dataColumnsCount; ++i)
		{
			if (i < m_lineThicknesses.size())
				file << m_lineThicknesses[i];
			else
				file << "1";
			if (i < dataColumnsCount - 1)
				file << delimiter;
		}

		file << "\n";

		// Write line colors
		file << "0" << delimiter; // No color for x-axis
		for(size_t i=0; i< dataColumnsCount; ++i)
		{
			if (i < m_lineColors.size())
				file << "\""+colorToHexString(m_lineColors[i])+"\"";
			else
				file << intToHexString(0);
			if (i < dataColumnsCount - 1)
				file << delimiter;
		}

		file << "\n";

		// Write rows
		for (const auto& row : m_rows)
		{
			for (size_t i = 0; i < row.size(); ++i)
			{
				file << row[i];
				if (i < row.size() - 1)
					file << delimiter;
			}
			file << "\n";
		}
		file.close();
		return true;
	}
}