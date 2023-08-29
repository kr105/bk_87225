
# bk_87225

Custom firmware for this chinese doorbell

![device picture](device.jpg?raw=true)

Labeled as:
- 83225EPC-WIFI
- Video Intercom Camera

It has a very good hardware configuration and good enough build quality, but it comes with Tuya firmware so 🤮.

#### For now I have implemented :
- Access to the raw H264 stream coming from the device camera.
- Access to the raw PCM audio coming from the microphone.
- Support for GPIO events (currently only the "call" button is implemented).
- Send MQTT notifications to a broker when the "call" button is pressed.

#### Missing features:
- Speakers.
- RFID/NFC reader.
- Touch keyboard.
- Electronic door-lock connection.

This repo is not meant as a public release usable project, at least not yet. At some point in the future, I will upload proper flashing instructions and add customization and other features. For now, you can use it as a reference or anything else (MIT Licensed).
 
#### GPIO info
 
| GPIO # | Direction | Description |
|--|--|--|
| 36 | OUT | Keypad backlight |
| 43 | OUT | Door lock relay |
| 44 | IN | Call button |
| 70 | OUT | Call button backlight, white |
| 71 | OUT | Call button backlight, red |
| 72 | OUT | Call button backlight, green |
| 73 | IN | ? |
| 200 | OUT | Camera flashlight LEDs |
| 201 | OUT | ? |
