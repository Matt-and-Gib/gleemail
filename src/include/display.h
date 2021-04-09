#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal.h>


namespace DisplayData {
	static const constexpr char BLANK_LINE[] = "                \0";
}

class Display {
private:
	enum class PRINT_ROW : bool {TOP = true, BOTTOM = false};

	static constexpr int RS = 7;
	static constexpr int EN = 6;
	static constexpr int D4 = 5;
	static constexpr int D5 = 4;
	static constexpr int D6 = 3;
	static constexpr int D7 = 2;
	LiquidCrystal lcd = LiquidCrystal(RS, EN, D4, D5, D6, D7);

	void print(const char*, const PRINT_ROW);

public:
	Display();
	~Display();
	void updateReading(const char*);
	void updateWriting(const char*);
	void clearReading() {updateReading(DisplayData::BLANK_LINE);}
	void clearWriting() {updateWriting(DisplayData::BLANK_LINE);}
};


Display::Display() {
	lcd.begin(16, 2);
}


Display::~Display() {

}


void Display::print(const char* message, const PRINT_ROW row) {
	lcd.setCursor(0, (row == PRINT_ROW::TOP ? 0 : 1));
	lcd.print(message);
}


void Display::updateWriting(const char* message) {
	print(message, PRINT_ROW::BOTTOM);
}

void Display::updateReading(const char* message) {
	print(message, PRINT_ROW::TOP);
}

#endif