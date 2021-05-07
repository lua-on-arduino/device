#include <Arduino.h>
#include <LuaOnArduino.h>

Lua lua(&Serial);

int mockFunction(lua_State *L) {
  return 0;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  lua.begin();
  lua.onErrorBegin([]() {
    Serial.print("ERROR!: ");
  });

  luaL_Reg library[] = { { "testFunction", mockFunction }, { NULL, NULL } };
  lua.registerLibrary("testLibrary", library);

  lua.execute(
    "for k,v in pairs(testLibrary) do print(k, v) end"
  );

  if (!lua.getFunction("testFunction")) Serial.println("oh no!"); else Serial.print("yes!!");
}

void loop() {}