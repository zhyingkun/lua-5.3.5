local redux = {}

local tableMT = { __index = table }

---@alias ListenerSignature fun():void
---@alias UnsubscribeSignature fun():void
---@alias DispatchSignature fun(action:table):table @return action
---@alias ReducerSignature fun(state:table, action:table):table @return newState
---@alias EnhancerSignature fun(createStore:CreateStoreSignature):CreateStoreSignature
---@alias CreateStoreSignature fun(reducer:ReducerSignature, preloadedState:table, enhancer:EnhancerSignature):ReduxStore

---@class ReduxStore:table
---@field public getState fun():table
---@field public subscribe fun(listener:ListenerSignature):UnsubscribeSignature
---@field public dispatch DispatchSignature
---@field public replaceReducer fun(reducer:ReducerSignature):void

---@overload fun(reducer:ReducerSignature):ReduxStore
---@overload fun(reducer:ReducerSignature, enhancer:EnhancerSignature):ReduxStore
---@overload CreateStoreSignature
---@param reducer ReducerSignature
---@param preloadedState table
---@param enhancer fun(createStore:CreateStoreSignature):CreateStoreSignature
---@return ReduxStore
local function createStore(reducer, preloadedState, enhancer)
    if type(preloadedState) == "function" and enhancer == nil then
        enhancer = preloadedState
        preloadedState = nil
    end
    if type(enhancer) == "function" then
        return enhancer(createStore)(reducer, preloadedState)
    end
    local currentReducer = reducer
    local currentState = preloadedState

    local currentListeners = setmetatable({}, tableMT)
    local addListeners = setmetatable({}, tableMT)
    local deleteListeners = {}

    local isDispatching = false
    local function getState()
        if isDispatching then error("Should not getState when Dispatching") end
        return currentState
    end
    local function subscribe(listener)
        if isDispatching then error("Should not subscribe when Dispatching") end
        addListeners:insert(listener)
        local isSubscribed = true
        return function()
            if isDispatching then error("Should not subscribe when Dispatching") end
            if not isSubscribed then return end
            isSubscribed = false

            local idx = addListeners:indexOf(listener)
            if idx then
                addListeners:remove(idx)
            else
                deleteListeners[listener] = true
            end
        end
    end
    local function dispatch(action)
        if isDispatching then error("Should not dispatch when Dispatching") end
        isDispatching = true
        _, currentState = pcall(currentReducer, currentState, action)
        isDispatching = false
        for listener in pairs(deleteListeners) do
            deleteListeners[listener] = nil
            local idx = currentListeners:indexOf(listener)
            assert(idx, "Delete listener must in currentListeners")
            currentListeners:remove(idx)
        end
        for idx, listener in ipairs(addListeners) do
            addListeners[idx] = nil
            currentListeners:insert(listener)
        end
        for _, listener in ipairs(currentListeners) do
            listener()
        end
        return action
    end
    local function replaceReducer(nextReducer)
        currentReducer = nextReducer
        dispatch()
    end

    dispatch()
    return {
        getState = getState,
        subscribe = subscribe,
        dispatch = dispatch,
        replaceReducer = replaceReducer,
    }
end
redux.createStore = createStore

---@param actionTypeMap table<string, ReducerSignature>
---@param initState table
---@return table
function redux.makeReducer(actionTypeMap, initState)
    return function(state, action)
        if not state then state = initState end
        local produce
        if action then produce = actionTypeMap[action.type] end
        if produce then
            return produce(state, action)
        else
            return state
        end
    end
end

---@param reducers table<integer, ReducerSignature>
---@return ReducerSignature
function redux.combineReducers(reducers)
    return function(state, action)
        state = state or {}
        for key, reducer in pairs(reducers) do
            state[key] = reducer(state[key], action)
        end
        return state
    end
end

---@alias ActionMakerSignature fun(...):table
---@alias ActionDispatcherSignature fun(...):table @return action

---@param actionCreator ActionMakerSignature
---@param dispatch DispatchSignature
---@return ActionDispatcherSignature
local function bindActionCreator(actionCreator, dispatch)
    return function(...)
        return dispatch(actionCreator(...))
    end
end
---@param actionCreators table<string, ActionMakerSignature>
---@param dispatch DispatchSignature
---@return table<string, ActionDispatcherSignature> @key is ActionDispatcher function name
function redux.bindActionCreators(actionCreators, dispatch)
    if type(actionCreators) == "function" then return bindActionCreator(actionCreators, dispatch) end
    local boundActionCreators = {}
    for key, actionCreator in pairs(actionCreators) do
        if type(actionCreator) == "function" then
            boundActionCreators[key] = bindActionCreator(actionCreator, dispatch)
        end
    end
    return boundActionCreators
end

--[[
local function reduce(list, callback, initialValue)
    local len = #list
    if len == 0 then return initialValue end
    local idx = 1
    if not initialValue then
        initialValue = list[1]
        idx = 2
    end
    local totalValue = initialValue
    for i = idx, len, 1 do
        totalValue = callback(totalValue, list[i], i, list)
    end
    return totalValue
end
--]]
local function compose(funcList)
    local len = #funcList
    if len == 0 then return function(arg) return arg end end
    if len == 1 then return funcList[1] end
    return funcList:reduce(function(a, b)
        return function(...)
            return a(b(...))
        end
    end)
end

---@alias DispatchEnhancerSignature fun(dispatch:DispatchSignature):DispatchSignature
---@alias MiddlewareSignature fun(store:ReduxStore):DispatchEnhancerSignature

---@param middlewares table<integer, MiddlewareSignature>
---return EnhancerSignature
function redux.applyMiddleware(middlewares)
    return function(oldCreateStore)
        return function(reducer, ...)
            local store = oldCreateStore(reducer, ...)
            local function dispatch() error("") end
            local middlewareAPI = {
                getState = store.getState,
                dispatch = function(action, ...) dispatch(action, ...) end,
            }
            setmetatable(middlewares, tableMT)
            local chain = middlewares:map(function(middleware) return middleware(middlewareAPI) end)
            store.dispatch = compose(chain)(store.dispatch)
            return store
        end
    end
end

return redux
