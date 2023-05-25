#include "LCDMenu.h"
  
void LCDLine::setTarget(LCDScreen* target){
  _target = target;
}

void LCDLine::setClickFunction(std::function<void()> clickFunction){
  _clickFunction = clickFunction;
}

void LCDLine::setResetTarget(bool resetTarget){
  _resetTarget = resetTarget;
}

void LCDLine::setDecimalPlaces(uint8_t places){
  _floatDecimalPlaces = places;
}

bool LCDLine::isLocked(){
  return false;
}

void LCDLine::addToParent(){
  _parent->addLine(this);
}

bool LCDLine::onClick(LCDMenu* menu){
  bool clicked = false;
  if (_clickFunction){
    _clickFunction();
    clicked = true;
  }
  if (_target){
    menu->setScreen(_target, false);
    if (_resetTarget) _target->reset();
    clicked = true;
  }
  return clicked;
}

bool LCDLine::onUp(){
  return false;
}

bool LCDLine::onDown(){
  return false;
}

bool LCDLine::onHold(){
  return false;
}

void LCDLine::getOutput(hd44780_I2Cexp* lcd){
  if (SERIAL_LCD) Serial.print(_text);
  else lcd->print(_text);
  for(uint8_t i = 0; i < _numVariables; i++){
    if(SERIAL_LCD) printVariableSerial(i);
    else printVariableLCD(lcd, i);
  }
}

