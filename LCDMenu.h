#ifndef LCDMenu_h
#define LCDMenu_h

#include <cstdint>
#include <string>
#include <functional>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // Bill Perry
#include "Config.h"

typedef bool (*boolFnPtr)();
typedef int8_t (*int8tFnPtr)();
typedef uint8_t (*uint8tFnPtr)();
typedef int16_t (*int16tFnPtr)();
typedef uint16_t (*uint16tFnPtr)();
typedef int32_t (*int32tFnPtr)();
typedef uint32_t (*uint32tFnPtr)();
typedef float (*floatFnPtr)();
typedef double (*doubleFnPtr)();
typedef char (*charFnPtr)();
typedef char * (*charPtrFnPtr)();
typedef const char * (*constcharPtrFnPtr)();

enum class DataType : uint8_t {
  NOT_USED = 0,
  BOOL = 1, BOOLEAN = 1,
  INT8_T = 8,
  UINT8_T = 9,
  INT16_T = 16,
  UINT16_T = 17,
  INT32_T = 32,
  UINT32_T = 33,
  FLOAT = 50, DOUBLE = 50,
  CHAR = 60,
  CHAR_PTR = 61,
  CONST_CHAR_PTR = 62,
  PROG_CONST_CHAR_PTR = 65,
  GLYPH = 70,
  FIRST_GETTER = 200,
  BOOL_GETTER = 201, BOOLEAN_GETTER = 201,
  INT8_T_GETTER = 208,
  UINT8_T_GETTER = 209, BYTE_GETTER = 209,
  INT16_T_GETTER = 216,
  UINT16_T_GETTER = 217,
  INT32_T_GETTER = 232,
  UINT32_T_GETTER = 233,
  FLOAT_GETTER = 240, DOUBLE_GETTER = 240,
  CHAR_GETTER = 250,
  CHAR_PTR_GETTER = 251,
  CONST_CHAR_PTR_GETTER = 252
};

DataType recognizeType(bool variable);
DataType recognizeType(char variable);
DataType recognizeType(char* variable);
DataType recognizeType(const char* variable);
DataType recognizeType(int8_t variable);
DataType recognizeType(uint8_t variable);
DataType recognizeType(int16_t variable);
DataType recognizeType(uint16_t variable);
DataType recognizeType(int32_t variable);
DataType recognizeType(uint32_t variable);
DataType recognizeType(float variable);
DataType recognizeType(double variable);
DataType recognizeType(boolFnPtr variable);
DataType recognizeType(int8tFnPtr variable);
DataType recognizeType(uint8tFnPtr variable);
DataType recognizeType(int16tFnPtr variable);
DataType recognizeType(uint16tFnPtr variable);
DataType recognizeType(int32tFnPtr variable);
DataType recognizeType(uint32tFnPtr variable);
DataType recognizeType(floatFnPtr variable);
DataType recognizeType(doubleFnPtr variable);
DataType recognizeType(charFnPtr variable);
DataType recognizeType(charPtrFnPtr variable);
DataType recognizeType(constcharPtrFnPtr variable);

class LCDScreen;
class LCDMenu;

class LCDLine {
public:
  
  // Main Constructor
  LCDLine(LCDScreen* parent, char* text = ""){
    _parent = parent;
    _text = new char[strlen(text)+1];
    strcpy(_text, text);
    _target = nullptr;
    addToParent();
    for (uint8_t i = 0; i < MAX_LINE_VARIABLES; i++) {
      _variables[i] = nullptr;
      _variableTypes[i] = DataType::NOT_USED;
    }
    _numVariables = 0;
    _clickFunction = nullptr;
    _floatDecimalPlaces = 1;
    _resetTarget = true;
  }

  // Constructor with 1 variable
  template <typename A>
  LCDLine(LCDScreen* parent, char* text = "", A& varA = nullptr): LCDLine(parent, text){
    addVariable(varA);
  }

  // Constructor with 2 variables
  template <typename A, typename B>
  LCDLine(LCDScreen* parent, char* text = "", A& varA = nullptr, B& varB = nullptr): LCDLine(parent, text, varA){
    addVariable(varB);
  }

  // Constructor with 3 variables
  template <typename A, typename B, typename C>
  LCDLine(LCDScreen* parent, char* text = "", A& varA = nullptr, B& varB = nullptr, C& varC = nullptr): LCDLine(parent, text, varA, varB){
    addVariable(varC);
  }

  // Constructor with 4 variables
  template <typename A, typename B, typename C, typename D>
  LCDLine(LCDScreen* parent, char* text = "", A& varA = nullptr, B& varB = nullptr, C& varC = nullptr, D& varD = nullptr): LCDLine(parent, text, varA, varB, varC){
    addVariable(varD);
  }

