This is an automatic translation, may be incorrect in some places. See sources and examples!

# Autoota
Library for automatic verification of OTA updates of the project with GITHUB and other sources

## compatibility
ESP8266/ESP32

## Content
- [use] (#usage)
- [versions] (#varsions)
- [installation] (# Install)
- [bugs and feedback] (#fedback)

<a id="usage"> </a>

## Usage
- indicate in the firmware the current version and the path to the file with information Project.json
- when updating, download binary and increase the version in the information file
- the program will check the version and offer to update

> The library was conceived to update projects with GitHub, so the default settings for it.You can also be updated from your domain, but the host where the file is stored should be the same where the binary is stored, the library has only one tuning of the host and the port.

`` `CPP
Autoota (Consta Char* Cur_ver, Cost char* Path, const Char* host = "raw.githubusercontent.com", uint16_t port = 443);

// current version
COST Char* Version ();

// Check updates.You can transmit lines to record information
Bool Checkupdate (String* Version = Nullptr, String* Notes = Nullptr);

// there is an update.Call after checking.Will drop in FALSE itself
Bool Hasupdate ();

// Update the firmware from Loop
VOID update ();

// update the firmware now and restart the chip
Bool updatatenow ();

// ticker, call in LOOP.Will return True when trying to update
Bool Tick ();

// there is a mistake
Bool Haserror ();

// Read the error
Error Geterror ();
`` `

### Examples
Local object, update immediately
`` `CPP
Autoota OTA ("1.0", "Gyverlibs/Gyverhub-Example/Main/Project.json");
ifa.checkupdate ()) {
ota.updatatenow ();
}
`` `

Loop update
`` `CPP
Autoota OTA ("1.0", "Gyverlibs/Gyverhub-Example/Main/Project.json");

VOID setup () {
ifa.checkupdate ()) {
ota.update ();
}
}

VOID loop () {
OTA.Tick ();
}
`` `

Receiving info on update
`` `CPP
String Ver, Notes;
ifa.checkupdate (& ver, & notes)) {
Serial.println (Ver);
Serial.println (notes);
}
`` `

## File Project.json
The file contains information about the project and paths to compiled firmware for different platforms in the format used in Gyverhub and Esphome:

`` `json
{
"NAME": "The name of the project",
"ABOUT": "Brief description of the project",
"Version": "1.0",
"Notes": "Comments on update",
"Builds": [
{
"Chipfamily": "ESP8266",
"Parts": [
{
"Path": "https://Rw.GITHUBUSERCONTENT.com/gyverlibs/gyverhub-example/main/bin/firmware.bin",
"Offset": 0
}
]
}
]
}
`` `

### Array Builds
If the project can be launched on different ESP shuts, you can attach a separate binary for each and indicate the path to them.The library itself determines which platform is launched and selects the desired file.The full example with the entire ESP family can be seen [here] (https://github.com/gyverlibs/autoota/blob/main/project.json).

### Parameter Chipfamily
Supported platforms and parameter values ​​`chipfamily`:
- `ESP8266`
- `ESP32`
- `ESP32-C3`
- `ESP32-C6`
- `ESP32-S2`
- `ESP32-S3`
- `ESP32-H2`

### Path PAth
The path should lead to a compiled firmware file.It can be placed both in the repository itself and in releases:

#### in the repository
`` `
https://Rw.GITHUBUSERCONTENT.com/< Account>/< Project>/main/< Pre -Board of the Root Repository>
`` `
Examples:
- Bin
- Firmware.bin
- ESP8266
- Firmware.bin
- ESP32
- Firmware.bin
`` `
https://Rw.GITHUBUSERCONTENT.com/gyverlibs/gyverhub-example/main/bin/firmware.bin
https://Rw.GITHUBUSERCONTENT.COM/gyverlibs/gyverhub-example/Main/BIN/ESP8266/FIRMWARE.BIN
https://Rw.GITHUBUSERCONTENT.com/gyverlibs/gyverhub-example/Main/bin/esp32/firmware.bin
`` `

#### in releases
`` `
https://github.com/ <Account>/<proecial>/Releases/Latest/Download/<file>
`` `
Example:
`` `
https://github.com/gyverlibs/gyverhub-example/releases/latest/download/firmware.bin
`` `

<a id="versions"> </a>

## versions
- V1.0

<a id="install"> </a>
## Installation
- The library can be found by the title ** Autoota ** and installed through the library manager in:
- Arduino ide
- Arduino ide v2
- Platformio
- [download the library] (https://github.com/gyverlibs/autoota/archive/refs/heads/main.zip) .Zip archive for manual installation:
- unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
- unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
- unpack and put in *documents/arduino/libraries/ *
- (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!

<a id="feedback"> </a>

## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code