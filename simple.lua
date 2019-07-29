

function fact (n) -- defines a factorial function
	if n == 0 then
		return 1
	else
		return n * fact(n-1)
	end
end

function fact2 (n) -- defines a factorial function
	local a = 1
	for i = 1,n do 
		a = a * i
	end
	return a
end


print("enter a number:")
a = io.read("*number")        -- read a number
print(fact(a))
print(fact2(a))
--[[
local a, b, c, d, e = 1, 2, 3, 4, 5

function test() 
	return 10, 11, 12, 13
end

a, b, c = test()
print(a, b, c, d, e)
-- print(test())
]]--