#include "Experiment/Scope.h"

namespace AutoTuner
{
    Scope::Scope(const std::string& title, QVector<double>& timePoints, size_t maxPoints)
        : m_title(title)
        , m_maxPoints(maxPoints)
        , m_timePoints(&timePoints)
    {

    }
    Scope::~Scope()
    {
        clear();
    }

    void Scope::setPointCount(size_t count)
    {
		m_maxPoints = count;
    }

    void Scope::drawPlot()
    {
        for (auto plotsPair : m_plots)
        {
            Plot* plot = plotsPair.second;
            for (auto s : plot->series)
            {
                if (s->points->size() > m_maxPoints)
                {
                    s->points->remove(0, s->points->size() - m_maxPoints);
                }
            }
        }
        if (m_timePoints->size() > m_maxPoints)
            m_timePoints->remove(0, m_timePoints->size() - m_maxPoints);

        ImGui::Begin(m_title.c_str());
        for (auto plotsPair : m_plots)
        {
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(plotsPair.first.c_str()))
            {
                for (auto s : plotsPair.second->series)
                {
					ImPlot::SetNextLineStyle(s->color, s->thickness);
                    ImPlot::PlotLine(s->name.c_str(), m_timePoints->data(), s->points->data(), s->points->size());
                }
                ImPlot::EndPlot();
            }
        }
        ImGui::End();
    }

    // Add a dataset (line) with a specific color
    void Scope::registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName)
    {
        //Series* s = new Series(series, seriesName);
        Plot* p = m_plots[plotName];
        if (!p)
        {
            p = new Plot();
            p->name = plotName;
            m_plots[plotName] = p;
        }
		p->addSeries(series, seriesName);
    }
    void Scope::registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName, const ImVec4& color, float thickness)
    {
		//Series* s = new Series(series, seriesName, color, thickness);
		Plot* p = m_plots[plotName];
		if (!p)
		{
			p = new Plot();
			p->name = plotName;
			m_plots[plotName] = p;
		}
		p->addSeries(series, seriesName, color, thickness);
	}

    // Clear all data
    void Scope::clear()
    {
        for (auto s : m_plots)
            delete s.second;
        m_plots.clear();
    }
    void Scope::clearData()
    {
        m_timePoints->clear();
        for (auto plotsPair : m_plots)
        {
            Plot* plot = plotsPair.second;
            for (auto s : plot->series)
            {
                s->points->clear();
            }
        }
    }

    // -------------------------------------------------------------------------        
    //         Series
    // -------------------------------------------------------------------------

    Scope::Series::Series(QVector<double>& points, const std::string& name)
		: points(&points)
		, name(name)
		, color(ImVec4{ static_cast<float>(128+(rand() % 128)) / 255.0f,
                        static_cast<float>(128+(rand() % 128)) / 255.0f, 
                        static_cast<float>(128+(rand() % 128)) / 255.0f, 1.0f })
		, thickness(1.0f)
	{

	}
    Scope::Series::Series(QVector<double>& points, const std::string& name, const ImVec4& color, float thickness)
        : points(&points)
        , name(name)
        , color(color)
        , thickness(thickness)
    {

    }
    Scope::Series::Series(Series&& other) noexcept
        : points(other.points)
		, name(std::move(other.name))
		, color(other.color)
		, thickness(other.thickness)
    {
        other.points = nullptr;
    }
	Scope::Series::~Series()
	{

	}

    // -------------------------------------------------------------------------        
    //         Plot
    // -------------------------------------------------------------------------

    Scope::Plot::~Plot()
    {
        for (auto s : series)
            delete s;
    }

    void Scope::Plot::addSeries(QVector<double>& series, const std::string& name)
    {
        this->series.push_back(new Series(series, name));
    }
    void Scope::Plot::addSeries(QVector<double>& series, const std::string& name, const ImVec4& color, float thickness)
    {
        this->series.push_back(new Series(series, name, color, thickness));
    }

}