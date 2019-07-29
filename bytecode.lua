function decompose(number)
	local factors = {}
	
	for possible_factor = 1,number,1 do
		local remainder = number % possible_factor
		if remainder == 0 then
			-- print(possible_factor)
			table.insert(factors, possible_factor)
		end
	end
	
	-- table.sort(factors)
	return factors
end

factors = decompose(42)

--[[ for i = 1,10 do 
	print(factors[i])
end ]]

for k, v in ipairs(factors) do 
	print(k, v)
end

