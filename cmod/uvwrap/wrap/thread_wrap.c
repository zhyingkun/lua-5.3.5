#define thread_wrap_c
#include <uvwrap.h>

#define THREAD_FUNCTION(name) UVWRAP_FUNCTION(thread, name)

static void* wrap_thread_create(void* entry, void* arg) {
  uv_thread_t* tid = (uv_thread_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_thread_t));
  int err = uv_thread_create(tid, (uv_thread_cb)entry, arg);
  if (err != 0) {
    (void)MEMORY_FUNCTION(free)((void*)tid);
    return NULL;
  }
  return (void*)tid;
}
static void* wrap_thread_self(void) {
  uv_thread_t* tid = (uv_thread_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_thread_t));
  *tid = uv_thread_self();
  return (void*)tid;
}
static void wrap_thread_invalid(void* tid) {
  (void)MEMORY_FUNCTION(free)((void*)tid);
}

static void* wrap_sem_init(int value) {
  uv_sem_t* sem = (uv_sem_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_sem_t));
  int err = uv_sem_init(sem, (unsigned int)value);
  if (err != 0) {
    (void)MEMORY_FUNCTION(free)((void*)sem);
    return NULL;
  }
  return (void*)sem;
}
static void wrap_sem_destroy(void* sem) {
  uv_sem_destroy((uv_sem_t*)sem);
  (void)MEMORY_FUNCTION(free)((void*)sem);
}

#define REGISTER_FUNC_WRAP(name) \
  REGISTER_LIGHTUSERDATA(name, wrap_##name)

#define REGISTER_FUNC_UV(name) \
  REGISTER_LIGHTUSERDATA(name, uv_##name)

static void registe_sem(lua_State* L) {
  lua_createtable(L, 0, 16);

  REGISTER_FUNC_WRAP(sem_init);
  REGISTER_FUNC_WRAP(sem_destroy);
  REGISTER_FUNC_UV(sem_post);
  REGISTER_FUNC_UV(sem_wait);
  REGISTER_FUNC_UV(sem_trywait);

  lua_setfield(L, -2, "sem");
}

void THREAD_FUNCTION(init)(lua_State* L) {
  luaL_checkversion(L);
  lua_createtable(L, 0, 16);

  REGISTER_FUNC_WRAP(thread_create);
  REGISTER_FUNC_WRAP(thread_invalid);
  REGISTER_FUNC_WRAP(thread_self);
  REGISTER_FUNC_UV(thread_join);
  REGISTER_FUNC_UV(thread_equal);

  registe_sem(L);

  lua_setfield(L, -2, "thread");
}
