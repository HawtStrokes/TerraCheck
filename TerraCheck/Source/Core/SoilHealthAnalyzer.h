#pragma once

namespace TerraCheck
{
	struct Insights
	{
		float tempOptimality;
		float frostRisk;
		float heatStressRisk;
		// float microbeRisk;
		// float nutrientAvailabilityRisk;
	};

	class Plant;

	class SoilHealthAnalyzer
	{
	private:
		SoilHealthAnalyzer();

	public:
		static SoilHealthAnalyzer& Get();
		Insights Analyze(const Plant& plant, float temp);
	};
}
