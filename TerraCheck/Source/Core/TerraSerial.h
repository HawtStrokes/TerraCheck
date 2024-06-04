#pragma once

#include <string>
#include <iostream>
#include <cstdio>


#include <serial/serial.h>

namespace TerraCheck
{
	namespace TerraSerial
	{
		inline void EnumeratePorts()
		{
		    std::vector<serial::PortInfo> devices_found = serial::list_ports();

		    auto iter = devices_found.begin();

		    while (iter != devices_found.end())
		    {
		        serial::PortInfo device = *iter++;

		        printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
		            device.hardware_id.c_str());
		    }
		}

		/*void Connect(const std::string& port, uint32_t baud_rate)
		{
			serial::Serial ser;
			try
			{
				ser.setPort(port);
				ser.setBaudrate(baud_rate);
				ser.open();
				if (ser.isOpen())
				{
					std::cout << "Connected to " << port << " at " << baud_rate << " bps.\n";
				}
				else
				{
					std::cerr << "Failed to open " << port << ".\n";
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
			}
		}*/

		inline std::string Read(serial::Serial& ser)
		{
			try
			{
				return ser.read(ser.available());
			}
			catch (const std::exception& e)
			{
				std::cerr << "Read exception: " << e.what() << "\n";
				return "";
			}
		}
		
		inline void Write(serial::Serial& ser, const std::string& data)
		{
			try
			{
				ser.write(data);
			}
			catch (const std::exception& e)
			{
				std::cerr << "Write exception: " << e.what() << "\n";
			}
		}
	}
}