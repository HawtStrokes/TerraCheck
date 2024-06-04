## TerraCheck
A compact and mobile soil health diagnostic tool that uses Arduino Nano.

## **Description**

TerraCheck is an approach to soil health assessment designed for field deployment. It integrates an Arduino Nano, sensors, an HC-05 Bluetooth module, a battery pack, and a protective enclosure. The device wirelessly transmits soil data to a paired computer, where a dedicated C++ program processes the information to infer insights.

## **Build**

### Cloning

In a terminal, enter the following commands.

```plaintext
git clone --recursive https://github.com/HawtStrokes/TerraCheck.git
cd TerraCheck/
git submodule update --init --recursive 
```

## Planned Features

- Add more sensors
- Implement GUI
- Android/IOS support

## **Dependencies**

- https://github.com/nlohmann/json
- https://github.com/wjwwood/serial
