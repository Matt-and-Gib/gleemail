#ifndef DISPLAY_H
#define DISPLAY_H


class hd44780_pinIO;


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
	hd44780_pinIO* lcd;

	char lcdBuffer[DISPLAY_COLUMN_LENGTH][DISPLAY_ROW_LENGTH];

	void print(const char*, const PRINT_ROW, const bool);

	unsigned short wroteLength = 0;
	char* remainingLine;
public:
	Display();
	~Display();
	void updateReading(const char* message, const bool retainScreen = false);
	void updateWriting(const char* message, const bool retainScreen = false);
	void clearReading() {updateReading(DisplayData::BLANK_LINE, true);}
	void clearWriting() {updateWriting(DisplayData::BLANK_LINE, true);}
	void clearAll();
};

#endif