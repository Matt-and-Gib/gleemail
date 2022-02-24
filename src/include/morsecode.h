#ifndef MORSECODE_H
#define MORSECODE_H

#ifdef UNIT_TEST_MODE
//include moc_inputmethod.h
#else
#include "inputmethod.h"
#include <ArduinoJson.h>
#endif


struct Pin;


class MorseCodeInput final : public InputMethod {
private:
	enum MORSE_CODE_STATE : bool {SWITCH_OPEN = false, SWITCH_CLOSED = true};

	const char MCCP_DATA_VERSION_SERVER_ENDPOINT[72] = "GET /Matt-and-Gib/gleemail/main/data/MorseCodeCharPairsVersion HTTP/1.1";
	const char MCCP_DATA_SERVER_ENDPOINT[70] = "GET /Matt-and-Gib/gleemail/main/data/MorseCodeCharPairs.json HTTP/1.1";

	const char MORSE_CODE_CHAR_PAIRS_PATH[18] = "GLEEMAIL/MCCP.GMD";

	static const unsigned short SWITCH_PIN_LOCATION = 9;

	Pin* pins[3];
	const unsigned short PINS_INDEX_SWITCH = 0;
	const unsigned short PINS_INDEX_LED = 1;

	char* morsePhraseSymbols = nullptr;
	char currentMorsePhrase[7] {0};

	static const constexpr unsigned short DEBOUNCE_THRESHOLD = 25;
	unsigned long lastDebounceTime = 0;

	MORSE_CODE_STATE lastInputState;
	MORSE_CODE_STATE currentInputState;
	unsigned long lastStateChangeTime = 0;
	unsigned long long elapsedCycleTime = 0;
	void updateElapsedTime(const unsigned long&);

	void processClosedToOpen(const unsigned long&);
	void processOpenToClosed(const unsigned long&);
	void pushMorseCharacter(const char&);
	const char& convertPhraseToCharacter();

	void checkOpenElapsedTime(const unsigned long&);
	void checkPhraseElapsedThreshold();
	void checkMessageElapsedThresholds();

	inline void resetMorsePhrase();

	unsigned short filterJsonPayloadSize(const char*) const;
	unsigned short calculateMorsePhraseSymbolsSize(const ArduinoJson::JsonArrayConst&) const;
	unsigned short calculateMorsePhraseIndex(const char* const) const;
public:
	explicit MorseCodeInput(const unsigned short, void (*)(char*), void (*)(char*));
	MorseCodeInput(const MorseCodeInput&) = delete;
	MorseCodeInput(MorseCodeInput&&) = delete;
	MorseCodeInput& operator=(const MorseCodeInput&) = delete;
	MorseCodeInput& operator=(MorseCodeInput&&) = delete;
	~MorseCodeInput();

	const char* getDataVersionRequestEndpoint() const override {return MCCP_DATA_VERSION_SERVER_ENDPOINT;}
	const char* getDataRequestEndpoint() const override {return MCCP_DATA_SERVER_ENDPOINT;}
	bool setNetworkData(const char*) override;

	const char* getCachedDataPath() const override {return MORSE_CODE_CHAR_PAIRS_PATH;}

	Pin** getPins() override {return pins;}
	void processInput(const unsigned long&) override;
};

#endif
