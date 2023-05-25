#ifndef Glyphs_h
#define Glyphs_h

// https://arduinogetstarted.com/faq/how-to-use-special-character-on-lcd

class Glyph{
private:
  byte _glyph[8];

public:
  Glyph(byte* glyph){
    for (uint8_t i = 0; i < 8; i++){
      _glyph[i] = glyph[i];
    }
  }
};

byte GLYPH_DEGREES_SYMBOL[8] = {
	0b00110,
	0b01001,
	0b01001,
	0b00110,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

byte GLYPH_BACK_ARROW[8] = {
	0b00100,
	0b01100,
	0b11111,
	0b01101,
	0b00101,
	0b00001,
	0b00001,
	0b00001
};

byte GLYPH_LEFT_ARROW[8] = {
	0b00001,
	0b00011,
	0b00111,
	0b01111,
	0b00111,
	0b00011,
	0b00001,
	0b00000
};

byte GLYPH_RIGHT_ARROW[8] = {
	0b10000,
	0b11000,
	0b11100,
	0b11110,
	0b11100,
	0b11000,
	0b10000,
	0b00000
};

#endif