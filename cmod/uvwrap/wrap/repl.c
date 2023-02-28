#define repl_c
#include <uvwrap.h>

#ifndef _WIN32 /* { */

#include <readline/readline.h>
#include <readline/history.h>

#define RL_INIT()
#define READLINE(p) readline(p)
#define SAVELINE(line) add_history(line)
#define FREELINE(b) free((void*)b)
#define RESTTERM() (void)rl_reset_terminal(NULL)

#else /* }{ */

#define LUA_MAXINPUT 512
#define RL_INIT() char __buffer__[LUA_MAXINPUT]

#define READLINE(p) \
  (fputs(p, stdout), \
   fflush(stdout), /* show prompt */ \
   fgets(__buffer__, LUA_MAXINPUT, stdin)) /* get line */
#define SAVELINE(line) \
  { \
    (void)line; \
  }
#define FREELINE(b) \
  { \
    (void)b; \
  }
#define RESTTERM()

#endif /* } */

#if !defined(lua_assert)
#define lua_assert(x) ((void)0)
#endif

/*
** {======================================================
** Functions from lua.c
** =======================================================
*/

#define LUA_PROGNAME "lua"
#define LUA_PROMPT "> "
#define LUA_PROMPT2 ">> "

static const char* progname = LUA_PROGNAME;

/*
** Prints an error message, adding the program name in front of it
** (if present)
*/
static void l_message(const char* pname, const char* msg) {
  if (pname)
    lua_printf("%s: ", pname);
  lua_printf("%s\n", msg);
}

/*
** Message handler used to run all chunks
*/
static int msghandler(lua_State* L) {
  const char* msg = lua_tostring(L, 1);
  if (msg == NULL) { /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") && /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING) /* that produces a string? */
      return 1; /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1); /* append a standard traceback */
  return 1; /* return the traceback */
}

/*
** Returns the string to be used as a prompt by the interpreter.
*/
// [-0, +1, -]
static const char* get_prompt(lua_State* L, int firstline) {
  const char* p;
  lua_getglobal(L, firstline ? "_PROMPT" : "_PROMPT2");
  p = lua_tostring(L, -1);
  if (p == NULL) {
    lua_pop(L, 1);
    p = (firstline ? LUA_PROMPT : LUA_PROMPT2);
    lua_pushstring(L, p);
  }
  return p;
}

/* mark in error messages for incomplete statements */
#define EOFMARK "<eof>"
#define marklen (sizeof(EOFMARK) / sizeof(char) - 1)

/*
** Check whether 'status' signals a syntax error and the error
** message at the top of the stack ends with the above mark for
** incomplete statements.
*/
// [-(0|1), +0, -], need 0 slot
static int incomplete(lua_State* L, int status) {
  if (status == LUA_ERRSYNTAX) {
    size_t lmsg;
    const char* msg = lua_tolstring(L, -1, &lmsg);
    if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
      lua_pop(L, 1);
      return 1;
    }
  }
  return 0; /* else... */
}

/*
** Prints (calling the Lua 'print' function) any values on the stack
*/
// [-n, +0, -]
static void l_print(lua_State* L) {
  int n = lua_gettop(L);
  if (n > 0) { /* any result to be printed? */
    luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
    lua_getglobal(L, "print");
    lua_insert(L, 1);
    if (lua_pcall(L, n, 0, 0) != LUA_OK) {
      l_message(progname, lua_pushfstring(L, "error calling 'print' (%s)", lua_tostring(L, -1)));
      lua_settop(L, 0);
    }
  }
}

