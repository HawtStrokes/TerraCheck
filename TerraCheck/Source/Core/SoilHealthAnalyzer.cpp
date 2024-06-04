#include "SoilHealthAnalyzer.h"

#include "Plant.h"

using namespace TerraCheck;

SoilHealthAnalyzer::SoilHealthAnalyzer() = default;

SoilHealthAnalyzer& SoilHealthAnalyzer::Get()
{
	static SoilHealthAnalyzer instance;
	return instance;
}

Insights SoilHealthAnalyzer::Analyze(const Plant& plant, float temp)
{
    Insights result;

    // Calculate tempOptimality via linear interpolation
    result.tempOptimality = (temp - plant.GetOptimalTempMin()) /
        (plant.GetOptimalTempMax() - plant.GetOptimalTempMin());

    // Calculate frostRisk and heatStressRisk via linear scaling
    result.frostRisk = 1.0 - result.tempOptimality;
    result.heatStressRisk = result.tempOptimality;

    return result;
}
