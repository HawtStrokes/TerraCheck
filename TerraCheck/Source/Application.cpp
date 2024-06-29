#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "imgui.h"
#include "DeviceINQ.h"
#include "BTSerialPortBinding.h"
#include "BluetoothException.h"
#include "serial/serial.h"

#include "Core/SoilHealthAnalyzer.h"
#include "Core/Plant.h"
#include "Gui/GuiApp.h"

// TODO: Remove unneccessary nested branches and use logical AND

using namespace TerraCheck;

namespace TerraCheck
{
    // GUI & Misc
    ImVec2 g_WindowDimensions = { 800, 520 };
	ImGuiTextBuffer g_LogBuf;
    bool showAbout = false;
    bool showHelp = false;
    // bool showResults = false;
    bool isBluetooth = false;
    // auto startTime = std::chrono::steady_clock::now();

    // Communication
    bool isConnected;

    // Physical Serial
	std::string selectedPort;
	std::vector<std::string> availablePorts;
    bool isPhyConnected = false;
    serial::Serial ser;


    // Bluetooth
    std::string selectedBtDevice;
    std::vector<device> btDevices;
    unsigned selectedBtDeviceIdx;
    std::unique_ptr<BTSerialPortBinding> btBinding;
    bool isBTConnected = false;
    device btDev;

    /// TODO: Make the ReadDataThread non-temp specific to seamlessly allow the quick integration of new sensor modules.
    /// TODO: More detailed logging
    ///         - Cannot make new connection when an exising connection has already been established


    // Analysis
	std::string selectedPlant;
    std::vector<std::string> plantOptions;
    std::atomic<double> readTemperature{ 0.0 }; // Atomic for thread safety
    // std::atomic<bool> isTempValid = false;
    std::mutex temperatureMutex; // Mutex for protecting readTemperature

    namespace Core
    {
        // GUI Log Function that writes to the log window's collapsing header.
        void Log(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);

            char buffer[256];
            vsnprintf(buffer, sizeof(buffer), fmt, args);

            std::string tempLog(buffer);

            // Insert a newline character every 100 characters
            constexpr int maxLineLength = 100;
            for (size_t i = maxLineLength; i < tempLog.size(); i += maxLineLength + 1) // +1 to account for the newline character
            {
                tempLog.insert(i, "\n");
            }

            // Update the main log buffer
            g_LogBuf.appendf("%s\n", tempLog.c_str());
            va_end(args);
        }

