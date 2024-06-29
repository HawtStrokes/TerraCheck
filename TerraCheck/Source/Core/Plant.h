#pragma once

#include "nlohmann/json.hpp"

namespace TerraCheck
{
    class Plant
    {
    private:
        nlohmann::json m_PlantsData;
        static Plant* m_Instance;

    private:
        void Internal_LoadPlantData(const std::string& jsonFilePath);
        Plant(const std::string& jsonFilePath);

    public:
        static void InitPlant(const std::string& plantFile);
        static Plant& Get();

    public:
        // nlohmann::json GetPlantData(const std::string& plantName);

        [[nodiscard]] std::vector<std::string> GetAllPlantNames() const;

        [[nodiscard]] double GetOptimalTempMin(const std::string& plantName) const;

        [[nodiscard]] double GetOptimalTempMax(const std::string& plantName) const;

        [[nodiscard]] double GetFrostRisk(const std::string& plantName) const;

        [[nodiscard]] double GetHeatStressRisk(const std::string& plantName) const;

        [[nodiscard]] std::string GetNote(const std::string& plantName) const;
    };
}
