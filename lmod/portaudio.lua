local libpa = require("libportaudio")

---@class portaudio
local pa = {}

---@class PaDeviceIndex:integer

---@class PaTime:number

---@class PaStreamParameters:userdata

---@class PaStreamParametersTable:table
---@field public device PaDeviceIndex
---@field public channelCount integer
---@field public sampleFormat portaudio_sample_format
---@field public suggestedLatency PaTime

---@overload fun(void):PaStreamParameters
---@overload fun(value:PaStreamParametersTable):PaStreamParameters
---@param value PaStreamParametersTable
---@return PaStreamParameters
function pa.StreamParameters(value)
	return libpa.StreamParameters(value)
end

---@class PaStreamCallbackTimeInfo:userdata

---@class PaStreamCallbackTimeInfoTable:table
---@field public inputBufferAdcTime PaTime
---@field public currentTime PaTime
---@field public outputBufferDacTime PaTime

---@overload fun(void):PaStreamCallbackTimeInfo
---@overload fun(value:PaStreamCallbackTimeInfoTable):PaStreamCallbackTimeInfo
---@param value PaStreamCallbackTimeInfoTable
---@return PaStreamCallbackTimeInfo
function pa.StreamCallbackTimeInfo(value)
	return libpa.StreamCallbackTimeInfo(value)
end

---@class PaVersionInfoTable:table
---@field public versionMajor integer
---@field public versionMinor integer
---@field public versionSubMinor integer
---@field public versionControlRevision string
---@field public versionText string

---@return PaVersionInfoTable
function pa.GetVersionInfo()
	return libpa.GetVersionInfo()
end

---@class PaError:integer

---@param err PaError
---@return string
function pa.GetErrorText(err)
	return libpa.GetErrorText(err)
end

---@return PaError
function pa.Initialize()
	return libpa.Initialize()
end

---@return PaError
function pa.Terminate()
	return libpa.Terminate()
end

---@return integer
function pa.GetHostApiCount()
	return libpa.GetHostApiCount()
end

---@class PaHostApiIndex:integer

---@return PaHostApiIndex
function pa.GetDefaultHostApi()
	return libpa.GetDefaultHostApi()
end

---@class PaHostApiTypeId:integer

---@class PaDeviceIndex:integer

---@class PaHostApiInfoTable:table
---@field public structVersion integer
---@field public type PaHostApiTypeId
---@field public name string
---@field public deviceCount integer
---@field public defaultInputDevice PaDeviceIndex
---@field public defaultOutputDevice PaDeviceIndex

---@param hostApi PaHostApiIndex
---@return PaHostApiInfoTable
function pa.GetHostApiInfo(hostApi)
	return libpa.GetHostApiInfo(hostApi)
end

---@param index PaHostApiTypeId
---@return PaHostApiIndex
function pa.HostApiTypeIdToHostApiIndex(typeId)
	return libpa.HostApiTypeIdToHostApiIndex(typeId)
end

---@param hostApi PaHostApiIndex
---@param idx integer
---@return PaDeviceIndex
function pa.HostApiDeviceIndexToDeviceIndex(hostApi, idx)
	return libpa.HostApiDeviceIndexToDeviceIndex(hostApi, idx)
end

---@class PaHostErrorInfoTable:table
---@field public hostApiType PaHostApiTypeId
---@field public errorCode integer
---@field public errorText string

---@return PaHostErrorInfoTable
function pa.GetLastHostErrorInfo()
	return libpa.GetLastHostErrorInfo()
end

---@return integer
function pa.GetDeviceCount()
	return libpa.GetDeviceCount()
end

---@return PaDeviceIndex
function pa.GetDefaultInputDevice()
	return libpa.GetDefaultInputDevice()
end

---@return PaDeviceIndex
function pa.GetDefaultOutputDevice()
	return libpa.GetDefaultOutputDevice()
end

---@class PaDeviceInfoTable:table
---@field public structVersion integer
---@field public name string
---@field public hostApi PaHostApiIndex
---@field public maxInputChannels integer
---@field public maxOutputChannels integer
---@field public defaultLowInputLatency PaTime
---@field public defaultLowOutputLatency PaTime
---@field public defaultHighInputLatency PaTime
---@field public defaultHighOutputLatency PaTime
---@field public defaultSampleRate number

---@param index PaDeviceIndex
---@return PaDeviceInfoTable
function pa.GetDeviceInfo(index)
	return libpa.GetDeviceInfo(index)
end

---@param inputParam PaStreamParameters
---@param outputParam PaStreamParameters
---@param sampleRate number
---@return PaError
function pa.IsFormatSupported(inputParam, outputParam, sampleRate)
	return libpa.IsFormatSupported(inputParam, outputParam, sampleRate)
end

---@class PaStream:lightuserdata

---@param inputParam PaStreamParameters
---@param outputParam PaStreamParameters
---@param sampleRate number
---@param framesPerBuffer integer
---@param streamFlags portaudio_stream_flag
---@param mb luaL_MemBuffer
---@param finishUD number
---@return PaStream, PaError
function pa.OpenStream(inputParam, outputParam, sampleRate, framesPerBuffer, streamFlags, mb, finishUD)
	return libpa.OpenStream(inputParam, outputParam, sampleRate, framesPerBuffer, streamFlags, mb, finishUD)
end

---@param inputChannelCount integer
---@param outputChannelCount integer
---@param sampleFormat portaudio_sample_format
---@param sampleRate double
---@param framesPerBuffer integer
---@return PaStream, PaError
function pa.OpenDefaultStream(inputChannelCount, outputChannelCount, sampleFormat, sampleRate, framesPerBuffer)
	return libpa.OpenDefaultStream(inputChannelCount, outputChannelCount, sampleFormat, sampleRate, framesPerBuffer)
