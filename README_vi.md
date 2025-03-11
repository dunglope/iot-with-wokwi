# iot-with-wokwi
Sử dụng [Wokwi cho Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode) để chạy project này.
## Dựng app

Đây là một project sử dụng [PlatformIO](https://platformio.org). Để bắt đầu sử dụng, [cài đặt PlatformIO](https://docs.platformio.org/en/latest/core/installation/index.html), rồi chạy câu lệnh sau:

```
pio run
```

## Về ThingSpeak

Tạo một channel mới với 3 field
Thay đổi ThingSpeak API key và Channel ID sau khi đã tạo.

```cpp
const char* thingSpeakHost = "api.thingspeak.com";
String thingSpeakApiKey = "YOUR_API_KEY_HERE"; // thingSpeak API key
unsigned long thingSpeakChannelID = YOUR_CHANNEL_ID_HERE; // channel ID
```

## Sử dụng

Để chạy project, cài đặt [Wokwi cho VS Code](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode). Mở thư mục chứa project trong Visual Studio Code, nhấn **F1** và chọn "Wokwi: Start Simulator".

Khi project đã bắt đầu chạy, mở trình duyệt và truy cập http://localhost:8180 để tương tác với HTTP server.