// [-0, +0, -]
static void call_registry_funcs(lua_State* L, const char* name, const char* msg) {
  if (lua_getfield(L, LUA_REGISTRYINDEX, name) == LUA_TTABLE) {
    int idx = lua_gettop(L);
    lua_pushcfunction(L, msghandler);
    lua_pushnil(L);
    while (lua_next(L, idx)) {
      if (lua_pcall(L, 0, 0, idx + 1) != LUA_OK) {
        lua_printf(msg, lua_tostring(L, -1));
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);
  }
  lua_pop(L, 1);
}

/* }====================================================== */

static int docall(lua_State* L, int narg, int nres) {
  int base = lua_gettop(L) - narg; /* function index */
  lua_pushcfunction(L, msghandler); /* push message handler */
  lua_insert(L, base); /* put it under function and args */
  int status = lua_pcall(L, narg, nres, base);
  lua_remove(L, base); /* remove message handler from the stack */
  return status;
}

#define REPL_OBJECT "_REPL_OBJECT_"
#define REPL_PROMPT "_REPL_PROMPT_"
#define REPL_HISTORY "_REPL_HISTORY_"
#define REPL_ONEVAL "_REPL_ONEVAL_"

#define GET_REGISTRY_FIELD(field_) lua_getfield(L, LUA_REGISTRYINDEX, field_)
#define SET_REGISTRY_FIELD(field_) lua_setfield(L, LUA_REGISTRYINDEX, field_)
#define CLEAR_REGISTRY_FIELD(field_) \
  lua_pushnil(L); \
  lua_setfield(L, LUA_REGISTRYINDEX, field_)

// [-0, +0, -]
static const char* get_prompt_hold(lua_State* L, int firstline) {
  const char* prmt = get_prompt(L, firstline);
  SET_REGISTRY_FIELD(REPL_PROMPT);
  return prmt;
}

static void save_history(const char* line) {
  if (line != NULL && line[0] != '\0') { /* non empty? */
    SAVELINE(line); /* keep history */
  }
}

/*
** {======================================================
** REPL Feature
** =======================================================
*/

typedef bool (*lua_EvalFunc)(void* ud, const char* code, bool bEOF, const char** pPrompt, const char** pHistory);
typedef struct {
  lua_EvalFunc func;
  void* ud;
} lua_Eval;

typedef void (*lua_EvalOneShotFunc)(void* ud, const char* code, bool bEOF);
typedef struct {
  lua_EvalOneShotFunc func;
  void* ud;
} lua_EvalOneShot;

typedef void (*lua_ShutdownFunc)(void* ud);
typedef struct {
  lua_ShutdownFunc func;
  void* ud;
} lua_Shutdown;

#define SET_NULL(struct_) \
  (struct_)->func = NULL; \
  (struct_)->ud = NULL

typedef struct {
  /* Context */
  bool bRunning;
  bool bOneShot;
  union {
    lua_Eval eval;
    lua_EvalOneShot shot;
  } cb;
  lua_Shutdown shutdown;
  uv_thread_t tid[1];
  /* Data Transmission */
  uv_async_t async[1];
  uv_sem_t sem[1];
  /* Read */
  const char* code; // Set by readline thread, clear by itself, used by eval callback
  bool bEOF;
  /* Print */
  const char* prompt; // Set by init or eval
  const char* history; // Set by eval
} lua_REPL;

void repl_initStart(lua_REPL* repl, uv_loop_t* loop, lua_Shutdown shut, lua_Eval eval, const char* firstPrompt);

void repl_initOneShot(lua_REPL* repl, uv_loop_t* loop, lua_Shutdown shut);
void repl_next(lua_REPL* repl, const char* prompt, const char* history, lua_EvalOneShot shot);
void repl_shutdown(lua_REPL* repl);

bool repl_isOneShot(lua_REPL* repl);

bool repl_defaultEval(void* ud, const char* code, bool bEOF, const char** pPrompt, const char** pHistory);

#define MSG_START "Multi thread REPL starting...\n"
#define MSG_STOP "Multi thread REPL end.\n"
static void thread_readLine(void* arg) {
  lua_REPL* repl = (lua_REPL*)arg;
  RL_INIT();
  lua_flushstring(MSG_START, sizeof(MSG_START));
  if (repl->bOneShot) {
    uv_sem_wait(repl->sem); // wait for next shot
  }
  while (repl->bRunning) { // for Ctrl-D in Unix or Ctrl-Z in Windows
    const char* buffer = READLINE(repl->prompt);

    // Read Send code and Wait
    repl->code = buffer;
    repl->bEOF = buffer == NULL;
    uv_async_send(repl->async);
    uv_sem_wait(repl->sem);

    repl->code = NULL;
    FREELINE(buffer);
    save_history(repl->history);
  }
  lua_flushstring(MSG_STOP, sizeof(MSG_STOP));
  RESTTERM();
}
static void repl_nextInternal(lua_REPL* repl, const char* prompt, const char* history) {
  lua_assert(repl->bRunning);
  repl->prompt = prompt;
  repl->history = history;
  uv_sem_post(repl->sem); // Fire next readline
}
static void async_doREPL(uv_async_t* handle) {
  lua_REPL* repl = uv_handle_get_data((const uv_handle_t*)handle);
  if (repl->bOneShot) {
    lua_EvalOneShot* shot = &repl->cb.shot;
    lua_assert(shot->func != NULL);
    shot->func(shot->ud, repl->code, repl->bEOF);
    SET_NULL(shot);
  } else {
    lua_Eval* eval = &repl->cb.eval;
    lua_assert(eval->func != NULL);
    const char *prompt, *history;
    if (eval->func(eval->ud, repl->code, repl->bEOF, &prompt, &history)) {
      repl_nextInternal(repl, prompt, history);
    } else {
      repl_shutdown(repl);
    }
  }
}
static void repl_initInternal(lua_REPL* repl, uv_loop_t* loop, lua_Shutdown shut, bool bOneShot, lua_Eval eval, const char* firstPrompt) {
  memset(repl, 0, sizeof(lua_REPL));
  /* Data Transmission */
  uv_async_init(loop, repl->async, async_doREPL);
  uv_handle_set_data((uv_handle_t*)repl->async, (void*)repl);
  uv_sem_init(repl->sem, 0);
  /* Read */
  repl->code = NULL;
  repl->bEOF = false;
  /* Print */
  repl->prompt = firstPrompt;
  repl->history = NULL;
  /* Context */
  repl->bRunning = true;
  repl->bOneShot = bOneShot;
  repl->cb.eval = eval; // one shot do not use this callback
  repl->shutdown = shut;
  signal(SIGINT, SIG_DFL); /* reset C-signal handler */
  uv_thread_create(repl->tid, thread_readLine, (void*)repl);
}

void repl_initStart(lua_REPL* repl, uv_loop_t* loop, lua_Shutdown shut, lua_Eval eval, const char* firstPrompt) {
  repl_initInternal(repl, loop, shut, false, eval, firstPrompt);
}

void repl_initOneShot(lua_REPL* repl, uv_loop_t* loop, lua_Shutdown shut) {
  static lua_Eval nullEval = {NULL, NULL};
  repl_initInternal(repl, loop, shut, true, nullEval, NULL);
}
void repl_next(lua_REPL* repl, const char* prompt, const char* history, lua_EvalOneShot shot) {
  lua_assert(repl->bRunning && repl->bOneShot);
  lua_assert(repl->cb.shot.func == NULL);
  repl->cb.shot = shot;
  repl_nextInternal(repl, prompt, history);
}
static void async_onClose(uv_handle_t* handle) {
  lua_REPL* repl = uv_handle_get_data(handle);
  lua_Shutdown* shut = &repl->shutdown;
  if (shut->func) {
    shut->func(shut->ud);
  }
  memset(repl, 0, sizeof(lua_REPL));
}
void repl_shutdown(lua_REPL* repl) {
  if (repl->bRunning) {
    repl->bRunning = false;
    uv_sem_post(repl->sem); // Maybe thread block in sem, so post it, maybe block in readline?
    uv_thread_join(repl->tid);
    uv_sem_destroy(repl->sem);
    uv_close((uv_handle_t*)repl->async, async_onClose);
  }
}

bool repl_isOneShot(lua_REPL* repl) {
  return repl->bOneShot;
}

/* }====================================================== */

/*
** {======================================================
** REPL Default Eval
** =======================================================
*/

// [-(0|1), +0, -], need 0 slot
static int report_with_print(lua_State* L, int status) {
  if (status != LUA_OK) {
    lua_getglobal(L, "print");
    if (progname) {
      const char* msg = lua_tostring(L, -2);
      lua_pushfstring(L, "%s: %s", progname, msg);
    } else {
      // lua_pushfstring(L, "%s", msg);
      lua_pushvalue(L, -2);
    }
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
      lua_pop(L, 1); // pop the error message
    }
    lua_pop(L, 1); /* remove message */
  }
  return status;
}

