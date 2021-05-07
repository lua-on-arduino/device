#include <Arduino.h>
#include <unity.h>
#include <LuaWrapper.h>

Lua lua(&Serial);

bool mockFunctionWasCalled = false;
int mockFunction(lua_State *L) {
  mockFunctionWasCalled = true;
  return 0;
}

void testLuaExecute() {
  lua.reset();
  TEST_ASSERT_EQUAL(true, lua.execute("testValue = 10"));
  lua_getglobal(lua.L, "testValue");
  TEST_ASSERT_EQUAL(10, lua_tonumber(lua.L, -1));
}

void testLuaRegisterFunction() {
  lua.reset();
  mockFunctionWasCalled = false;
  lua.registerFunction("testFunction", mockFunction);
  lua_getglobal(lua.L, "testFunction");
  TEST_ASSERT_FALSE(lua_pcall(lua.L, 0, 0, 0));
  TEST_ASSERT_TRUE(mockFunctionWasCalled);
}

void testLuaRegisterLibrary() {
  lua.reset();
  mockFunctionWasCalled = false;
  luaL_Reg library[] = { { "testFunction", mockFunction }, { NULL, NULL } };
  lua.registerLibrary("testLibrary", library);
  lua_getglobal(lua.L, "testLibrary");
  lua_getfield(lua.L, -1, "testFunction");
  TEST_ASSERT_FALSE(lua_pcall(lua.L, 0, 0, 0));
  TEST_ASSERT_TRUE(mockFunctionWasCalled);
}

void testLuaGetFunction() {
  lua.reset();
  lua.registerFunction("testFunction", mockFunction);
  TEST_ASSERT_TRUE(lua.getFunction("testFunction"));
  TEST_ASSERT_FALSE(lua.getFunction("doesNotExist"));

  lua.reset();
  luaL_Reg library[] = { { "testFunction", mockFunction }, { NULL, NULL } };
  lua.registerLibrary("testLibrary", library);
  TEST_ASSERT_TRUE(lua.getFunction("testLibrary", "testFunction"));
  TEST_ASSERT_FALSE(lua.getFunction("testLibrary", "doesNotExist"));
}

void testLuaPush() {
  lua.reset();

  int number = 10;
  lua.push(number);
  TEST_ASSERT_EQUAL(number, lua_tonumber(lua.L, -1));

  bool boolean = true;
  lua.push(boolean);
  TEST_ASSERT_TRUE(lua_toboolean(lua.L, -1));

  const char *text = "test";
  lua.push(text);
  TEST_ASSERT_EQUAL_STRING(text, lua_tostring(lua.L, -1));
}

void testLuaCall() {
  lua.reset();
  mockFunctionWasCalled = false;
  lua.registerFunction("testFunction", mockFunction);
  lua.getFunction("testFunction");
  TEST_ASSERT_TRUE(lua.call(0, 0));
  TEST_ASSERT_TRUE(mockFunctionWasCalled);
  lua.getFunction("doesNotExist");
  TEST_ASSERT_FALSE(lua.call(0, 0));
}

void setup() {
  while (!Serial) {}

  UNITY_BEGIN();

  RUN_TEST(testLuaExecute);
  RUN_TEST(testLuaRegisterFunction);
  RUN_TEST(testLuaRegisterLibrary);
  RUN_TEST(testLuaGetFunction);
  RUN_TEST(testLuaPush);
  RUN_TEST(testLuaCall);
  
  UNITY_END();
}

void loop() {}