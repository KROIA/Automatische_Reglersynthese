#pragma once

/// USER_SECTION_START 1

/// USER_SECTION_END

#include "AutoTuner_info.h"

/// USER_SECTION_START 2
//#include "MatlabEngine.hpp"
//#include "MatlabDataArray.hpp"
#include "MatlabAPI.h"
#include "QSFML_EditorWidget.h"

#include "Experiment/Experiment.h"
#include "Experiment/Scope.h"


#include "Components/ZieglerNichols.h"
#include "Components/ChartViewComponent.h"
#include "Components/NyquistPlotComponent.h"

#include "GameObjects/Solver.h"
#include "GameObjects/GeneticSolver.h"
#include "GameObjects/DifferentialEvolutionSolver.h"

#include "Utilities/TimeBasedSystem.h"
#include "Utilities/TunableTimeBasedSystem.h"
#include "Utilities/StatespaceSystem.h"
#include "Utilities/PID.h"
#include "Utilities/CSVExport.h"
#include "Utilities/FrequencyResponse.h"

/// USER_SECTION_END