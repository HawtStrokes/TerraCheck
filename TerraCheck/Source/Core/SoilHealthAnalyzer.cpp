#include "SoilHealthAnalyzer.h"

#include "Plant.h"

using namespace TerraCheck;

SoilHealthAnalyzer::SoilHealthAnalyzer() = default;

SoilHealthAnalyzer& SoilHealthAnalyzer::Get()
{
	static SoilHealthAnalyzer instance;
	return instance;
}

Insights SoilHealthAnalyzer::Analyze(const std::string& plantName, double temp)
{
	Insights result;
	const auto plantInstance = Plant::Get();
	const double frostRisk = plantInstance.GetFrostRisk(plantName);
	const double heatStressRisk = plantInstance.GetHeatStressRisk(plantName);
	const double optimalTempMin = plantInstance.GetOptimalTempMin(plantName);
	const double optimalTempMax = plantInstance.GetOptimalTempMax(plantName);
	const double optimalTemp = (optimalTempMax + optimalTempMin) / 2;



	auto sigmoid = [](double x) -> double {
	 return 1.0 / (1.0 + std::exp(-x));
	};

	auto scaler = [](double a, double b, double x) -> double
	{
		const double range = std::abs(a - b);
		return x / range;
	};

	// 41 degrees fahrenheit is 5 degrees celcius

	const double heatX = scaler(-41, 41, (temp - heatStressRisk));
	const double frostX = scaler(-41, 41, (frostRisk - temp));
	const double optimalTempX = scaler(-41, 41,(temp - optimalTemp));

	result.heatStressRisk = sigmoid(heatX);
	result.frostRisk = sigmoid (frostX);
	result.tempOptimality = sigmoid(optimalTempX);
	result.note = plantInstance.GetNote(plantName);

    return result;
}
