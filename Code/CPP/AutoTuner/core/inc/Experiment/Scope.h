#pragma once
#include "AutoTuner_base.h"
#include <vector>
#include <QVector>
#include <unordered_map>

namespace AutoTuner
{
    class AUTO_TUNER_API Scope
    {
    public:
        Scope(const std::string& title, QVector<double>& timePoints, size_t maxPoints = 1000);
        ~Scope();

        /**
         * @brief Amount of points per signal in the scope
         * @param count
         */
        void setPointCount(size_t count);


        void drawPlot();

        // Add a dataset (line) with a specific color
        void registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName);
        void registerSignal(QVector<double>& series, const std::string& plotName, const std::string& seriesName, const ImVec4 &color, float thickness);

        // Clear all data
        void clear();
        void clearData();
    protected:


    private:
        struct Series 
        {
            QVector<double>* points;
            std::string name;
			ImVec4 color;
			float thickness = 1.0f;

            Series(QVector<double>& points, const std::string& name);
            Series(QVector<double>& points, const std::string& name, const ImVec4 &color, float thickness);
            Series(const Series& other) = delete; // disable copy
            Series(Series&& other) noexcept;
            ~Series();
        };
        struct Plot
        {
            std::string name;
            std::vector<Series*> series;

            ~Plot();
            void addSeries(QVector<double>& series, const std::string& name);
            void addSeries(QVector<double>& series, const std::string& name, const ImVec4& color, float thickness);
        };
        std::unordered_map<std::string, Plot*> m_plots;
        std::string m_title;
        size_t m_maxPoints = 1000;

        QVector<double>* m_timePoints;

    };
}