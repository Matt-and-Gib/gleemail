#ifndef TILTTYPE_H
#define TILTTYPE_H

#include "inputmethod.h"

class TiltType : public InputMethod {
public:
	const Pin *getPins() const {return pins;}
private:
	const Pin pins[1] = {NULL_PIN};
};

#endif