![Latest Build Status](https://github.com/Matt-and-Gib/gleemail/actions/workflows/compile.yml/badge.svg)
[![CodeFactor](https://www.codefactor.io/repository/github/matt-and-gib/gleemail/badge/a6-cleanup)](https://www.codefactor.io/repository/github/matt-and-gib/gleemail/overview/a6-cleanup)

![glEEmail Banner](resources/glEEmail_social_media_final.png)

## _Voted† 100% better than Discord!*_

___

## Table of Contents

1. [Basic Info](#info)
2. [Components](#parts)
3. [Wiring Diagram](#diagram)
4. [Error Codes](#errortable)
5. [Getting Started in the Code](#gettingstarted)
6. [Usage Instructions](#instructions)
7. [Startup Codes](#startup-codes)
8. [Future Plans](#plans)
9. [Legal Info](#legal)

___

### <a name="info"></a>**Basic Info**

What is glEEmail? Welcome to the future, kid. glEEmail is like sending your uncle a telegram, except it doesn't cost 25 cents per character. It's easy, and fun (some assembly required).

glEEmail allows you to securely‡ send messages to peers over the Internet via that old Morse Code switch that you have installed next to your computer terminal. Just hook it up and get flyin'!

Okay, for real, glEEmail started as a side project while we were waiting for 200 SMD LEDs to arrive from China. The plan was to whip up some test code allowing us to work with input and output from an Arduino, and to send messages to an Arduino over the Internet. As development went on, glEEmail because significantly more interesting than that excessive-LED project, and we shifted focus to making glEEmail into it's best self. It's fair to say at this point that glEEmail is an incredibly over-engineered curio at best, or a social-networking-service-for-two at worst.
___

### <a name="parts"></a>**Components**

We're actively testing glEEmail for the Arduino Nano 33 IoT, however it should work on any Arduino with WiFi capabilities (or any Arduino connected to a WiFi radio module, with minor code modifications). If you're considering running glEEmail on a different Arduino, please make sure that it has sufficient space; glEEmail uses ~72,000 bytes of program storage space (27% on Nano 33 IoT) and ~15,000 bytes of dynamic memory with global variables (45% on Nano 33 IoT). Additionally, please note that the Adafruit 4682 must be powered with 3.3V (it **cannot** run on 5V).

For the full parts list for a single user, please see the table below:

|Quantity|Component|
|:-------|:--------|
|1x|Nano 33 IoT|
|1x|Breadboard|
|1x|54-410 Switch|
|2x|Crimp Connectors|
|1x|LCD HD44780|
|1x|270 Ω Resistor|
|1x|10k Ω Resistor|
|1x|10k Ω Potentiometer|
|1x|Adafruit 4682|
|1x|Micro SD Card (Format as Fat32)|

___

### <a name="diagram"></a>**Wiring Diagram**

[View Interactive Online Wiring Diagram (dark mode recommended)](https://app.diagrams.net/?src=about#HMatt-and-Gib%2Fgleemail%2Fmain%2Fwiring%20diagram.drawio)

![Wiring Diagram 5-3-2021](https://github.com/Matt-and-Gib/gleemail/raw/main/resources/Wiring%20Diagram%205-3-21.png)

For more information about wiring the LCD HD44780, we recommend [this tutorial from Ada Fruit](https://learn.adafruit.com/character-lcds/wiring-a-character-lcd)

___

### <a name="errortable"></a>**Error Codes**

Last updated: 2/19/2022
<details>
<summary>0 - 9: General</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|0|None|No error was reported||
|1|Debug Debug Log|Verified that debug log is functional||
|2|Unknown Setup State|An unknown step was specified in setup|High|
|3|Continuous Frame Latency|Lag may cause inaccuracies in input|High|
|4|Overflow Errors Lost|Too many errors were received in one cycle|High|
</details>

<details>
<summary>10 - 49: Input Method</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|10|Morse Phrase Imminent Overflow|The Morse phrase is at capacity and must be committed without storing the provided dot/dash|High|
|11|Pushed Char Nothing||High|
|12|Morse Code Lookup Failed|The entered combination of dots and dashes does not match any Morse Code pattern|Low|
|13|Morse Message To Send Reached Chat Complete Threshold|Entered message has reached the maximum length and must be sent immediately|Low|
|14|Morse Message To Send Exceeded Chat Complete Threshold|Entered message has exceeded the maximum length and must be sent immediately without storing the provided character|High|
|17|Commit Empty Message||High|
|18|Version Number Overflow||High|
|19|Morse Code Char Pairs Version Mismatch||Low|
|20|Message Only Whitespace||Low|
|21|Data Version Download Failed||Low|
|22|Data Download Failed||Low|
</details>

<details>
<summary>50 - 89: Network</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|51|Unknown Message Sender||High|
|52|Too Many Messages Received||High|
|53|Heartbeat Flatline|Too many consecutive heartbeats missing|High|
|54|Outgoing Token Timestamp Elapsed||High|
|57|Unknown Incoming Message Type||High|
|58|Confirmation No Match Found||Low|
|59|Duplicate Handshake||Low|
|60|Unexpected Handshake from Connected IP|Either received handshake after token removed from queue (expired) or handshake has unique idempotency token|Low|
|61|Heartbeat Stillborn|Heartbeat never received|High|
|62|Connection Attempt while Connected||Low|
|63|Authentication Failed|UserID and PeerID not unique or peer signature invalid|High|
|64|Getting Messages Took Significantly Longer than Allocated||High|
|65|Getting Messages Took Insignificantly Longer than Allocated||Low|
|66|Processing Incoming Messages Took Significantly Longer than Allocated|Note: this will occur once when first authenticating peer and can be safely ignored|High|
|67|Processing Incoming Messages Took Insignificantly Longer than Allocated||Low|
|68|Sending Messages Took Significantly Longer than Allocated||High|
|69|Sending Messages Took Insignificantly Longer than Allocated||Low|
|70|Received Unauthentic Message|Decryption of chat message revealed it may have been tampered with (message will be discarded)|High|
</details>

<details>
<summary>90 - 129: JSON</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|90|Nullptr Payload|No data provided to deserialize|High|
|91|Prefs Deserialization Error|Could not read prefs file|High|
|92|Message Deserialization Error|Could not read message|High|
|93|Input Data Deserialization Error|Could not read downloaded data|High|
|94|MorseCode Network Data Deserialization Error|Failed parsing downloaded MorseCodeCharPairs|High|
|95|Prefs Filtered Deserialization Error|Failed parsing filtered prefs file|High|
</details>

<details>
<summary>130 - 169: MessageError</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|130|None|No message error||
</details>

<details>
<summary>170 - 209: Internet Access</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|170|Passed Invalid Parameter|SSID or Password invalid|High|
|171|Connection Failed|May attempt retry|High|
|172|WiFi Connection Failed Retry Occurred||High|
|173|Unknown Status||High|
|174|SSID Possibly Truncated|Characters after SSID max length may have been missed|High|
|175|Password Possibly Truncated|Characters after Password max length may have been missed|High|
|176|Disconnected During Connection Attempt||High|
</details>

<details>
<summary>210 - 249: Web Access</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|210|Header Termination Omitted||High|
|211|Download Impossible: Not Connected||High|
|212|Secure Connection To Server Failed|SSL certificate may not be installed (see [Usage Instructions](#instructions))|High|
|213|Request to Server Header Invalid||High|
|214|Data Buffer Overflow||High|
|215|Data Buffer Underutilized||Low|
</details>

<details>
<summary>250 - 289: Storage</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|250|Storage Not Detected|Verify wiring and presence of SD card|Low|
|252|Storage Already Initialized||Low|
|253|Uninitialized Write|Uninitialized SD caused write to fail|High|
|254|Root FS Access Failed||High|
|255|Create glEEmail Root Failed|Unable to create GLEEMAIL/ folder in SD root|High|
|256|Write Open Failed|Writing to SD failed because the file couldn't be opened|High|
|257|Uninitialized Read|Uninitialized SD caused read to fail|High|
|258|Read Open Failed|Reading from SD failed because the file couldn't be opened|High|
|259|Unconfirmed Erase|Attempted to erase SD without confirmation (no data erased)|High|
|260|Uninitialized Erase All|SD was initialized early to allow erasing all data|Low|
|261|Open glEEmail Root For Erase Failed|No data erased because a glEEmail root folder couldn't be found|Low|
|262|glEEmail Root Is File|glEEmail root was file before erase|High|
|263|Uninitialized Erase|Erase single file/folder failed because SD was uninitialized|High|
|264|Storage Object Unsuccessfully Allocated||High|
</details>

<details>
<summary>290 - 329: Display</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|290|RAM Integrity Threatened|Message to print exceeded available display line RAM|High|
</details>

<details>
<summary>330 - 369: Preferences</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|330|Load Failed|Couldn't load preferences from SD card|Low|
|330|File Size Mismatch|Size attribute of preferences file does not match expected size|Low|
|331|File Version Mismatch|Version attribute of preferences file does not match expected version|Low|
</details>

<details>
<summary>370 - 409: Startup Code</summary>

|Code|Title|Notes|Severity|
|----|-----|-----|--------|
|370|Unexpected Serial Input||Low|
|371|Malformed Prompt Trigger|Provided code not recognized (use -A to trigger Startup Codes Prompt)|High|
|372|Too Many Codes Provided||High|
|373|Duplicate Codes Not Allowed||Low|
</details>

___

### <a name="gettingstarted"></a>**Getting Started in the Code**

The first step to becoming a glEEveloper is understanding the project structure. Now, you may think that you already understand the structure because you saw that "src" folder, and you know what that's all about, but I'm here to tell you that there's more to the story. See, Arduino requires a .ino file with the same name as the project to live in the root folder. Now, again, you're like "I already know this," but hang on; if you open the .ino, you might eventually notice that it's entirely empty. Yeah, like, what? Well, there's also a main.cpp file in the root of the project, and _that's_ where the magic happens. Our main.cpp overwrites the implicit main.cpp created by Arduino so that we can control the main loop. Why did we do this? Red-hot performance gains.

Aside from that, all other code files may be found inside `src`. The root folder `src` itself is for source files, `src/include` is for headers, and `src/include/LiteChaCha` is for the encryption library [written by DualJustice](https://github.com/DualJustice/LiteChaCha). We've separated as many files as possible into definition and implementation, but several important files are exclusive to `src/include`, such as data structures like our queue.

Most work will be done on implementation files in the base `src` folder. For an idea of what glEEmail does at runtime, please take a look at our flow diagram below, however for a basic overview, see the following:

> main.cpp : initial setup, then loop control
>
> morsecode.cpp : processInputMethod()
>
> networking.cpp : processNetwork()
>
> display.cpp : updateDisplay()
>
> main.cpp : printErrorCodes()

[View Online Program Flow Diagram (dark mode recommended)](https://app.diagrams.net/?src=about#HMatt-and-Gib%2Fgleemail%2Fmain%2FglEEmail%20flow.drawio)

Required Libraries

- ArduinoJson by Benoit Blanchon
- hd44780 by Bill Perry
- WiFiNINA
- SdFat by Bill Greiman

Note: Do not pass ASCII character 24 (cancel) to `InputMethod::pushCharacterToMessage`, it is reserved to prevent printing erroneous lines.

<details>
<summary>Steps to take after changing certain files</summary>

- preferences.h
  - Increment `PREFERENCES_VERSION` in `src/include/preferences.h`
  - Re-Calculate `CALCULATED_PREFS_SIZE` in `src/include/preferences.h`

- MorseCodeCharPairs.json (note: don't forget to update `SERVER` and `SERVER_REQUEST` URLs to your forked repo)
  - Increment `morseCodeCharPairsVersion` in `data/MorseCodeCharPairsVersion`
  - Re-Calculate `CALCULATED_MCCP_DOCUMENT_SIZE_IN_BYTES` in `data/MorseCodeCharPairs.json`

</details>
___

### <a name="instructions"></a>**Usage Instructions**

Important!

In order to use glEEmail's Morse Code functionality, you will need to install the SSL certificate for `raw.githubusercontent.com` because the Morse-Code-to-character conversion uses JSON data hosted in this repository. To install the certificate, open the Arduino IDE, click on tools, then `WiFi101 / WiFiNINA Firmware Updater`, and make sure that your Arduino is selected in Step 1 of the Firmware Updater (Select the port of the WiFi Module).

![Install Certificate Picture One.png](resources/InstallCertificatePictureOne.png)

![Install Certificate Picture Two.png](resources/InstallCertificatePictureTwo.png)

Click on Open Updater Sketch, and upload the project that appears to your Arduino. Next, (back in the Firmware Updater) skip down to Step 3 of the Firmware Updater, click on Add domain, and paste the following: `raw.githubusercontent.com`.

![Install Certificate Picture Three.png](resources/InstallCertificatePictureThree.png)

Click OK, and you should see "raw.githubusercontent.com:443" in the table below "arduino.cc:443". Now, click on "Upload Certificates to WiFi Module." If you receive an error that the Programmer is not responding, make sure that the uploader sketch is uploaded on the Arduino first by clicking OK, then clicking "Open uploader sketch" back in Step 1 of the Firmware Updater, and uploading that to your Arduino before uploading any certificates.

![Install Certificate Picture Four.png](resources/InstallCertificatePictureFour.png)

#### How to Connect with Your Friend

If you've configured the hardware correctly, simply upload the code to your Arduino, open the serial monitor, and follow the prompts on screen (the LCD and serial monitor). If you haven't yet wired the hardware, first refer to the [Wiring Diagram](#diagram).

#### How to Morse Like a Pro

To do...

[How to input](https://morsecode.world/international/morse2.html)

[How to stay on beat](https://morsecode.world/international/timing.html)

[THE Standard](https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf)

#### Unexpected Behavior?

Experiencing unexpected behavior? Maybe we've run into your problem before you!
The first step we'd advise you take if anything unexpected happens is to turn on Verbose Mode and try it again. Please see [startup codes](#startup-codes) for further instructions.

If that doesn't solve your problem, submit an [issue on GitHub](https://github.com/Matt-and-Gib/gleemail/issues) and we will take a look at it (_should the stars align_).

#### Features

- 57 ~~flavors~~ morse code characters
- You can hold down the switch to pause input
- DDoS protection built-in!
- Encrypted chat so you can't be spied upon‡
- Necessary downloads are cached to reduce network impact
- Learn Morse Code
- Easily send SOS when necessary‡
- Be social
- Be a glEEpal

**_IMPORTANT NOTE:_** You are limited to sending no more than ~32,768 messages per session due to the size of the idempotency token (unsigned short- i.e. 2 bytes) and the necessity of confirmation messages. This decision was made to reduce message overhead and speed up encrypting and parsing. We understand that this limit will affect most users, and deeply apologize for the issue. There is simply nothing that we can do to change this, so just remember to power-cycle your glEEware every once in a while.
___

### <a name="startup-codes"></a>**Startup Codes**

As soon as possible at launch, send **-A** via Serial to trigger the startup code prompt. Once directed, provide a string consisting of single-letter, case-sensitive arguments to alter or enable additional behavior in glEEmail. The built-in arguments are listed below:

<!-- NOTE: Keep the following list in alphabetical order with all lowercase first -->

|Code|Behavior|Component|Notes|
|----|--------|---------|-----|
|`[DEL]`|None|_core_|Reserved for system use|
|`k`|Print DSA keys|Networking|Confirm your keys out-of-band to ensure a secure connection|
|`v`|Enable Verbose Debug Mode|_core_|Low-severity errors will be sent via Serial (high-severity errors will always be sent)|
|`I`|Enable Incoming-Only Mode|Display|Only incoming messages will be displayed (hide your output)|
|`O`|Enable Offline Mode|??? (todo)|No network connections will be made (there's no shame in a solo glEEmail session)|
|`R`|Remove glEEmail data from SD card|Storage|Lost data is unrecoverable by glEEmail- be certain that you want to do this|

A usage example is provided below. Note that `-A` is entered as soon as glEEmail launches, then the desired arguments, `tvR`, are entered after "Enter startup codes:" arrives via Serial.

``` plaintext
Welcome to glEEmail!
Version alpha 6
-A
Enter Startup Codes: Iv
Joining WiFi
...
```

___

### <a name="plans"></a>**Future Plans**

We'd like to implement additional input methods, including (but not limited to):

1. Phone app that connects directly to the Arduino, which can also import/export saved data.
2. Speech-to-text
3. Light-level (?)
4. Web app
5. Dance-dance-revolution
6. Game controller
7. Touch screen keyboard
8. Single-button random messages (magic 8 ball style)

Other than that, please view [our project board](https://github.com/orgs/Matt-and-Gib/projects/4/views/1) for an idea of what's next.

___

### <a name="legal"></a>**Legal Info**

glEEmail is Open Source with no promise of support! Please feel free to contribute by submitting pull requests or issues. Contact us if you have any questions (or want to glEE-chat).

`†by glEEmail developers`

`*in terms of hardware** support`

`**Arduino`

`‡use this novel toy at your own risk and only for entertainment purposes - do not use glEEmail in emergency situations!`
