#ifndef DISPLAY_H
#define DISPLAY_H


#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>


namespace DisplayData {
	static const constexpr char BLANK_LINE[] = "                \0";
}

class Display {
private:
	static const unsigned short DISPLAY_ROW_LENGTH = 16;
	static const unsigned short DISPLAY_COLUMN_LENGTH = 2;
	enum class PRINT_ROW : bool {TOP = true, BOTTOM = false};

	static constexpr int RS = 7;
	static constexpr int EN = 6;
	static constexpr int D4 = 5;
	static constexpr int D5 = 4;
	static constexpr int D6 = 3;
	static constexpr int D7 = 2;
	hd44780_pinIO lcd = hd44780_pinIO(RS, EN, D4, D5, D6, D7);

	char lcdBuffer[DISPLAY_COLUMN_LENGTH][DISPLAY_ROW_LENGTH];

	void print(const char*, const PRINT_ROW, const bool);

	unsigned short wroteLength = 0;
	char* remainingLine;
public:
	Display();
	~Display();
	void updateReading(const char*, const bool);
	void updateWriting(const char*, const bool);
	void clearReading() {updateReading(DisplayData::BLANK_LINE, true);}
	void clearWriting() {updateWriting(DisplayData::BLANK_LINE, true);}
	void clearAll() {lcd.clear();}
};


Display::Display() {
	lcd.begin(16, 2);
	for(unsigned short i = 0; i < DISPLAY_COLUMN_LENGTH; i += 1) {
		for(unsigned short j = 0; j < DISPLAY_ROW_LENGTH; j += 1) {
			lcdBuffer[i][j] = ' ';
		}
	}
}


Display::~Display() {

}


void Display::print(const char* message, const PRINT_ROW row, const bool retainScreen) {
	lcd.setCursor(0, (row == PRINT_ROW::TOP ? 0 : 1));
	wroteLength = lcd.print(message);

	if(!retainScreen) {
		while(wroteLength < DISPLAY_ROW_LENGTH) {
			lcd.write(' ');
			wroteLength += 1;
		}
	}
}


void Display::updateWriting(const char* message, const bool retainScreen = false) {
	print(message, PRINT_ROW::BOTTOM, retainScreen);
}

void Display::updateReading(const char* message, const bool retainScreen = false) {
	print(message, PRINT_ROW::TOP, retainScreen);
}

#endif