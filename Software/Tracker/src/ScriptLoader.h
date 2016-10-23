#pragma once

struct ScriptLoader {
  struct lua_State* lua = nullptr;

  ~ScriptLoader();
};

bool ScriptLoaderInit(ScriptLoader* loader);
const char* ScriptLoaderUpdate(ScriptLoader* loader, double dt,
                               struct World* world, struct ControlState* state);
bool ScriptLoaderExecFile(ScriptLoader* loader, const char* file);
bool ScriptLoaderExec(ScriptLoader* loader, const char* script);
const char* ScriptLoaderGetError(ScriptLoader* loader);
