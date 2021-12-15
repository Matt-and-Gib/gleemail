#include "include/display.h"


#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>


Display::Display() {
	lcd = new hd44780_pinIO(RS, EN, D4, D5, D6, D7);

	lcd->begin(16, 2);
	lcd->clear();
}


void Display::print(const char* message, const PRINT_ROW row, const bool retainScreen) {
	lcd->setCursor(0, row);
	wroteLength = lcd->print(message);

	if(!retainScreen) {
		while(wroteLength++ < DISPLAY_ROW_LENGTH) {
			lcd->write(' ');
		}
	}
}


void Display::updateReading(const char* message, const bool retainScreen) {
	print(message, PRINT_ROW::TOP, retainScreen);
}


void Display::updateWriting(const char* message, const bool retainScreen) {
	print(message, PRINT_ROW::BOTTOM, retainScreen);
}


void Display::clearAll() {
	lcd->clear();
}