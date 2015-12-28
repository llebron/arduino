#include "NeoPixelRing.h"

NeoPixelRing::NeoPixelRing(uint16_t arg_size, uint8_t pin) {
	size = arg_size;
	maxIndex = size-1;
	ring.updateLength(size);
	ring.setPin(pin);
	lightActiveStatusAbsolute = new bool[size]{false}; 
	pixels = new NeoPixel[size];
}

NeoPixelRing::~NeoPixelRing() {
	delete[] lightActiveStatusAbsolute;
	delete[] pixels;
}

void NeoPixelRing::update() {
	// process the spin offset first, so values will be processed accurately
	// N.B. - as spinoffset updates, use spinOffset % size to keep it in simple bounds

}

uint16_t NeoPixelRing::getAbsoluteIndexFromCurrentIndex(uint16_t index) {
	return getWrappedIndex(index - spinOffset);
}

/**
	Accepts any index value and calculates the proper value by "wrapping" it 
	within the allowable bounds for the ring indices (0, size-1)
*/
uint16_t NeoPixelRing::getWrappedIndex(uint16_t index)
{
	// accounts for negative values
	if (index < 0)
        index += size * ((-index) / size + 1);

    return index % size;
}
