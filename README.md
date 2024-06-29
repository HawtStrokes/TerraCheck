## TerraCheck
A compact and mobile soil health diagnostic tool that uses Arduino Nano.

<img src="https://github.com/HawtStrokes/TerraCheck/blob/main/Resources/sample0.png" width=90% height=90%>

## **Description**

TerraCheck is an approach to soil health assessment designed for field deployment. It integrates an Arduino Nano, sensors, an HC-05 Bluetooth module, a battery pack, and a protective enclosure. The device wirelessly transmits plant-related data to a paired computer, where a dedicated C++ program processes the information to infer insights.

## **Build**

### Cloning

In a terminal, enter the following commands.

```plaintext
git clone --recursive https://github.com/HawtStrokes/TerraCheck.git
cd TerraCheck/
git submodule update --init --recursive 
```

### External
Use C-Make to generate the project files of the following and link appropriately:
 - https://github.com/wjwwood/serial
 - https://github.com/Agamnentzar/bluetooth-serial-port
 - https://github.com/glfw/glfw

## Planned Features

- Add more sensors
- Android/IOS support

## **Dependencies**

- https://github.com/nlohmann/json
- https://github.com/wjwwood/serial
- https://github.com/Agamnentzar/bluetooth-serial-port
- https://github.com/glfw/glfw
- https://github.com/ocornut/imgui
