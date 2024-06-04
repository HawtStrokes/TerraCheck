#include "Core/TerraSerial.h"
#include "Core/SoilHealthAnalyzer.h"
#include "Core/Plant.h"

// TODO: GUI

using namespace TerraCheck;

int main()
{
    try
    {
        TerraSerial::EnumeratePorts();
        std::string portName;
        std::cout << "Enter serial port name: ";
        std::getline(std::cin, portName);

        // Connect to the serial port
     
        serial::Serial ser;
        ser.setPort(portName);
        ser.setBaudrate(9600);
        ser.open();
        if (ser.isOpen())
        {
            std::cout << "Connected to " << portName << " at 9600 bps.\n";
        }
        else
        {
            std::cerr << "Failed to open " << portName << ".\n";
            return 1;
        }

        // Load data for a specific plant (e.g., Rose)
        std::string plantName;
        std::cout << "Enter plant name: ";
        std::getline(std::cin, plantName);

        // Extract data for the specified plant
        Plant plant(plantName + ".json");

        // Get SoilHealthAnalyzer instance
        SoilHealthAnalyzer& shaInstance = SoilHealthAnalyzer::Get();
    	std::string tempStr;
    	ser.readline(tempStr);
        const float currentTemp = std::stof(tempStr);

        // Analyze soil health
        const Insights insights = shaInstance.Analyze(plant, currentTemp);
        std::cout << "Temp Optimality: " << insights.tempOptimality << "\n";
        std::cout << "Frost Risk: " << insights.frostRisk << "\n";
        std::cout << "Heat Stress Risk: " << insights.heatStressRisk << "\n";
        std::cin.get();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}