#pragma once
#include <iostream>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

namespace TerraCheck
{
    class Plant
    {
    private:
        nlohmann::json m_Data;

        void Internal_LoadPlantData(const std::string& jsonFilePath)
        {
            std::ifstream file(jsonFilePath);
            if (file.is_open())
            {
                file >> m_Data;
                file.close();
            }
            else
            {
                std::cerr << "Error opening JSON file: " << jsonFilePath << std::endl;
            }
        }

    public:
        Plant(const std::string& jsonFilePath)
        {
            Internal_LoadPlantData(jsonFilePath);
        }

        [[nodiscard]] double GetOptimalTempMin() const
        {
            return m_Data["optimalTempMin"];
        }

        [[nodiscard]] double GetOptimalTempMax() const
        {
            return m_Data["optimalTempMax"];
        }

        [[nodiscard]] double GetFrostRisk() const
        {
            return m_Data["frostRisk"];
        }

        [[nodiscard]] double GetHeatStressRisk() const
        {
            return m_Data["heatStressRisk"];
        }
    };
}