static bool firstline = true;
// [-0, +2, -]
int compile_source_code(lua_State* L, const char* code, const char** phistory) {
  char* b = (char*)code;
  size_t l = strlen(b);
  if (l > 0 && b[l - 1] == '\n') { /* line ends with newline? */
    b[--l] = '\0'; /* remove it */
  }
  bool hasClosure = false;
  if (firstline) {
    if (b[0] == '=') { /* for compatibility with 5.2, ... */
      lua_pushfstring(L, "return %s", b + 1); /* change '=' to 'return' */
    } else {
      lua_pushlstring(L, b, l);
      const char* retline = lua_pushfstring(L, "return %s;", b);
      if (luaL_loadbuffer(L, retline, strlen(retline), "=stdin") == LUA_OK) {
        lua_remove(L, -2); /* remove modified line */
        lua_insert(L, -2); // move the history string to top
        hasClosure = true;
      } else {
        lua_pop(L, 2); /* pop result from 'luaL_loadbuffer' and modified line */
      }
    }
  } else {
    GET_REGISTRY_FIELD(REPL_HISTORY);
    lua_pushliteral(L, "\n"); /* add newline... */
    lua_pushlstring(L, b, l);
    lua_concat(L, 3); /* join them */
  }
  size_t len = 0;
  const char* line = lua_tolstring(L, -1, &len);
  if (phistory) {
    *phistory = line;
  }
  SET_REGISTRY_FIELD(REPL_HISTORY);
  if (hasClosure) {
    return LUA_OK;
  }
  return luaL_loadbuffer(L, line, len, "=stdin"); /* try it */
}

