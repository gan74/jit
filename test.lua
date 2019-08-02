
print(true or 6)
print(false or 6)
print(nil or 6)
print(0 or 6)
print("pwet" or 6)

print("------------------")

print(true and 6)
print(false and 6)
print(nil and 6)
print(0 and 6)
print("pwet" and 6)
print(3.1415)

print("------------------")

print(false and false)
print(true and false)
print(true and true)
print(false and true)


print("------------------")

print(false or false)
print(true or false)
print(true or true)
print(false or true)

print("------------------")

print(0)
print(1.2)
print(-1.2)
print(-0)
print(-7)
print(9.9999999999)


print("------------------")
local t = {}
table.insert(t, 4)
table.insert(t, "pwet")
print(#t)
t["foo"] = 16;
print(#t)
t[4646843435] = 0;
print(#t)
