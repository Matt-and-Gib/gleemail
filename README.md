# gleemail

## Table of Contents

1. [Basic Info](#info)
2. [Components](#parts)
3. [Wiring Diagram](#diagram)
4. [Error Codes](#errortable)
5. [Getting Started in the Code](#gettingstarted)
6. [Usage Instructions](#instructions)
7. [Future Plans](#plans)

___

### <a name="info"></a>**Basic Info**

What is glEEmail? Welcome to the future, kid. glEEmail is like sending your uncle a telegram, except it doesn't cost 25 cents per character. It's easy, and fun (some assembly required).

glEEmail allows you to send messages to (predefined) clients over the Internet via that old Morse Code switch that you have installed next to your computer terminal. Just hook it up and get flyin'!

Okay, for real, glEEmail is a side project while we're waiting for 200 SMD LEDs to arrive from China. The project allows you to test input and output from your Arduino, and to send messages to an Arduino at a different IP address as long as both can connect to the Internet.
___

### <a name="parts"></a>**Components**

We're testing glEEmail on an Arduino Nano 33 IoT. For the full parts list that we used (for one user), please see the table below:

|Quantity|Component|
|-------------|:---|
|1x|Nano 33 IoT|
|1x|Breadboard|
|1x|54-410 Switch|
|1x|10k Ω Resistor|
|2x|Crimp Connectors|
|1x|LCD HD44780|
|1x|270 Ω Resistor|
|1x|10k Ω Potentiometer|

___

### <a name="diagram"></a>**Wiring Diagram**

[View Online Wiring Diagram (dark mode recommended)](https://app.diagrams.net/?src=about#HMatt-and-Gib%2Fgleemail%2FNetwork-JSON-Rewrite%2Fwiring%20diagram.drawio)

*** Note: update link to use main branch!

For more information about wiring the LCD, we recommend [this tutorial from Ada Fruit](https://learn.adafruit.com/character-lcds/wiring-a-character-lcd)

___

### <a name="errortable"></a>**Error Codes**

Last updated: 4/13/2021
<details>
<summary>0 - 9: General</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|0|None|No error was reported|N/A|
|1|Debug Debug Log|Verified that debug log is functional|N/A|
|2|Unknown Setup State|An unknown step was specified in setup|High|
</details>

<details>
<summary>10 - 49: Input</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|10|Morse Phrase Imminent Overflow|The Morse phrase is at capacity and must be committed without storing the provided dot/dash|High|
|12|Morse Code Lookup Failed|The entered combination of dots and dashes does not match any Morse Code pattern|Low|
|13|Morse Message To Send Reached Max Message Length|Entered message has reached the maximum length and must be sent immediately|Low|
|14|Morse Message To Send Exceeded Max Message Length|Entered message has exceeded the maximum length and must be sent immediately without storing the provided character|High|
|15|Message Contains Preceding Whitespace||Low|
|16|Message Contains Trailing Whitespace||Low|
</details>

<details>
<summary>50 - 89: Network</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|50|Connection Failed||High|
|51|Passed Invalid Parameter|Provided SSID or password were null pointers|High|
|52|WiFi Connection Failed Retry Occurred||High|
|53|Download Impossible Not Connected||High|
|54|Secure Connection to Server Failed||High|
|55|Request To Server Header Invalid||High|
|56|Header Termination Omitted||High|
|57|Data Buffer Overflow||High|
|58|Data Buffer Underutilized|The calculated space for the data buffer was too large (likely data download was incomplete)|Low|
|59|SSID Possibly Truncated|WiFi name provided exceeded maximum length (as defined by IEEE 802.11) and was truncated|High|
|60|Password Possibly Truncated|WiFi password exceeded maximum length (as defined by IEEE 802.11) and was truncated|High|
|61|Invalid Handshake Message||High|
|62|Unexpected Handshake IP|Handshake received but not from user-entered IP address|Low|
|63|Invalid Peer IP Address||High|
|64|Unknown Status|Connection to WiFi router is neither FAILED nor CONNECTED|High|
|65|Write Failed|Sending message to peer failed|High|
|66|Peer Message Read Failed||High|
|67|Unknown Message Type||High|
</details>

<details>
<summary>90 - 129: JSON</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|90|Nullptr Payload|No data provided to deserialize|High|
|91|Deserialization Error|Unable to deserialize data into JSON objects|High|
</details>

___

### <a name="gettingstarted"></a>**Getting Started in the Code**

To-do

[View Online Program Flow Diagram (dark mode recommended)](https://app.diagrams.net/?src=about#HMatt-and-Gib%2Fgleemail%2FNetwork-JSON-Rewrite%2FglEEmail%20flow.drawio)

*** Note: update link to use main branch!

Note: Do not use ASCII character 24 (cancel), it is reserved to prevent printing erroneous lines.

___

### <a name="instructions"></a>**Usage Instructions**

Important!

In order to use gleemail's Morse Code functionality, you will need to install the SSL certificate for `raw.githubusercontent.com` because the Morse-Code-to-character conversion uses a binary tree populated with JSON data hosted in this repository. To install the certificate, open the Arduino IDE, click on tools, then `WiFi101 / WiFiNINA Firmware Updater`, and make sure that your Arduino is selected in Step 1 of the Firmware Updater (Select the port of the WiFi Module).

![Install Certificate Picture One.png](resources/InstallCertificatePictureOne.png)

![Install Certificate Picture Two.png](resources/InstallCertificatePictureTwo.png)

Click on Open Updater Sketch, and upload the project that appears to your Arduino. Next, (back in the Firmware Updater) skip down to Step 3 of the Firmware Updater, click on Add domain, and paste the following: `raw.githubusercontent.com`.

![Install Certificate Picture Three.png](resources/InstallCertificatePictureThree.png)

Click OK, and you should see "raw.githubusercontent.com:443" in the table below "arduino.cc:443". Now, click on "Upload Certificates to WiFi Module." If you receive an error that the Programmer is not responding, make sure that the uploader sketch is uploaded on the Arduino first by clicking OK, then clicking "Open uploader sketch" back in Step 1 of the Firmware Updater, and uploading that to your Arduino before uploading any certificates.

![Install Certificate Picture Four.png](resources/InstallCertificatePictureFour.png)

• How to connect with your friend

• To Do

• Features

• You can hold down the switch to pause input

___

### <a name="plans"></a>**Future Plans**

We'd like to implement additional input methods, including (but not limited to):

1. Phone app that connects directly to the Arduino, which also stores past messages.
2. Speech-to-text
3. Light-level
4. Web app
5. Dance-dance-revolution input
6. Game controller
7. Touch screen keyboard
8. Single-button random messages (magic 8 ball style)
