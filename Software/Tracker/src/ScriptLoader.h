#pragma once

struct ScriptLoader {
  struct lua_State* lua = nullptr;
  void* user = nullptr;
  void (*log)(const char*, void*) = nullptr;

  ~ScriptLoader();
};

bool ScriptLoaderInit(ScriptLoader* loader);
const char* ScriptLoaderUpdate(ScriptLoader* loader, double dt,
                               struct World* world, struct ControlState* state,
                               struct TrackingState* tracking);
bool ScriptLoaderExecFile(ScriptLoader* loader, const char* file);
bool ScriptLoaderExec(ScriptLoader* loader, const char* script);
void ScriptLoaderSetLogCallback(ScriptLoader* loader,
                                void (*cb)(const char*, void*), void* user);
const char* ScriptLoaderGetError(ScriptLoader* loader);
