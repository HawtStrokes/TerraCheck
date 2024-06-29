#include "Plant.h"

#include <iostream>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

TerraCheck::Plant* TerraCheck::Plant::m_Instance = nullptr;

void TerraCheck::Plant::Internal_LoadPlantData(const std::string& jsonFilePath)
{
	std::ifstream file(jsonFilePath);
	if (file.is_open())
	{
		file >> m_PlantsData;
		file.close();
	}
	else
	{
		std::cerr << "Error opening JSON file: " << jsonFilePath << std::endl;
		throw std::exception("Cannot Load Plant Data");
	}
}

TerraCheck::Plant::Plant(const std::string& jsonFilePath)
{
	Internal_LoadPlantData(jsonFilePath);
}

void TerraCheck::Plant::InitPlant(const std::string& plantFile)
{
	if (m_Instance != nullptr)
		throw std::exception("GuiApp already Initialized!");
	else
	{
		static Plant instance(plantFile);
		m_Instance = &instance;
	}
}

TerraCheck::Plant& TerraCheck::Plant::Get()
{
	if (m_Instance == nullptr)
		throw std::exception("GuiApp Not Initialized! Call InitGui first");
	else  return *m_Instance;
}

//nlohmann::json TerraCheck::Plant::GetPlantData(const std::string& plantName)
//{
//	return m_PlantsData[plantName];
//}

std::vector<std::string> TerraCheck::Plant::GetAllPlantNames() const
{
	std::vector<std::string> plantNames;
	for (const auto& plant : m_PlantsData.items())
	{
		plantNames.push_back(plant.key());
	}
	return plantNames;
}


double TerraCheck::Plant::GetOptimalTempMin(const std::string& plantName) const
{
	return m_PlantsData[plantName]["optimalTempMin"];
}

double TerraCheck::Plant::GetOptimalTempMax(const std::string& plantName) const
{
	return m_PlantsData[plantName]["optimalTempMax"];
}

double TerraCheck::Plant::GetFrostRisk(const std::string& plantName) const
{
	return m_PlantsData[plantName]["frostRisk"];
}

double TerraCheck::Plant::GetHeatStressRisk(const std::string& plantName) const
{
	return m_PlantsData[plantName]["heatStressRisk"];
}

std::string TerraCheck::Plant::GetNote(const std::string& plantName) const
{
	return m_PlantsData[plantName]["note"];
}