  // add variable to array
  template <typename T>
  bool addVariable(T &var){
    if (_numVariables >= MAX_LINE_VARIABLES) return false;
    _variables[_numVariables] = (void*)&var;
    _variableTypes[_numVariables] = recognizeType(var);
    _numVariables++;
    return true;
  }

  void setTarget(LCDScreen* target);
  void setClickFunction(std::function<void()> clickFunction);
  void setResetTarget(bool resetTarget);
  void setDecimalPlaces(uint8_t places);
  virtual bool isLocked();

protected:
  void addToParent();
  virtual bool onClick(LCDMenu* menu);
  virtual bool onUp();
  virtual bool onDown();
  virtual bool onHold();
  virtual void getOutput(hd44780_I2Cexp* lcd);
  void printVariableSerial(uint8_t index);
  void printVariableLCD(hd44780_I2Cexp* lcd, uint8_t index);

protected:
  LCDScreen* _parent;
  LCDScreen* _target;
  std::function<void()> _clickFunction;
  uint8_t _floatDecimalPlaces;
  bool _resetTarget;
  char* _text;

protected:
  friend class LCDScreen;

private:
  DataType _variableTypes[MAX_LINE_VARIABLES];
  const void* _variables[MAX_LINE_VARIABLES];
  uint8_t _numVariables;
};


class LCDEditLine: public LCDLine{
public:
  LCDEditLine(LCDScreen* parent,
              char* text = "",
              long value = 0,
              long min = 0,
              long max = 0,
              long step = 1,
              std::function<std::string(long)> translationFunction = nullptr,
              std::function<void()> applyFunction = nullptr,
              LCDScreen* target = nullptr);

  template <typename T>
  void addVariable(T &var){
    if (_numVariables == 0) LCDLine::addVariable(var);
  }

  bool isLocked() override;
  void setValue(long value);
  long getValue();
  void setEditing(bool editing);
  void setStep(long step);

protected:
  bool onClick(LCDMenu* menu) override;
  bool onUp() override;
  bool onDown() override;
  bool onHold() override;
  void getOutput(hd44780_I2Cexp* lcd) override;

protected:
  long _value;
  long _min;
  long _max;
  long _step;
  std::function<std::string(long)> _translationFunction;
  std::function<void()> _applyFunction;
  bool _editing;
};


class LCDChoiceLine: public LCDEditLine{
public:
  LCDChoiceLine(LCDScreen* parent,
              uint8_t numChoices,
              char** choices,
              char* text = "",
              std::function<void()> applyFunction = nullptr,
              uint8_t defaultChoice = 0,
              LCDScreen* target = nullptr);

  char* getChoice();

protected:
  void getOutput(hd44780_I2Cexp* lcd) override;

private:
  char* _choices[MAX_CHOICE_CHOICES];
};


class LCDScreen{
public:
  LCDScreen();
  void addLine(LCDLine* line);
  void hideCursor(bool hide = true);
  void leftCursor();
  void rightCursor();
  void reset();

protected:
  void up(uint8_t rows, bool wrap);
  void down(uint8_t rows, bool wrap);
  void adjustScroll(uint8_t rows);
  bool click(LCDMenu* menu);
  void hold();
  void print(hd44780_I2Cexp* lcd, uint8_t rows);

protected:
  uint8_t _cursor;
  uint8_t _scroll;
  LCDLine* _lines[MAX_SCREEN_LINES];
  uint8_t _numLines;
  uint8_t _cursorStyle;

protected:
  friend class LCDMenu;
};


class LCDSubScreen: public LCDScreen{
public:
  LCDSubScreen(LCDScreen* parent, char* name);
  void setOnEntryClickFunction(std::function<void()> clickFunction);

private:
  LCDScreen* _parent = nullptr;
  LCDLine* _towards = nullptr;
  LCDLine* _backwards = nullptr;
};


class LCDMenu{
public:
  LCDMenu(uint8_t cols, uint8_t rows, hd44780_I2Cexp* lcd);

  void click();
  void up();
  void down();
  void hold();
  void update();
  void setScreen(LCDScreen* screen, bool update = true);
  void setRoot(LCDScreen* screen);
  void setWrapping(bool wrap);

  LCDScreen* getScreen();

private:
  uint8_t _cols;
  uint8_t _rows;  
  hd44780_I2Cexp* _lcd;
  LCDScreen* _screen;
  LCDScreen* _root;
  bool _wrap;
};


#endif