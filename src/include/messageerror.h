#ifndef MESSAGE_ERROR
#define MESSAGE_ERROR

//MessageError will cause seg faults when deleted if dynamic memory has not been allocated (You can only delete what yo have new'd!) ☜(ﾟヮﾟ☜)
/lass MessageError {
private:
	ERROR_CODE id;
	const char* attribute;

	static const constexpr unsigned short MAX_ATTRIBUTE_SIZE = 1;
public:
	MessageError() {
		id = ERROR_CODE::MESSAGE_ERROR_NONE;
		attribute = new char[MAX_ATTRIBUTE_SIZE];
		//attribute[0] = '\0';
	}
	MessageError(const ERROR_CODE c, const char* a) {
		id = c;
		attribute = a;
	}
	MessageError(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument) {
		const unsigned short tempErrorID = parsedDocument["E"]["D"];
		id = static_cast<ERROR_CODE>(tempErrorID);

		const char* tempAttribute = parsedDocument["E"]["A"];
		attribute = copyString(tempAttribute, MAX_ATTRIBUTE_SIZE);
	}
	~MessageError() {
		delete[] attribute;
	}

	const ERROR_CODE getID() const {return id;}
	const char* getAttribute() const {return attribute;}
};

#endif