end

---@param stream PaStream
---@return PaError
function pa.CloseStream(stream)
	return libpa.CloseStream(stream)
end

---@class AsyncEmitter:lightuserdata

---@param emitter AsyncEmitter
function pa.SetAsyncEmitter(emitter)
	return libpa.SetAsyncEmitter(emitter)
end

---@param stream PaStream
---@return PaError
function pa.SetStreamFinishedCallback(stream)
	return libpa.SetStreamFinishedCallback(stream)
end

---@param stream PaStream
---@return PaError
function pa.StartStream(stream)
	return libpa.StartStream(stream)
end

---@param stream PaStream
---@return PaError
function pa.StopStream(stream)
	return libpa.StopStream(stream)
end

---@param stream PaStream
---@return PaError
function pa.AbortStream(stream)
	return libpa.AbortStream(stream)
end

---@param stream PaStream
---@return PaError
function pa.IsStreamStopped(stream)
	return libpa.IsStreamStopped(stream)
end

---@param stream PaStream
---@return PaError
function pa.IsStreamActive(stream)
	return libpa.IsStreamActive(stream)
end

---@class PaStreamInfoTable:table
---@field public structVersion integer
---@field public inputLatency PaTime
---@field public outputLatency PaTime
---@field public sampleRate number

---@param stream PaStream
---@return PaStreamInfoTable
function pa.GetStreamInfo(stream)
	return libpa.GetStreamInfo(stream)
end

---@param stream PaStream
---@return PaTime
function pa.GetStreamTime(stream)
	return libpa.GetStreamTime(stream)
end

---@param stream PaStream
---@return number
function pa.GetStreamCpuLoad(stream)
	return libpa.GetStreamCpuLoad(stream)
end

---@param stream PaStream
---@return integer
function pa.GetStreamReadAvailable(stream)
	return libpa.GetStreamReadAvailable(stream)
end

---@param stream PaStream
---@return integer
function pa.GetStreamWriteAvailable(stream)
	return libpa.GetStreamWriteAvailable(stream)
end

---@param format portaudio_sample_format
---@return PaError | integer
function pa.GetSampleSize(format)
	return libpa.GetSampleSize(format)
end

---@param msec integer
function pa.Sleep(msec)
	return libpa.Sleep(msec)
end

---@class portaudio_error_code
---@field public NoError integer
---@field public NotInitialized integer
---@field public UnanticipatedHostError integer
---@field public InvalidChannelCount integer
---@field public InvalidSampleRate integer
---@field public InvalidDevice integer
---@field public InvalidFlag integer
---@field public SampleFormatNotSupported integer
---@field public BadIODeviceCombination integer
---@field public InsufficientMemory integer
---@field public BufferTooBig integer
---@field public BufferTooSmall integer
---@field public NullCallback integer
---@field public BadStreamPtr integer
---@field public TimedOut integer
---@field public InternalError integer
---@field public DeviceUnavailable integer
---@field public IncompatibleHostApiSpecificStreamInfo integer
---@field public StreamIsStopped integer
---@field public StreamIsNotStopped integer
---@field public InputOverflowed integer
---@field public OutputUnderflowed integer
---@field public HostApiNotFound integer
---@field public InvalidHostApi integer
---@field public CanNotReadFromACallbackStream integer
---@field public CanNotWriteToACallbackStream integer
---@field public CanNotReadFromAnOutputOnlyStream integer
---@field public CanNotWriteToAnInputOnlyStream integer
---@field public IncompatibleStreamHostApi integer
---@field public BadBufferPtr integer

---@type portaudio_error_code
pa.error_code = libpa.error_code

---@class portaudio_device_index
---@field public NoDevice integer
---@field public UseHostApiSpecificDeviceSpecification integer

---@type portaudio_device_index
pa.device_index = libpa.device_index

---@class portaudio_host_api_type_id
---@field public InDevelopment integer
---@field public DirectSound integer
---@field public MME integer
---@field public ASIO integer
---@field public SoundManager integer
---@field public CoreAudio integer
---@field public OSS integer
---@field public ALSA integer
---@field public AL integer
---@field public BeOS integer
---@field public WDMKS integer
---@field public JACK integer
---@field public WASAPI integer
---@field public AudioScienceHPI integer

---@type portaudio_host_api_type_id
pa.host_api_type_id = libpa.host_api_type_id

---@class portaudio_sample_format
---@field public Float32 integer
---@field public Int32 integer
---@field public Int24 integer
---@field public Int16 integer
---@field public Int8 integer
---@field public UInt8 integer
---@field public CustomFormat integer
---@field public NonInterleaved integer

---@type portaudio_sample_format
pa.sample_format = libpa.sample_format

---@class portaudio_stream_flag
---@field public NoFlag integer
---@field public ClipOff integer
---@field public DitherOff integer
---@field public NeverDropInput integer
---@field public PrimeOutputBuffersUsingStreamCallback integer
---@field public PlatformSpecificFlags integer

---@type portaudio_stream_flag
pa.stream_flag = libpa.stream_flag

---@class portaudio_stream_callback_flag
---@field public InputUnderflow integer
---@field public InputOverflow integer
---@field public OutputUnderflow integer
---@field public OutputOverflow integer
---@field public PrimingOutput integer

---@type portaudio_stream_callback_flag
pa.stream_callback_flag = libpa.stream_callback_flag

---@class portaudio_stream_callback_result
---@field public Continue integer
---@field public Complete integer
---@field public Abort integer

---@type portaudio_stream_callback_result
pa.stream_callback_result = libpa.stream_callback_result

return pa
