#ifndef LuaWrapper_h
#define LuaWrapper_h

#include <Arduino.h>
#include <SPI.h>
#include <sdios.h>
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"

class Lua {
public:
  typedef void (*Callback)();
  Callback handleErrorBegin;
  Callback handleErrorEnd;

  Stream *serial;
  lua_State *L;  

  ~Lua();
  void begin(Stream* serial);
  void reset();

  bool check(int error);
  bool execute(const char *string);
  bool executeFile(const char* fileName);

  void registerFunction(const char *name, const lua_CFunction function);
  void registerLibrary(const char *name, const luaL_reg library[]);

  bool getFunction(const char *name);
  bool getFunction(const char *table, const char *name);
  bool getFunction(const char *name, bool logError);
  bool getFunction(const char *table, const char *name, bool logError);

  void push(lua_Number number);
  void push(const char *string);
  bool call(byte argsCount, byte resultsCount);
  
  void onErrorBegin(Callback handler) { handleErrorBegin = handler; }
  void onErrorEnd(Callback handler) { handleErrorEnd = handler; }

private:
  void initState();
  int luaLoadFile(lua_State *L);
  void registerPolyfills();
  void errorBegin() { if (handleErrorBegin != NULL) handleErrorBegin(); }
  void errorEnd() { if (handleErrorEnd != NULL) handleErrorEnd(); }
};

#endif