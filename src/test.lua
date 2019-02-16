local minhook = require("minhook")
hooks = {}

local f = function(ct, ...) return ct, {...} end
local calltype, args = f(...)

minhook.initialize()
local callback = function(hook, ...)
	io.write(string.format("%d > ", #{...}))
	local whook = minhook.unpack(minhook.pack(hook))
	whook:setresult()
	local result = whook(...)
	whook:setresult(result)
	assert(whook:getresult() == result)
	return result
end

for i, v in ipairs(args) do
	local h = minhook.create(v, i - 1, calltype, callback)
	table.insert(hooks, h)
	h:hook()
end