void LCDLine::printVariableSerial(uint8_t index) {
	switch (_variableTypes[index]) {
	    // Variables -----
		case DataType::CONST_CHAR_PTR: {
			const char* variable = reinterpret_cast<const char*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case CONST_CHAR_PTR
		case DataType::CHAR_PTR: {
			char* variable = const_cast<char*>(reinterpret_cast<const char *>(_variables[index]));
			Serial.print(variable);
			break;
		} //case CHAR_PTR
		case DataType::CHAR: {
			const char variable = *static_cast<const char*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case CHAR

		case DataType::INT8_T: {
			const int8_t variable = *static_cast<const int8_t*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case INT8_T
		case DataType::UINT8_T: {
			const uint8_t variable = *static_cast<const uint8_t*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case UINT8_T

		case DataType::INT16_T: {
			const int16_t variable = *static_cast<const int16_t*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case INT16_T
		case DataType::UINT16_T: {
			const uint16_t variable = *static_cast<const uint16_t*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case UINT16_T

		case DataType::INT32_T: {
			const int32_t variable = *static_cast<const int32_t*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case INT32_T
		case DataType::UINT32_T: {
			const uint32_t variable = *static_cast<const uint32_t*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case UINT32_T

		case DataType::FLOAT: {
			const float variable = *static_cast<const float*>(_variables[index]);
			Serial.print(variable, _floatDecimalPlaces);
			break;
		} //case FLOAT

		case DataType::BOOL: {
			const bool variable = *static_cast<const bool*>(_variables[index]);
			Serial.print(variable);
			break;
		} //case BOOL

		case DataType::GLYPH: {
			const uint8_t variable = *static_cast<const uint8_t*>(_variables[index]);
			Serial.write((uint8_t)variable);
			break;
		} //case BOOL

		case DataType::PROG_CONST_CHAR_PTR: {
			const char* variable = reinterpret_cast<const char*>(_variables[index]);
			volatile const int len = strlen_P(variable);
			char buffer[len];
			for (uint8_t i = 0; i < len; i++) {
				buffer[i] = pgm_read_byte_near(variable + i);
			}
			buffer[len] = '\0';
			Serial.print(buffer);
			break;
		} //case PROG_CONST_CHAR_PTR
	    // ~Variables -----

	    // Getter functions -----
		case DataType::CONST_CHAR_PTR_GETTER: {
			const constcharPtrFnPtr getterFunction = reinterpret_cast<constcharPtrFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				const char * variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case CONST_CHAR_PTR_GETTER

		case DataType::CHAR_PTR_GETTER: {
			const charPtrFnPtr getterFunction = reinterpret_cast<charPtrFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				char* variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case CHAR_PTR_GETTER

		case DataType::CHAR_GETTER: {
			const charFnPtr getterFunction = reinterpret_cast<charFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				char variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case CHAR_GETTER

		case DataType::INT8_T_GETTER: {
			const int8tFnPtr getterFunction = reinterpret_cast<int8tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				int8_t variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case INT8_T_GETTER

		case DataType::UINT8_T_GETTER: {
			const uint8tFnPtr getterFunction = reinterpret_cast<uint8tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				uint8_t variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case UINT8_T_GETTER

		case DataType::INT16_T_GETTER: {
			const int16tFnPtr getterFunction = reinterpret_cast<int16tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				int16_t variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case INT16_T_GETTER

		case DataType::UINT16_T_GETTER: {
			const uint16tFnPtr getterFunction = reinterpret_cast<uint16tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				uint16_t variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case UINT16_T_GETTER

		case DataType::INT32_T_GETTER: {
			const int32tFnPtr getterFunction = reinterpret_cast<int32tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				int32_t variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case INT32_T_GETTER

		case DataType::UINT32_T_GETTER: {
			const uint32tFnPtr getterFunction = reinterpret_cast<uint32tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				uint32_t variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case UINT32_T_GETTER

		case DataType::FLOAT_GETTER: {
			const floatFnPtr getterFunction = reinterpret_cast<floatFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				const float variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case FLOAT_GETTER

		case DataType::BOOL_GETTER: {
			const boolFnPtr getterFunction = reinterpret_cast<boolFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				bool variable = (getterFunction)();
				Serial.print(variable);
			} 
			break;
		} // case BOOL_GETTER
	    // ~Getter functions -----

		default: { break; }
	} //switch (_variablesType)
}

void LCDLine::printVariableLCD(hd44780_I2Cexp* lcd, uint8_t index) {
	switch (_variableTypes[index]) {
	    // Variables -----
		case DataType::CONST_CHAR_PTR: {
			const char* variable = reinterpret_cast<const char*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case CONST_CHAR_PTR
		case DataType::CHAR_PTR: {
			char* variable = const_cast<char*>(reinterpret_cast<const char *>(_variables[index]));
			lcd->print(variable);
			break;
		} //case CHAR_PTR
		case DataType::CHAR: {
			const char variable = *static_cast<const char*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case CHAR

		case DataType::INT8_T: {
			const int8_t variable = *static_cast<const int8_t*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case INT8_T
		case DataType::UINT8_T: {
			const uint8_t variable = *static_cast<const uint8_t*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case UINT8_T

		case DataType::INT16_T: {
			const int16_t variable = *static_cast<const int16_t*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case INT16_T
		case DataType::UINT16_T: {
			const uint16_t variable = *static_cast<const uint16_t*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case UINT16_T

		case DataType::INT32_T: {
			const int32_t variable = *static_cast<const int32_t*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case INT32_T
		case DataType::UINT32_T: {
			const uint32_t variable = *static_cast<const uint32_t*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case UINT32_T

		case DataType::FLOAT: {
			const float variable = *static_cast<const float*>(_variables[index]);
			lcd->print(variable, _floatDecimalPlaces);
			break;
		} //case FLOAT

		case DataType::BOOL: {
			const bool variable = *static_cast<const bool*>(_variables[index]);
			lcd->print(variable);
			break;
		} //case BOOL

		case DataType::GLYPH: {
			const uint8_t variable = *static_cast<const uint8_t*>(_variables[index]);
			lcd->write((uint8_t)variable);
			break;
		} //case BOOL

		case DataType::PROG_CONST_CHAR_PTR: {
			const char* variable = reinterpret_cast<const char*>(_variables[index]);
			volatile const int len = strlen_P(variable);
			char buffer[len];
			for (uint8_t i = 0; i < len; i++) {
				buffer[i] = pgm_read_byte_near(variable + i);
			}
			buffer[len] = '\0';
			lcd->print(buffer);
			break;
		} //case PROG_CONST_CHAR_PTR
	    // ~Variables -----

	    // Getter functions -----
		case DataType::CONST_CHAR_PTR_GETTER: {
			const constcharPtrFnPtr getterFunction = reinterpret_cast<constcharPtrFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				const char * variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case CONST_CHAR_PTR_GETTER

		case DataType::CHAR_PTR_GETTER: {
			const charPtrFnPtr getterFunction = reinterpret_cast<charPtrFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				char* variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case CHAR_PTR_GETTER

		case DataType::CHAR_GETTER: {
			const charFnPtr getterFunction = reinterpret_cast<charFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				char variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case CHAR_GETTER

		case DataType::INT8_T_GETTER: {
			const int8tFnPtr getterFunction = reinterpret_cast<int8tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				int8_t variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case INT8_T_GETTER

		case DataType::UINT8_T_GETTER: {
			const uint8tFnPtr getterFunction = reinterpret_cast<uint8tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				uint8_t variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case UINT8_T_GETTER

		case DataType::INT16_T_GETTER: {
			const int16tFnPtr getterFunction = reinterpret_cast<int16tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				int16_t variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case INT16_T_GETTER

		case DataType::UINT16_T_GETTER: {
			const uint16tFnPtr getterFunction = reinterpret_cast<uint16tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				uint16_t variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case UINT16_T_GETTER

		case DataType::INT32_T_GETTER: {
			const int32tFnPtr getterFunction = reinterpret_cast<int32tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				int32_t variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case INT32_T_GETTER

		case DataType::UINT32_T_GETTER: {
			const uint32tFnPtr getterFunction = reinterpret_cast<uint32tFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				uint32_t variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case UINT32_T_GETTER

		case DataType::FLOAT_GETTER: {
			const floatFnPtr getterFunction = reinterpret_cast<floatFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				const float variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case FLOAT_GETTER

		case DataType::BOOL_GETTER: {
			const boolFnPtr getterFunction = reinterpret_cast<boolFnPtr>(_variables[index]);
			if (getterFunction != nullptr) {
				bool variable = (getterFunction)();
				lcd->print(variable);
			} 
			break;
		} // case BOOL_GETTER
	    // ~Getter functions -----

		default: { break; }
	} //switch (_variablesType)
}


LCDEditLine::LCDEditLine(LCDScreen* parent,
                        char* text,
                        long value,
                        long min,
                        long max,
                        long step,
                        std::function<std::string(long)> translationFunction,
                        std::function<void()> applyFunction,
                        LCDScreen* target)
                        : LCDLine(parent, text){
  _value = value;
  _min = min;
  _max = max;
  _step = step;
  _translationFunction = translationFunction;
  _applyFunction = applyFunction;
  _editing = false;
  if (_value < _min) _value = _min;
  if (_value > _max) _value = _max;
  setTarget(target);
}

bool LCDEditLine::onClick(LCDMenu* menu){
  LCDLine::onClick(menu);
  _editing = !_editing;
  if (!_editing && _applyFunction != nullptr){
    _applyFunction();
  }
  return true;
}

bool LCDEditLine::onUp(){
  if (!_editing) return false;
  if (_value + _step <= _max) _value = _value + _step;
  return true;
}

bool LCDEditLine::onDown(){
  if (!_editing) return false;
  if (_value - _step >= _min) _value = _value - _step;
  return true;
}

bool LCDEditLine::onHold(){
  _editing = false;
  return false;
}

bool LCDEditLine::isLocked(){
  return _editing;
}

void LCDEditLine::getOutput(hd44780_I2Cexp* lcd){
  if (SERIAL_LCD){
    Serial.print(_text);
    if (_translationFunction) Serial.print(_translationFunction(_value).c_str());
    else Serial.print(_value);
  } else {
    lcd->print(_text);
    if (_translationFunction) lcd->print(_translationFunction(_value).c_str());
    else lcd->print(_value);
  }
}

void LCDEditLine::setValue(long value){
  _value = value;
}

long LCDEditLine::getValue(){
  return _value;
}

void LCDEditLine::setEditing(bool editing){
  _editing = editing;
}

void LCDEditLine::setStep(long step){
  _step = step;
}


LCDChoiceLine::LCDChoiceLine(LCDScreen* parent,
                            uint8_t numChoices,
                            char** choices,
                            char* text,
                            std::function<void()> applyFunction,
                            uint8_t defaultChoice,
                            LCDScreen* target)
                            : LCDEditLine(parent,
                                          text,
                                          defaultChoice,
                                          0,
                                          numChoices-1,
                                          1,
                                          nullptr, 
                                          applyFunction,
                                          target){
  for (uint8_t i = 0; i < MAX_CHOICE_CHOICES; i++) {
    _choices[i] = choices[i];
  }
}

void LCDChoiceLine::getOutput(hd44780_I2Cexp* lcd){
  if (_value >= MAX_CHOICE_CHOICES) return;
  if (SERIAL_LCD){
    Serial.print(_text);
    Serial.print(_choices[_value]);
  } else {
    lcd->print(_text);
    lcd->print(_choices[_value]);
  }
}

char* LCDChoiceLine::getChoice(){
  if (_value >= MAX_CHOICE_CHOICES) return "Error";
  return _choices[_value];
}







LCDScreen::LCDScreen(){
  _cursor = 0;
  _scroll = 0;
  for (uint8_t i = 0; i < MAX_SCREEN_LINES; i++) _lines[i] = nullptr;
  _numLines = 0;
  _cursorStyle = CURSOR_RIGHT;
}


void LCDScreen::up(uint8_t rows, bool wrap){
  if (_numLines > 0 && _lines[_cursor]->onUp()) return;
  if (wrap) _cursor = (_cursor + 1) % _numLines;
  else if (_cursor < _numLines - 1) _cursor++;
  else if (_cursor > _numLines - 1) _cursor = _numLines - 1;
  adjustScroll(rows);
}

void LCDScreen::down(uint8_t rows, bool wrap){
  if (_numLines > 0 && _lines[_cursor]->onDown()) return;
  if (wrap) _cursor = (_cursor - 1) % _numLines;
  else if (_cursor > 0) _cursor--;
  else if (_cursor < 0) _cursor = 0;
  adjustScroll(rows);
}

void LCDScreen::adjustScroll(uint8_t rows){
  if (_cursor < _scroll) _scroll = _cursor;
  else if (_cursor > _scroll + rows - 1) _scroll = _cursor - rows + 1;
}

bool LCDScreen::click(LCDMenu* menu){
  if (_numLines) return _lines[_cursor]->onClick(menu);
  return false;
}

void LCDScreen::hold(){
  if (_numLines) _lines[_cursor]->onHold();
}

void LCDScreen::print(hd44780_I2Cexp* lcd, uint8_t rows){
  lcd->clear();
  char* cursor;
  for (uint8_t i = 0; i < _numLines && i < rows; i++){
    lcd->setCursor(0, i);
    if (_cursorStyle == CURSOR_HIDDEN) cursor = "";
    else if (_cursor == _scroll + i){
      if (_lines[_scroll + i]->isLocked()){
        if (_cursorStyle == CURSOR_LEFT) cursor = ">";
        else cursor = "<";
      } else {
        if (_cursorStyle == CURSOR_LEFT) cursor = "*"; //"▶"
        else cursor = "*"; //"◀"
      }
    } else {
      cursor = " ";
    }
    if (_cursorStyle == CURSOR_RIGHT){
      _lines[_scroll + i]->getOutput(lcd);
      if (SERIAL_LCD) Serial.print(cursor);
      else lcd->print(cursor);
    } else {
      if (SERIAL_LCD) Serial.print(cursor);
      else lcd->print(cursor);
      _lines[_scroll + i]->getOutput(lcd);
    }
    if (SERIAL_LCD) Serial.print("\n");
  }
  if (SERIAL_LCD){
    Serial.println();
    Serial.println();
  }
}

void LCDScreen::addLine(LCDLine* line){
  if (_numLines >= MAX_SCREEN_LINES) return;
  _lines[_numLines] = line;
  _numLines++;
}

void LCDScreen::hideCursor(bool hide){
  if (hide) _cursorStyle = CURSOR_HIDDEN;
  else _cursorStyle = CURSOR_RIGHT;
}

void LCDScreen::leftCursor(){
  _cursorStyle = CURSOR_LEFT;
}

void LCDScreen::rightCursor(){
  _cursorStyle = CURSOR_RIGHT;
}

void LCDScreen::reset(){
  _cursor = 0;
  _scroll = 0;
}



LCDSubScreen::LCDSubScreen(LCDScreen* parent, char* name) : LCDScreen(){
  _parent = parent;
  _towards = new LCDLine(_parent, name);
  _towards->setTarget(this);
  _backwards = new LCDLine(this, "<- Back"); // ↰
  _backwards->setTarget(parent);
  _backwards->setResetTarget(false);
}

void LCDSubScreen::setOnEntryClickFunction(std::function<void()> clickFunction){
  _towards->setClickFunction(clickFunction);
}



LCDMenu::LCDMenu(uint8_t cols, uint8_t rows, hd44780_I2Cexp* lcd){
  _cols = cols;
  _rows = rows;
  _lcd = lcd;
  _screen = nullptr;
  _root = nullptr;
  _wrap = false;
}

void LCDMenu::click(){
  if (!_screen) return;
  _screen->click(this);
  update();
}

void LCDMenu::up(){
  if (!_screen) return;
  _screen->up(_rows, _wrap);
  update();
}

void LCDMenu::down(){
  if (!_screen) return;
  _screen->down(_rows, _wrap);
  update();
}

void LCDMenu::hold(){
  if (_screen) _screen->hold();
  if (_root) setScreen(_root);
}

void LCDMenu::update(){
  if (_screen) _screen->print(_lcd, _rows);
}

void LCDMenu::setScreen(LCDScreen* screen, bool update){
  _screen = screen;
  if(update) this->update();
}

void LCDMenu::setRoot(LCDScreen* root){
  _root = root;
  update();
}

void LCDMenu::setWrapping(bool wrap){
  _wrap = wrap;
}

LCDScreen* LCDMenu::getScreen(){
  return _screen;
}
