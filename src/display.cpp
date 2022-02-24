#include "include/display.h"
#include "include/global.h"

#include "include/keyvaluepair.h"
#include "include/queue.h"

#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>


Display::Display() : lcd{new hd44780_pinIO(RS, EN, D4, D5, D6, D7)} {
	lcd->begin(16, 2);
	clearAll();
}


void Display::registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>& startupCodeHandlers) {
	startupCodeHandlers.enqueue(new KVPair<const char&, StartupCodeHandlerData* const>(INCOMING_ONLY_STARTUP_CODE, new StartupCodeHandlerData(this, reinterpret_cast<void (StartupCodeHandler::*)(void)>(&Display::enableIncomingOnlyMode))));
}


void Display::startupCodeReceived(void (StartupCodeHandler::*memberFunction)(void)) {
	(this->*(reinterpret_cast<void (Display::*)(void)>(memberFunction)))();
}


void Display::print(const char* message, const PRINT_ROW row, const bool retainScreen) {
	if(strlen(message) > MAX_LINE_LENGTH) { //Surely, we have already calculated this somewhere.
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::DISPLAY_RAM_INTEGRITY_THREATENED);
		return;
	}

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
	scrollReading = false;
	scrollWriting = false;
}


void Display::scrollDisplay() {
	//How to do this without adding significant processing every frame (comparing against millis, for example)?
}