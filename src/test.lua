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

if test_kmdfind_func_addr then
	local rr = minhook.rawcreate(2, calltype, test_kmdfind_func_addr)
	assert(rr(1, 2) == 0)

	local tt = minhook.create(test_kmdfind_func_addr, 2, calltype, function(hook, addr, size)
		minhook.kmdfind(addr, size, "ababaaababaa", false)

		local patn = "8B C8 E8 . . . . E9 BE 00 00 00 6A 00 6A 00 6A FF 6A FF BA 04 00 00 00 6B C2 0E 8B"
		return minhook.kmdfind(addr, size, patn)
	end)
	table.insert(hooks, tt)
	tt:hook()
end