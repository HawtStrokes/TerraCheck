#pragma once
#include <string>

// TODO: provide a celcius data rerepresentationm

namespace TerraCheck
{
	struct Insights
	{
		double tempOptimality;
		double frostRisk;
		double heatStressRisk;
		std::string note;
	};

	class Plant;

	class SoilHealthAnalyzer
	{
	private:
		SoilHealthAnalyzer();

	public:
		static SoilHealthAnalyzer& Get();
		Insights Analyze(const std::string& plantName, double temp);
	};
}
