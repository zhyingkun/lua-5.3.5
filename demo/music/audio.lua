local libuv = require("libuv")
local mbio = libuv.mbio

local mad = require("mad")

local mb = mbio.readFile("demo/music/WhereTheRoad.mp3")

print(mb)
local decodeMB, err = mad.madDecode(mb)

print(decodeMB, err)

local pa = require("portaudio")

local err = pa.Initialize()
print("Initialize:", err)

local index = pa.GetDefaultOutputDevice()
local deviceInfo = pa.GetDeviceInfo(index)

local streamParameters = pa.StreamParameters({
	device = index,
	channelCount = 2,
	sampleFormat = pa.sample_format.Int32,
	suggestedLatency = deviceInfo.defaultLowOutputLatency,
})
print(streamParameters)

local stream, err = pa.OpenStream(nil, streamParameters, 44100, 64, pa.stream_flag.ClipOff, decodeMB)

print(stream, err)

local err = pa.StartStream(stream)
print(err)

local sec = 120
print("Play for " .. tostring(sec) .. " seconds.")
pa.Sleep(sec * 1000);

local err = pa.StartStream(stream)
print(err)

local err = pa.StopStream(stream)
print(err)

local err = pa.CloseStream(stream)
print(err)

local err = pa.Terminate()
print(err)
