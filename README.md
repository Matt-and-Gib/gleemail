# gleemail

## Table of Contents

1. [Basic Info](#info)
2. [Components](#parts)
3. [Wiring Diagram](#diagram)
4. [Getting Started in the Code](#gettingstarted)
5. [Usage Instructions](#instructions)
6. [Future Plans](#plans)

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
|1x|10k Ohm Resistor|
|6x|Jumper Wires|
|2x|Crimp Connectors|
|1x|LCD Display|
|1x|HD44780 LCD Controller|
|1x|PCF8574T I/O Expander for I2C|

___

### <a name="diagram"></a>**Wiring Diagram**

To-do

___

### <a name="gettingstarted"></a>**Getting Started in the Code**

To-do

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
