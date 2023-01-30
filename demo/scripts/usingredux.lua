local redux = require("redux")

--[[
local store = redux.createStore(redux.makeReducer({
    COUNT_INCREASE = function(state) state.count = state.count + 1 return state end,
    COUNT_DECREASE = function(state) state.count = state.count - 1 return state end,
    SET_NAME = function(state, action) state.name = action.name or state.name return state end,
}, {
    count = 1,
    name = "Jack",
}))

store.subscribe(function()
    print("---state change---")
    print(tostring(store.getState(), 1))
end)

store.dispatch({ type = "COUNT_INCREASE" })
store.dispatch({ type = "COUNT_DECREASE" })
store.dispatch({ type = "SET_NAME", name = "Like" })
--]]

---[[
local tradeReducer = redux.makeReducer({
    COUNT_INCREASE = function(state) state.count = state.count + 1 return state end,
    COUNT_DECREASE = function(state) state.count = state.count - 1 return state end,
}, {
    count = 1,
})

local userReduce = redux.makeReducer({
    SET_NAME = function(state, action) state.name = action.name or state.name return state end,
    SET_PHONENUMBER = function(state, action) state.phoneNumber = action.phoneNumber or state.phoneNumber return state end,
}, {
    name = "Jack",
    phoneNumber = "18888888888",
})

local store = redux.createStore(redux.combineReducers({
    trade = tradeReducer,
    user = userReduce,
}))

store.subscribe(function()
    print("---state change---")
    print(tostring(store.getState(), 2))
end)
--]]

--[[
store.dispatch({ type = "COUNT_INCREASE" })
store.dispatch({ type = "COUNT_DECREASE" })
store.dispatch({ type = "SET_NAME", name = "Like" })
store.dispatch({ type = "SET_PHONENUMBER", phoneNumber = "188 8888 8888" })
--]]

--[[
local actionCreators = redux.bindActionCreators({
    increment = function() return { type = "COUNT_INCREASE" } end,
    decrement = function() return { type = "COUNT_DECREASE" } end,
    setName = function(name) return { type = "SET_NAME", name = name } end,
    setPhoneNumber = function(phoneNumber) return { type = "SET_PHONENUMBER", phoneNumber = phoneNumber } end,
}, store.dispatch)

actionCreators.increment()
actionCreators.decrement()
actionCreators.setName("Like")
actionCreators.setPhoneNumber("188 8888 8888")
--]]

---[[
local function functionMiddleware(store)
    return function(dispatch) -- dispatch enhancer
        return function(action) -- new dispatch
            print("from functionMiddleware: ", type(action))
            if type(action) == "function" then
                return action(dispatch)
            else
                return dispatch(action)
            end
        end
    end
end
local function coroutineMiddleware(store)
    return function(dispatch) -- dispatch enhancer
        return function(action) -- new dispatch
            print("from coroutineMiddleware: ", type(action))
            if type(action) == "thread" then
                return coroutine.resume(action, dispatch)
            else
                return dispatch(action)
            end
        end
    end
end
local function loggerMiddleware(store)
    return function(dispatch) -- dispatch enhancer
        return function(action) -- new dispatch
            print("from loggerMiddleware: ", tostring(action, 1), tostring(store.getState(), 2))
            return dispatch(action)
        end
    end
end
local function dateMiddleware(store)
    return function(dispatch) -- dispatch enhancer
        return function(action) -- new dispatch
            print("from dateMiddleware")
            return dispatch(action)
        end
    end
end

local store = redux.createStore(redux.combineReducers({
    trade = tradeReducer,
    user = userReduce,
}), redux.applyMiddleware({
    functionMiddleware, 
    coroutineMiddleware,
    loggerMiddleware,
    dateMiddleware,
}))

store.subscribe(function()
    print("---state change---")
    print(tostring(store.getState(), 2))
end)

local actionCreators = redux.bindActionCreators({
    increment = function() return { type = "COUNT_INCREASE" } end,
    decrement = function() return { type = "COUNT_DECREASE" } end,
    setName = function(name) return { type = "SET_NAME", name = name } end,
    setPhoneNumber = function(phoneNumber) return { type = "SET_PHONENUMBER", phoneNumber = phoneNumber } end,

    setNameAsync = function(name)
        return function(dispatch)
            dispatch({ type = "SET_NAME", name = name })
            -- make a callback with dispatch, wait some event and then dispatch SET_NAME
        end
    end,
}, store.dispatch)

actionCreators.increment()
actionCreators.decrement()
actionCreators.setName("Like")
actionCreators.setPhoneNumber("188 8888 8888")

actionCreators.setNameAsync("AnotherLike")
--]]
