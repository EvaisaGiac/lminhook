local minhook = require("minhook")
hooks = {}

local f = function(ct, ...) return ct, {...} end
local calltype, args = f(...)

minhook.initialize()
local callback = function(hook, ...)
	io.write(string.format("(lua hook, calltype[%s], paramCount[%d])", hook:calltype(), #{...}))
	return hook(...)
end

for i, v in ipairs(args) do
	local h = minhook.create(v, i - 1, calltype, callback)
	table.insert(hooks, h)
	h:hook()
end