        // Temperature reading thread
	    void ReadDataThread()
	    {
            auto& guiInstance = WrapGui::GuiApp::Get();
            // Loop so long as the main run loop is still true
            while (*guiInstance.GetRunStatePtr())
            {
                // Bluetooth data reading
                if(isBTConnected && btBinding->IsDataAvailable())
                {
                    // Update readTemperature (protected by mutex)
                    {
                        std::lock_guard<std::mutex> lock(temperatureMutex);
                        char buffer[512];
                        btBinding->Read(buffer, 512);
                        std::string temp(buffer);
                        readTemperature = std::stod(temp);
                        //isTempValid = true;
                    }
                }
                // Physical data reading
                else if (isPhyConnected && ser.isOpen() && ser.available())
                {
                    std::string tempStr;
                    ser.readline(tempStr);
                    if (!tempStr.empty())
                    {
                        // Update readTemperature (protected by mutex)
                        const double currentTemp = std::stof(tempStr);
                        std::lock_guard<std::mutex> lock(temperatureMutex);
                        readTemperature = currentTemp;
                        //isTempValid = true;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
	    }

	    void ScanPorts()
	    {
            // TerraSerial::EnumeratePorts();
	        availablePorts.clear(); // Clear existing entries
	        const std::vector<serial::PortInfo> devices_found = serial::list_ports();

	        for (const auto& device : devices_found)
	        {
	            availablePorts.push_back(device.port);
	        }
	    }

        void ScanPortsBT()
	    {
		    const std::unique_ptr<DeviceINQ> inq(DeviceINQ::Create());
            btDevices = inq->Inquire();

            for (const auto& d : btDevices)
            {
                std::cout << d.name << " " << d.address << std::endl;
                Core::Log("%s %s", d.name.c_str(), d.address.c_str());
            }
            std::cout << std::endl << "Found " << btDevices.size() << " device(s)" << std::endl;
            Core::Log("Found %d device(s).", btDevices.size());
	    }

    }


    // UI Components
    namespace GuiComponents
    {
        // Information
        static void About()
        {
            ImGui::Begin("About", &showAbout);
            ImGui::TextWrapped("TerraCheck ver0.1a");
            ImGui::End();
        }
        static void Help()
        {
            ImGui::Begin("Help", &showHelp);
            if (ImGui::Button("Copy Link to Official GitHub Page"))
            {
                ImGui::SetClipboardText("https://github.com/HawtStrokes/TerraCheck");
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Navigation and Layout"))
            {
                const std::string strNavHelp = R"(There are two dock spaces, and each window can be docked to any of the two dock spaces or be left floating. 

To dock a window to a dock space, simply drag the title bar of a window to one of the two dock spaces.
)";
                ImGui::TextWrapped("%s", strNavHelp.c_str());
            }

            if (ImGui::CollapsingHeader("Analysis"))
            {
                const std::string analysisHelp = R"(Two perform a soil analysis, a connection must first be made with the TerraCheck device, this can be done via physical Serial communication or Bluetooth in their appropriate windows.

The probe of the TerraCheck device must then be stuck into the surrounding soil of a specified plant.

After the prior steps have been performed, go to the analysis window, select the plant that is beein analyzed and click the analyze button to retrieve the inferences gathered.
)";
                ImGui::TextWrapped("%s", analysisHelp.c_str());
            }

            ImGui::End();
        }

        // Data Inference
        static void Analysis()
        {
            ImGui::Begin("Analysis");
            // Display temperature (Used a separate thread for fetching temp info)
            {
                std::lock_guard<std::mutex> lock(temperatureMutex);
                ImGui::Text("Current Temperature: %.2f", readTemperature.load());
            }

            // Dropdown for plant selection
            if (ImGui::BeginCombo("Select Plant", selectedPlant.c_str()))
            {
                for (const auto& plant : plantOptions)
                {
	                const bool isSelected = (selectedPlant == plant);
                    if (ImGui::Selectable(plant.c_str(), isSelected))
                    {
                        selectedPlant = plant;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            static bool showResults = false;
            // Analyze button
            if (ImGui::Button("Analyze") && !selectedPlant.empty())
			{
                showResults = true;
			}
            if (showResults)
            {
                // Get SoilHealthAnalyzer instance
                SoilHealthAnalyzer& shaInstance = SoilHealthAnalyzer::Get();

                const Insights insights = shaInstance.Analyze(selectedPlant, readTemperature);
                ImGui::Separator();
                // Display in a panel or column layout
                ImGui::BeginGroup();

                // Plant name
                ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.2f, 1.0f), "Results for %s:", selectedPlant.c_str());

                // Temperature information
                ImGui::Text("Temp Optimality: %.2f", insights.tempOptimality);

                // Frost risk
                ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.8f, 1.0f), "Frost Risk: %.2f", insights.frostRisk);

                // Heat stress risk
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Heat Stress Risk: %.2f", insights.heatStressRisk);

                ImGui::Separator();

                ImGui::TextWrapped("%s", insights.note.c_str());

                ImGui::EndGroup();

            }
            ImGui::End();
        }

        // Communication
        static void ConnectPhysicalWindow()
        {
            ImGui::Begin("Physical");
            // Scan button
             if (ImGui::Button("Scan Ports")) Core::ScanPorts();

            // Port Selection Combo
             if (ImGui::BeginCombo("Select Port", selectedPort.c_str()))
             {
                 for (const auto& port : availablePorts)
                 {
	                 const bool isSelected = (selectedPort == port);
                     if (ImGui::Selectable(port.c_str(), isSelected))
                     {
                         selectedPort = port;
                     }
                     if (isSelected)
                     {
                         ImGui::SetItemDefaultFocus();
                     }
                 }
                 ImGui::EndCombo();
             }

             // Not connected to a physical device, connection possible.
             if (!isPhyConnected)
             {
                 if (ImGui::Button("Connect Phy") && !isConnected)
                 {
                    // Try to Connect if we aren't connected to anything (BT or Phy)
                     try
                     {
                         if (!selectedPort.empty())
                         {
							ser.setPort(selectedPort);
							ser.setBaudrate(9600);
							auto timeOut = serial::Timeout::simpleTimeout(3000);
							ser.setTimeout(timeOut);
							ser.open();
							isPhyConnected = true;

							if (ser.isOpen())
							{
								std::cout << "Connected to " << selectedPort << " at 9600 bps.\n";
								Core::Log("Connected to %s at 9600 bps.", selectedPort.c_str());
							}
							else
							{
								std::cout << "Failed to open " << selectedPort << ".\n";
								Core::Log("Failed to open %s.", selectedPort.c_str());
							}
							}
							else
							{
								std::cout << "Select a COM Port." << std::endl;
								Core::Log("Select a COM Port.");
							}
                     }
                     catch (const std::exception& e)
                     {
                         std::cerr << "Unhandled Exception: " << e.what() << std::endl;
                         Core::Log("Unhandled Exception: %s", e.what());
                     }
                 }
             }
             // Connected to a Phy device, disconnection possible.
             else
             {
                 if (ImGui::Button("Disconnect Phy"))
                 {
					ser.close();
                 	isPhyConnected = false;
                    std::cout << "Disconnected from Physical Serial Connection: " << selectedPort << std::endl;
                    Core::Log("Disconnected from device: %s", selectedPort.c_str());
                 }
             }
         	ImGui::End();
        }
    	static void ConnectBTWindow()
        {
            ImGui::Begin("Bluetooth");
            // Scan button
            if (ImGui::Button("Scan Bluetooth Devices")) Core::ScanPortsBT();
            if (ImGui::BeginCombo("Select Bluetooth Device", selectedBtDevice.c_str()))
            {
                unsigned idx = 0;
                for (const auto& device : btDevices)
                {
	                const bool isSelected = (selectedBtDevice == device.name);
                    if (ImGui::Selectable(device.name.c_str(), isSelected))
                    {
                        selectedBtDevice = device.name;
                        selectedBtDeviceIdx = idx;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    ++idx;
                }
                ImGui::EndCombo();
            }

            // Not connected to a bluetooth device, connection possible.
            if (!isBTConnected)
            {
                if (ImGui::Button("Connect BT") && !isConnected)
                {
                    // Try to Connect if we aren't connected to anything (BT or Phy)
                    try
                    {
                        if (!selectedBtDevice.empty())
                        {
                            btDev = btDevices[selectedBtDeviceIdx];

                            // Connect to the first device found (you can choose a specific device)
                            
                        	btBinding = std::unique_ptr<BTSerialPortBinding>(BTSerialPortBinding::Create(btDev.address, 1));


                            // Establish the connection
                            try
                            {
                                btBinding->Connect();
	                            isBTConnected = true;
	                            std::cout << "Connected to device: " << btDev.name << std::endl;
	                            Core::Log("Connected to device: %s", btDev.name.c_str());
                            }
                            catch (const BluetoothException& e)
                            {
                                std::cerr << "Unhandled Exception: " << e.what() << std::endl;
                                Core::Log("Unhandled Exception: %s", e.what());
                            }
                            
                        }
                        else
                        {
                            std::cout << "Select a device." << std::endl;
                            Core::Log("Select a device");
                        }
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
                        Core::Log("Unhandled Exception: %s", e.what());
                    }
                }
            }

            // Connected to a bluetooth deivde, disconnection possible.
            else
            {
				if(ImGui::Button("Disconnect BT"))
				{
                    btBinding->Close();
                    std::cout << "Disconnected from device: " << btDev.name << std::endl;
                    Core::Log("Disconnected from device: %s", btDev.name.c_str());
                    isBTConnected = false;
				}
            }

            ImGui::End();
        }

        // Logging
        static void LogWindow()
        {
            ImGui::Begin("Log Window");
            if (ImGui::Button("Clear"))
                g_LogBuf.clear();
            if (ImGui::CollapsingHeader("Logs", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("%s", g_LogBuf.begin());
            }
            ImGui::End();
        }
    	
    }

    // GUI Main Function, called within each render loop.
    void GUIMain()
    {
        auto& guiInstance = WrapGui::GuiApp::Get();
        ImGui::SetNextWindowSize(g_WindowDimensions);
        ImGui::Begin("TerraCheck Analyzer", guiInstance.GetRunStatePtr(), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

        // TODO: stop constant assignment and only change the value when it needs to be changed. (OPTIMIZATION.)
        isConnected = isBTConnected || isPhyConnected;

        // Menu Bar
    	if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("About"))
                {
                    showAbout = true;
                }
                if (ImGui::MenuItem("Exit"))
                {
                    guiInstance.Close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("View Help"))
                {
                    showHelp = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(g_WindowDimensions.x - 20, g_WindowDimensions.y / 2));
        ImGui::DockSpace(ImGui::GetID("LogDockSpace"), ImVec2(g_WindowDimensions.x - 20, (g_WindowDimensions.y / 2) - 40));
        ImGui::SetNextWindowDockID(ImGui::GetID("MainDockSpace"), ImGuiConfigFlags_DockingEnable);
        GuiComponents::Analysis();

        ImGui::SetNextWindowDockID(ImGui::GetID("MainDockSpace"), ImGuiConfigFlags_DockingEnable);
        GuiComponents::ConnectPhysicalWindow();

        ImGui::SetNextWindowDockID(ImGui::GetID("MainDockSpace"), ImGuiConfigFlags_DockingEnable);
        GuiComponents::ConnectBTWindow();

        ImGui::SetNextWindowDockID(ImGui::GetID("LogDockSpace"), ImGuiConfigFlags_DockingEnable);
        GuiComponents::LogWindow();

        if (showAbout)
        {
            GuiComponents::About();
        }

        if (showHelp)
        {
            GuiComponents::Help();
        }
        
        ImGui::End();
    }
}


int main()
{
    // GUI Setup
    WrapGui::GuiApp::InitGui({ "", 1, 1 });
    auto& guiInstance = WrapGui::GuiApp::Get();
    guiInstance.HideWindow();
    guiInstance.SetMain(GUIMain);

    // Initialize Plant Data and Options for combo box selection
    Plant::InitPlant("plants.json");
    const auto& plantInstance = Plant::Get();
    plantOptions = plantInstance.GetAllPlantNames();

    // Run data gathering functionalities in a separate thread
	std::thread tempThread(TerraCheck::Core::ReadDataThread);

    // Start Main Loop
    guiInstance.Run();

    // Exit
    tempThread.join();
    return 0;
}