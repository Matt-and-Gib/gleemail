#ifndef DISPLAY_H
#define DISPLAY_H

#include "corecomponent.h"
#include "startupcodehandler.h"


#include "Arduino.h" //delete me!


class hd44780_pinIO;


class Display final : public CoreComponent, public StartupCodeHandler {
private:
	unsigned short RS = 7;
	unsigned short EN = 6;
	unsigned short D4 = 5;
	unsigned short D5 = 4;
	unsigned short D6 = 3;
	unsigned short D7 = 2;
	hd44780_pinIO* const lcd;

	char*& topLineBuffer;

	bool incomingOnly = false;
	const char INCOMING_ONLY_STARTUP_CODE = 'I';
	void enableIncomingOnlyMode() {incomingOnly = true;} //TODO: Finish me

	static const constexpr unsigned short DISPLAY_ROW_LENGTH = 16;
	static const constexpr unsigned short DISPLAY_COLUMN_LENGTH = 2;
	enum PRINT_ROW : bool {TOP = false, BOTTOM = true};

	const char BLANK_LINE[17] = "                "; //Maybe generate this at startup based on DISPLAY_ROW_LENGTH? An abstract Display class cannot depend on a fixed line length.
	const unsigned short MAX_LINE_LENGTH = 40; //HD44780 RAM size for each line

	//char lcdBuffer[DISPLAY_COLUMN_LENGTH][DISPLAY_ROW_LENGTH]; //Maybe used for scrolling messages?

	void print(const char*, const PRINT_ROW, const bool);

	unsigned short wroteLength = 0;

	bool scrollReading = false;
	bool scrollWriting = false;
public:
	explicit Display(char*&);
	Display(const Display&) = delete;
	Display(Display&&) = delete;
	Display& operator=(const Display&) = delete;
	Display& operator=(Display&&) = delete;
	~Display() = default;

	void Update() override;

	void registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>&) override;
	void startupCodeReceived(void (StartupCodeHandler::*)(void)) override;

	void updateReading(const char* message, const bool retainScreen = false);
	void updateWriting(const char* message, const bool retainScreen = false);
	void clearReading() {updateReading(BLANK_LINE, true);}
	void clearWriting() {updateWriting(BLANK_LINE, true);}
	void clearAll();
	void scrollDisplay();
};

#endif