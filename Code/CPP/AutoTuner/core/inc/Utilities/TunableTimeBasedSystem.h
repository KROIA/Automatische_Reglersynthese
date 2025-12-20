#pragma once
#include "AutoTuner_base.h"

namespace AutoTuner
{
	class AUTO_TUNER_API TunableTimeBasedSystem : public TimeBasedSystem
	{
		public:
		TunableTimeBasedSystem()
			: TimeBasedSystem()
		{
		}
		TunableTimeBasedSystem(const TunableTimeBasedSystem& other)
			: TimeBasedSystem(other)
		{

		}
		virtual ~TunableTimeBasedSystem()
		{
		}
		
		virtual void setParameters(const std::vector<double>& params) = 0;
		virtual std::vector<double> getParameters() const = 0;
	};
}