bool repl_defaultEval(void* ud, const char* code, bool bEOF, const char** pPrompt, const char** pHistory) {
  (void)ud;
  lua_State* L = GET_MAIN_LUA_STATE();
  bool running = true;
  *pHistory = NULL;
  if (bEOF) { // Ctrl-D or Ctrl-Z+Enter
    if (firstline) {
      running = false;
    } else {
      firstline = true; // end multi line
      GET_REGISTRY_FIELD(REPL_HISTORY);
      *pHistory = lua_tostring(L, -1);
      lua_pop(L, 1);
    }
    lua_getglobal(L, "print");
    if (running) {
      lua_getglobal(L, "_PROMPT2");
      lua_pushfstring(L, "%s", lua_tostring(L, -1));
      lua_remove(L, -2);
    }
    if (lua_pcall(L, running ? 1 : 0, 0, 0) != LUA_OK) {
      lua_printf_err("%s\n", lua_tostring(L, -1));
      lua_pop(L, 1); // pop the error message
    }
  } else {
    const char* history;
    int status = compile_source_code(L, code, &history);
    if (incomplete(L, status)) {
      // incomplete will pop the error message if return true
      firstline = false;
    } else {
      firstline = true;
      *pHistory = history;
      if (status == LUA_OK) {
        if (lua_gettop(L) != 1) {
          lua_replace(L, 1);
          lua_settop(L, 1); // Only left the closure
        }
        status = docall(L, 0, LUA_MULTRET);
        if (status == LUA_OK) {
          l_print(L); // print direct
        }
      }
      report_with_print(L, status); // will pop the error message
      call_registry_funcs(L, LUA_ATREPL, "Call atrepl failed: %s\n");
    }
  }
  *pPrompt = get_prompt_hold(L, firstline);
  return running;
}

/* }====================================================== */

/*
** {======================================================
** REPL Wrap
** =======================================================
*/

