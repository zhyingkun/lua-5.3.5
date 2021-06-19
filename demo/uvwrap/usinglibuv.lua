local fs = require("libuv.fs")

local filename = "./word.txt"
print("Start...")
fs.open(filename, "r", 0, function(fd)
	print("In callback of fs.open:", fd)
	local function read_cb(result, str)
		print("In callback of fs.read:", result)
		if str ~= nil then
			print("Read from file:", str)
			fs.read(fd, -1, read_cb)
		else
			fs.close(fd, function()
				print("In callback of fs.close")
			end)
		end
	end
	fs.read(fd, -1, read_cb)
end)
print("End")
