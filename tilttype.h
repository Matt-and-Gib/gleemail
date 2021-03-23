#ifndef TILTTYPE_H
#define TILTTYPE_H

#include "inputdevice.h"

class TiltType : public InputDevice {
public:
	const Pin *getPins() const {return pins;}
private:
	const Pin pins[1] = {NULL_PIN};
};

#endif