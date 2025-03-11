# iot-with-wokwi
Đang tìm bản [Tiếng Việt?](README_vi.md)

Use [Wokwi for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode) to simulate this project.
## Building

This is a [PlatformIO](https://platformio.org) project. To build it, [install PlatformIO](https://docs.platformio.org/en/latest/core/installation/index.html), and then run the following command:

```
pio run
```

## About ThingSpeak

Create a new channel with 3 fields
Replace ThingSpeak API key and Channel ID.

```cpp
const char* thingSpeakHost = "api.thingspeak.com";
String thingSpeakApiKey = "YOUR_API_KEY_HERE"; // thingSpeak API key
unsigned long thingSpeakChannelID = YOUR_CHANNEL_ID_HERE; // channel ID
```

## Simulating

To simulate this project, install [Wokwi for VS Code](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode). Open the project directory in Visual Studio Code, press **F1** and select "Wokwi: Start Simulator".

Once the simulation is running, open http://localhost:8180 in your web browser to interact with the simulated HTTP server.