#define REPL_TYPE "lua_REPL*"
static bool uvwrap_onEval(void* ud, const char* code, bool bEOF, const char** pPrompt, const char** pHistory) {
  lua_assert(!repl_isOneShot((lua_REPL*)ud));
  bool bRunning = false;
  lua_State* L = GET_MAIN_LUA_STATE();
  PREPARE_CALL_LUA(L);
  GET_REGISTRY_FIELD(REPL_ONEVAL);
  lua_pushstring(L, code);
  lua_pushboolean(L, bEOF);
  CALL_LUA(L, 2, 3);
  *pHistory = lua_tostring(L, -1);
  if (*pHistory != NULL) { // for using REPL_HISTORY to cache multiline in default implementationÏ
    SET_REGISTRY_FIELD(REPL_HISTORY);
  } else {
    lua_pop(L, 1);
  }
  *pPrompt = lua_tostring(L, -1);
  SET_REGISTRY_FIELD(REPL_PROMPT);
  bRunning = lua_toboolean(L, -1);
  lua_pop(L, 1);
  POST_CALL_LUA(L);
  return bRunning;
}
static void uvwrap_onEvalOneShot(void* ud, const char* code, bool bEOF) {
  lua_assert(repl_isOneShot((lua_REPL*)ud));
  lua_State* L = GET_MAIN_LUA_STATE();
  PREPARE_CALL_LUA(L);
  GET_REGISTRY_FIELD(REPL_ONEVAL);
  lua_pushstring(L, code);
  lua_pushboolean(L, bEOF);
  CALL_LUA_FUNCTION(L, 2);
}
static void uvwrap_onClose(void* ud) {
  (void)ud;
  lua_State* L = GET_MAIN_LUA_STATE();
  CLEAR_REGISTRY_FIELD(REPL_OBJECT);
  CLEAR_REGISTRY_FIELD(REPL_PROMPT);
  CLEAR_REGISTRY_FIELD(REPL_HISTORY);
  CLEAR_REGISTRY_FIELD(REPL_ONEVAL);
}
static lua_Shutdown shut = {uvwrap_onClose, NULL};
static lua_REPL* getREPLObject(lua_State* L) {
  GET_REGISTRY_FIELD(REPL_OBJECT);
  lua_REPL* repl = (lua_REPL*)luaL_checkudata_recursive(L, -1, REPL_TYPE);
  lua_pop(L, 1);
  return repl;
}
static lua_REPL* addREPLObject(lua_State* L) {
  GET_REGISTRY_FIELD(REPL_OBJECT);
  if (luaL_testudata_recursive(L, -1, REPL_TYPE)) {
    (void)luaL_error(L, "REPL is already running");
    return NULL;
  }
  lua_pop(L, 1);
  return (lua_REPL*)lua_newuserdata(L, sizeof(lua_REPL));
}
static lua_Eval holdEvalCallback(lua_State* L, int idx, lua_REPL* repl) {
  lua_Eval eval;
  if (lua_isnoneornil(L, idx)) {
    CLEAR_REGISTRY_FIELD(REPL_ONEVAL);
    firstline = true; // Used for firstline check
    eval.func = repl_defaultEval;
    eval.ud = NULL;
  } else {
    lua_pushvalue(L, idx);
    SET_REGISTRY_FIELD(REPL_ONEVAL);
    eval.func = uvwrap_onEval;
    eval.ud = (void*)repl;
  }
  return eval;
}
static const char* holdFirstPrompt(lua_State* L, int idx) {
  if (lua_isnoneornil(L, idx)) {
    return get_prompt_hold(L, true);
  } else {
    lua_pushvalue(L, idx);
    SET_REGISTRY_FIELD(REPL_PROMPT);
    return luaL_checkstring(L, idx);
  }
}
int uvwrap_replStart(lua_State* L) {
  lua_REPL* repl = addREPLObject(L);

  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const lua_Eval eval = holdEvalCallback(L, 2, repl);
  const char* prompt = holdFirstPrompt(L, 3);

  repl_initStart(repl, loop, shut, eval, prompt);

  luaL_setmetatable(L, REPL_TYPE);
  SET_REGISTRY_FIELD(REPL_OBJECT);
  return 0;
}
int uvwrap_replInitOneShot(lua_State* L) {
  lua_REPL* repl = addREPLObject(L);

  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  repl_initOneShot(repl, loop, shut);

  luaL_setmetatable(L, REPL_TYPE);
  SET_REGISTRY_FIELD(REPL_OBJECT);
  return 0;
}
int uvwrap_replNext(lua_State* L) {
  lua_REPL* repl = getREPLObject(L);

  const char* pPrompt = luaL_optstring(L, 1, LUA_PROMPT);
  const char* pHistory = luaL_optstring(L, 2, NULL);
  luaL_checkany(L, 3);
  lua_pushvalue(L, 3);
  SET_REGISTRY_FIELD(REPL_ONEVAL);
  lua_EvalOneShot shot = {uvwrap_onEvalOneShot, repl};

  repl_next(repl, pPrompt, pHistory, shot);
  return 0;
}
int uvwrap_replShutdown(lua_State* L) {
  lua_REPL* repl = getREPLObject(L);
  repl_shutdown(repl);
  return 0;
}
int uvwrap_replIsOneShot(lua_State* L) {
  lua_REPL* repl = getREPLObject(L);
  bool ret = repl_isOneShot(repl);
  lua_pushboolean(L, ret);
  return 1;
}
int uvwrap_replDefaultEval(lua_State* L) {
  const char* code = luaL_optstring(L, 1, NULL);
  bool bEOF = luaL_checkboolean(L, 2);
  const char* pPrompt;
  const char* pHistory;
  bool running = repl_defaultEval(NULL, code, bEOF, &pPrompt, &pHistory); // maybe error with firstline
  lua_pushboolean(L, running);
  lua_pushstring(L, pPrompt);
  lua_pushstring(L, pHistory);
  return 3;
}

int uvwrap_replRead(lua_State* L) {
  const char* prompt = luaL_checkstring(L, 1);

  RL_INIT();
  char* buffer = READLINE(prompt);
  lua_pushstring(L, buffer);
  FREELINE(buffer);
  return 1;
}
int uvwrap_replHistory(lua_State* L) {
  const char* history = lua_tostring(L, 1);
  save_history(history);
  return 0;
}

static int uvwrap_repl__gc(lua_State* L) {
  lua_REPL* repl = (lua_REPL*)luaL_checkudata_recursive(L, 1, REPL_TYPE);
  repl_shutdown(repl);
  return 0;
}
static const luaL_Reg uvwrap_replMetaFuncs[] = {
    {"__gc", uvwrap_repl__gc},
    {NULL, NULL},
};
void uvwrap_replInitMetatable(lua_State* L) {
  luaL_newmetatable(L, REPL_TYPE);
  luaL_setfuncs(L, uvwrap_replMetaFuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

/* }====================================================== */
