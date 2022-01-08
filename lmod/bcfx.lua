local libbcfx = require("libbcfx")
local libimage = libbcfx.image
local libgraphics3d = libbcfx.math.graphics3d
local libmatrix = libbcfx.math.matrix
local libvector = libbcfx.math.vector
local libmesh = libbcfx.mesh
local libcolor = libbcfx.color
local libmbio = libbcfx.mbio
local libmembuf = libbcfx.membuf
local libutils = libbcfx.utils

local running = coroutine.running
local yield = coroutine.yield
local resume = coroutine.resume

local libuv = require("libuv")

local ASYNC_WAIT_MSG = "AsyncWait api must running in coroutine"

---@class bcfx
local bcfx = {}

---@class bcfx_image
local image = {}
bcfx.image = image

--[[
** {======================================================
** ImageDecode
** =======================================================
--]]

---@param mb MemBuffer
---@param format bcfx_texture_format
---@return MemBuffer, number, number, number, number
function image.imageDecode(mb, format)
	return libimage.imageDecode(mb, format)
end
---@param mb MemBuffer
---@param format bcfx_texture_format
---@param callback fun(mb:MemBuffer, width:number, height:number, nrChannels:number, wantChannels:number):void
local function imageDecodeAsync(mb, format, callback)
	local ptr = libimage.packImageDecodeParam(mb, format)
	libuv.queue_work(libimage.imageDecodePtr, ptr, function(status, result)
		callback(libimage.unpackImageDecodeResult(result))
	end)
end
image.imageDecodeAsync = imageDecodeAsync
---@param mb MemBuffer
---@param format bcfx_texture_format
---@return MemBuffer, number, number, number, number
function image.imageDecodeAsyncWait(mb, format)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	imageDecodeAsync(mb, format, function(mb, width, height, nrChannels, wantChannels)
		resume(co, mb, width, height, nrChannels, wantChannels)
	end)
	return yield()
end

-- }======================================================

--[[
** {======================================================
** ImageEncode
** =======================================================
--]]

---@param mb MemBuffer
---@param x number
---@param y number
---@param comp number
---@param type bcfx_image_type
---@param sorq number @strider or quality
---@return MemBuffer
function image.imageEncode(mb, x, y, comp, type, sorq)
	return libimage.imageEncode(mb, x, y, comp, type, sorq)
end
---@param mb MemBuffer
---@param x number
---@param y number
---@param comp number
---@param type bcfx_image_type
---@param sorq number @strider or quality
---@param callback fun(mb:MemBuffer):void
local function imageEncodeAsync(mb, x, y, comp, type, sorq, callback) -- stride or quality
	local ptr = libimage.packImageEncodeParam(mb, x, y, comp, type, sorq);
	libuv.queue_work(libimage.imageEncodePtr, ptr, function(status, result)
		local mb = libimage.unpackImageEncodeResult(result)
		callback(mb)
	end)
end
image.imageEncodeAsync = imageEncodeAsync
---@param mb MemBuffer
---@param x number
---@param y number
---@param comp number
---@param type bcfx_image_type
---@param sorq number @strider or quality
---@return MemBuffer
function image.imageEncodeAsyncWait(mb, x, y, comp, type, sorq)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	imageEncodeAsync(mb, x, y, comp, type, sorq, function(mb)
		resume(co, mb)
	end)
	return yield()
end

-- }======================================================

--[[
** {======================================================
** Image Utils
** =======================================================
--]]

---@param mb MemBuffer
---@param width number
---@param height number
---@return MemBuffer
function image.imageFlipVertical(mb, width, height)
	return libimage.imageFlipVertical(mb, width, height)
end
---@param mb MemBuffer
---@param width number
---@param height number
---@param callback fun(mb:MemBuffer):void
local function imageFlipVerticalAsync(mb, width, height, callback)
	local ptr = libimage.packImageFlipVerticalParam(mb, width, height);
	libuv.queue_work(libimage.imageFlipVerticalPtr, ptr, function(status, result)
		callback(libimage.unpackImageFlipVerticalResult(result))
	end)
end
image.imageFlipVerticalAsync = imageFlipVerticalAsync
---@param mb MemBuffer
---@param width number
---@param height number
---@return MemBuffer
function image.imageFlipVerticalAsyncWait(mb, width, height)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	imageFlipVerticalAsync(mb, width, height, function(mb)
		resume(co, mb)
	end)
	return yield()
end

-- }======================================================

---@class bcfx_image_type
---@field public PNG number
---@field public BMP number
---@field public TGA number
---@field public JPG number

---@type bcfx_image_type
image.image_type = libimage.image_type

---@class bcfx_math
local math = {}
bcfx.math = math

--[[
** {======================================================
** Graphics3D
** =======================================================
--]]

---@class bcfx_math_graphics3d
local graphics3d = {}
math.graphics3d = graphics3d

---@param vec Vec3
---@return Mat4x4
function graphics3d.translate(vec)
	return libgraphics3d.translate(vec)
end
---@param angle number
---@param axis Vec3
---@return Mat4x4
function graphics3d.rotate(angle, axis)
	return libgraphics3d.rotate(angle, axis)
end
---@param vec Vec3
---@return Mat4x4
function graphics3d.scale(vec)
	return libgraphics3d.scale(vec)
end
---@param fovy number
---@param aspect number
---@param zNear number
---@param zFar number
---@return Mat4x4
function graphics3d.perspective(fovy, aspect, zNear, zFar)
	return libgraphics3d.perspective(fovy, aspect, zNear, zFar)
end
---@param left number
---@param right number
---@param bottom number
---@param top number
---@param zNear number
---@param zFar number
---@return Mat4x4
function graphics3d.orthogonal(left, right, bottom, top, zNear, zFar)
	return libgraphics3d.orthogonal(left, right, bottom, top, zNear, zFar)
end
---@param eye Vec3
---@param center Vec3
---@param up Vec3
---@return Mat4x4
function graphics3d.lookAt(eye, center, up)
	return libgraphics3d.lookAt(eye, center, up)
end

-- }======================================================

--[[
** {======================================================
** Matrix
** =======================================================
--]]

---@class bcfx_math_matrix
local matrix = {}
math.matrix = matrix

---@alias Matrix2Src1DstSignature fun(src1:Mat, src2:Mat):Mat
---@alias Matrix1Src1DstSignature fun(src:Mat):Mat

---@class Mat
---@field public set fun(self:Mat, row:number, col:number, value:number):void
---@field public get fun(self:Mat, row:number, col:number):number
---@field public add Matrix2Src1DstSignature
---@field public subtract Matrix2Src1DstSignature
---@field public componentWiseProduct Matrix2Src1DstSignature
---@field public scale fun(src:Mat, scale:number):Mat
---@field public multiply Matrix2Src1DstSignature
---@field public transpose Matrix1Src1DstSignature
---@field public copy Matrix1Src1DstSignature
---@field public determinant fun(src:Mat):number
---@field public adjoint Matrix1Src1DstSignature
---@field public inverse fun(src:Mat):(nil | Mat)

---@class Mat4x4:Mat

---@vararg number
---@return Mat4x4
function matrix.Mat4x4(...)
	return libmatrix.Mat4x4(...)
end

-- }======================================================

--[[
** {======================================================
** Vector
** =======================================================
--]]

---@class bcfx_math_vector
local vector = {}
math.vector = vector

---@alias Vector2Src1DstSignature fun(src1:Vec, src2:Vec):Vec
---@alias Vector1Src1DstSignature fun(src:Vec):Vec

---@class Vec
---@field public add Vector2Src1DstSignature
---@field public subtract Vector2Src1DstSignature
---@field public componentWiseProduct Vector2Src1DstSignature
---@field public scale fun(src:Vec, scale:number):Vec
---@field public dotProduct Vector2Src1DstSignature
---@field public lengthSquared fun(self:Vec):number
---@field public length fun(self:Vec):number
---@field public distanceSquared fun(src1:Vec, src2:Vec):number
---@field public distance fun(src1:Vec, src2:Vec):number
---@field public normalize Vector1Src1DstSignature
---@field public copy Vector2Src1DstSignature
---@field public isZero fun(self:Vec):boolean
---@field public projection Vector2Src1DstSignature
---@field public perpendicular Vector2Src1DstSignature

---@class Vec2:Vec

---@overload fun():Vec2
---@overload fun(x:number):Vec2
---@overload fun(x:number, y:number):Vec2
---@param x number
---@param y number
---@return Vec2
function vector.Vec2(x, y)
	return libvector.Vec2(x, y)
end

---@class Vec3:Vec
---@field public crossProduct Vector2Src1DstSignature

---@overload fun():Vec3
---@overload fun(x:number):Vec3
---@overload fun(x:number, y:number):Vec3
---@overload fun(x:number, y:number, z:number):Vec3
---@param x number
---@param y number
---@param z number
---@return Vec3
function vector.Vec3(x, y, z)
	return libvector.Vec3(x, y, z)
end

---@class Vec4:Vec

---@overload fun():Vec4
---@overload fun(x:number):Vec4
---@overload fun(x:number, y:number):Vec4
---@overload fun(x:number, y:number, z:number):Vec4
---@overload fun(x:number, y:number, z:number, w:number):Vec4
---@param x number
---@param y number
---@param z number
---@param w number
---@return Vec4
function vector.Vec4(x, y, z, w)
	return libvector.Vec4(x, y, z, w)
end

---@param src1 Vec
---@param src2 Vec
---@return Vec
function vector.max(src1, src2)
	return libvector.max(src1, src2)
end

---@param src1 Vec
---@param src2 Vec
---@return Vec
function vector.min(src1, src2)
	return libvector.min(src1, src2)
end

-- }======================================================

---@class bcfx_mesh
local mesh = {}
bcfx.mesh = mesh

--[[
** {======================================================
** MeshParse
** =======================================================
--]]

---@class Mesh
---@field vertex MemBuffer
---@field index MemBuffer

---@param mb MemBuffer
---@return Mesh[], string[]
function mesh.meshParse(mb)
	return libmesh.meshParse(mb)
end
---@param mb MemBuffer
---@param callback fun(meshs:Mesh[], materialNames:string[]):void
local function meshParseAsync(mb, callback)
	local ptr = libmesh.packMeshParseParam(mb);
	libuv.queue_work(libmesh.meshParsePtr, ptr, function(status, result)
		callback(libmesh.unpackMeshParseResult(result))
	end)
end
mesh.meshParseAsync = meshParseAsync
---@param mb MemBuffer
---@return Mesh[], string[]
function mesh.meshParseAsyncWait(mb)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	meshParseAsync(mb, function(meshs, materialNames)
		resume(co, meshs, materialNames)
	end)
	return yield()
end

-- }======================================================

--[[
** {======================================================
** MaterialParse
** =======================================================
--]]

---@class Material
---@field name string
---@field ka number
---@field kd number
---@field ks number
---@field ns number
---@field ni number
---@field d number
---@field illum number
---@field mapKa string
---@field mapKd string
---@field mapKs string
---@field mapNs string
---@field mapD string
---@field mapBump string

---@param mb MemBuffer
---@return Material[]
function mesh.materialParse(mb)
	return libmesh.materialParse(mb)
end
---@param mb MemBuffer
---@param callback fun(materials:Material[]):void
local function materialParseAsync(mb, callback)
	local ptr = libmesh.packMaterialParseParam(mb);
	libuv.queue_work(libmesh.materialParsePtr, ptr, function(status, result)
		callback(libmesh.unpackMaterialParseResult(result))
	end)
end
mesh.materialParseAsync = materialParseAsync
---@param mb MemBuffer
---@return Material[]
function mesh.materialParseAsyncWait(mb)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	materialParseAsync(mb, function(materials)
		resume(co, materials)
	end)
	return yield()
end

-- }======================================================

--[[
** {======================================================
** Error handler
** =======================================================
--]]

---@param callback fun(msg:string):string
function bcfx.setErrorMessageHandler(callback)
	libbcfx.SetErrorMessageHandler(callback)
end

-- }======================================================

--[[
** {======================================================
** Inject API
** =======================================================
--]]

---@param create userdata
---@param self userdata
---@param invalid userdata
---@param join userdata
---@param equal userdata
function bcfx.setThreadFuncs(create, self, invalid, join, equal)
	libbcfx.setThreadFuncs(create, self, invalid, join, equal)
end
---@param init userdata
---@param destroy userdata
---@param post userdata
---@param wait userdata
---@param tryWait userdata
function bcfx.setSemFuncs(init, destroy, post, wait, tryWait)
	libbcfx.setSemFuncs(init, destroy, post, wait, tryWait)
end
---@param makeCurrent userdata
---@param swapBuffers userdata
---@param swapInterval userdata
---@param getProcAddress userdata
---@param getFramebufferSize userdata
function bcfx.setWinCtxFuncs(makeCurrent, swapBuffers, swapInterval, getProcAddress, getFramebufferSize)
	libbcfx.setWinCtxFuncs(makeCurrent, swapBuffers, swapInterval, getProcAddress, getFramebufferSize)
end
---@param getTime userdata
function bcfx.setMiscFuncs(getTime)
	libbcfx.setMiscFuncs(getTime)
end

-- }======================================================

--[[
** {======================================================
** Basic API
** =======================================================
--]]

function bcfx.frameId()
	return libbcfx.frameId()
end
---@overload fun():void
---@overload fun(callback:)
---@param callback fun(frameId:number):void
function bcfx.setFrameCompletedCallback(callback)
	libbcfx.setFrameCompletedCallback(callback)
end
---@param mainWin userdata
function bcfx.init(mainWin)
	libbcfx.init(mainWin)
end
---@overload fun():void
---@overload fun(renderCount:number):void
---@param renderCount number
function bcfx.apiFrame(renderCount)
	libbcfx.apiFrame(renderCount)
end
function bcfx.shutdown()
	libbcfx.shutdown()
end

-- }======================================================

--[[
** {======================================================
** Create Render Resource
** =======================================================
--]]

---@class Handle:number

---@param layout bcfx_VertexLayout
---@return Handle
function bcfx.createVertexLayout(layout)
	return libbcfx.createVertexLayout(layout)
end
---@param mb MemBuffer
---@param layoutHandle Handle
---@return Handle
function bcfx.createVertexBuffer(mb, layoutHandle)
	return libbcfx.createVertexBuffer(mb, layoutHandle)
end
---@param size number
---@param layoutHandle Handle
---@return Handle
function bcfx.createDynamicVertexBuffer(size, layoutHandle)
	return libbcfx.createDynamicVertexBuffer(size, layoutHandle)
end
---@param mb MemBuffer
---@param type bcfx_index_type
---@return Handle
function bcfx.createIndexBuffer(mb, type)
	return libbcfx.createIndexBuffer(mb, type)
end
---@param size number
---@param type bcfx_index_type
---@return Handle
function bcfx.createDynamicIndexBuffer(size, type)
	return libbcfx.createDynamicIndexBuffer(size, type)
end
---@param mb string | MemBuffer
---@param type bcfx_shader_type
---@return Handle
function bcfx.createShader(mb, type)
	return libbcfx.createShader(mb, type)
end
---@param vs Handle
---@param fs Handle
---@return Handle
function bcfx.createProgram(vs, fs)
	return libbcfx.createProgram(vs, fs)
end
---@overload fun(name:string, type:bcfx_uniform_type):Handle
---@overload fun(name:string, type:bcfx_uniform_type, num:number):Handle
---@param name string
---@param type bcfx_uniform_type
---@param num number
---@return Handle
function bcfx.createUniform(name, type, num)
	return libbcfx.createUniform(name, type, num)
end
---@param mb MemBuffer
---@param width number
---@param height number
---@param format bcfx_texture_format
---@return Handle
function bcfx.createTexture(mb, width, height, format)
	return libbcfx.createTexture(mb, width, height, format)
end
---@param width number
---@param height number
---@param format bcfx_texture_format
---@return Handle
function bcfx.createRenderTexture(width, height, format)
	return libbcfx.createRenderTexture(width, height, format)
end
---@vararg Handle
---@return Handle
function bcfx.createFrameBuffer(...)
	return libbcfx.createFrameBuffer(...)
end

-- }======================================================

--[[
** {======================================================
** Update Render Resource
** =======================================================
--]]

---@param handle Handle
---@param vs Handle
---@param fs Handle
function bcfx.updateProgram(handle, vs, fs)
	libbcfx.updateProgram(handle, vs, fs)
end
---@param handle Handle
---@param offset number
---@param mb MemBuffer
function bcfx.updateDynamicVertexBuffer(handle, offset, mb)
	libbcfx.updateDynamicVertexBuffer(handle, offset, mb)
end
---@param handle Handle
---@param offset number
---@param mb MemBuffer
function bcfx.updateDynamicIndexBuffer(handle, offset, mb)
	libbcfx.updateDynamicIndexBuffer(handle, offset, mb)
end

-- }======================================================

--[[
** {======================================================
** Destroy Render Resource
** =======================================================
--]]

---@vararg Handle
function bcfx.destroy(...)
	libbcfx.destroy(...)
end

-- }======================================================

--[[
** {======================================================
** View
** =======================================================
--]]

---@class ViewId:number

---@param id ViewId
---@param win userdata
function bcfx.setViewWindow(id, win)
	libbcfx.setViewWindow(id, win)
end
---@param id ViewId
---@param handle Handle
function bcfx.setViewFrameBuffer(id, handle)
	libbcfx.setViewFrameBuffer(id, handle)
end
---@param id ViewId
---@param flags bcfx_clear_flag
---@param rgba Color
---@param depth number
---@param stencil number
function bcfx.setViewClear(id, flags, rgba, depth, stencil)
	libbcfx.setViewClear(id, flags, rgba, depth, stencil)
end
---@param id ViewId
---@param x number
---@param y number
---@param width number
---@param height number
function bcfx.setViewRect(id, x, y, width, height)
	libbcfx.setViewRect(id, x, y, width, height)
end
---@param id ViewId
---@param x number
---@param y number
---@param width number
---@param height number
function bcfx.setViewScissor(id, x, y, width, height)
	libbcfx.setViewScissor(id, x, y, width, height)
end
---@param id ViewId
---@param viewMat Mat4x4
---@param projMat Mat4x4
function bcfx.setViewTransform(id, viewMat, projMat)
	libbcfx.setViewTransform(id, viewMat, projMat)
end
---@param id ViewId
---@param mode bcfx_view_mode
function bcfx.setViewMode(id, mode)
	libbcfx.setViewMode(id, mode)
end
---@param id ViewId
---@param debug bcfx_debug
function bcfx.setViewDebug(id, debug)
	libbcfx.setViewDebug(id, debug)
end
---@param id ViewId
function bcfx.resetView(id)
	libbcfx.resetView(id)
end
---@param callback fun(frameId:number, id:ViewId, width:number, height:number, mb:MemBuffer):void
function bcfx.setFrameViewCaptureCallback(callback)
	libbcfx.setFrameViewCaptureCallback(callback)
end
---@param id ViewId
function bcfx.requestCurrentFrameViewCapture(id)
	libbcfx.requestCurrentFrameViewCapture(id)
end

-- }======================================================

--[[
** {======================================================
** Submit DrawCall
** =======================================================
--]]

---@param handle Handle
---@vararg any
function bcfx.setUniform(handle, ...)
	libbcfx.setUniform(handle, ...)
end
---@param id ViewId
function bcfx.touch(id)
	libbcfx.touch(id)
end
---@param stream number
---@param handle Handle
function bcfx.setVertexBuffer(stream, handle)
	libbcfx.setVertexBuffer(stream, handle)
end
---@param handle Handle
---@param start number
---@param count number
function bcfx.setIndexBuffer(handle, start, count)
	libbcfx.setIndexBuffer(handle, start, count)
end
---@param mat Mat4x4
function bcfx.setTransform(mat)
	libbcfx.setTransform(mat)
end
---@param stage number
---@param sampler Handle
---@param texture Handle
---@param flags bcfx_sampler_flag
function bcfx.setTexture(stage, sampler, texture, flags)
	libbcfx.setTexture(stage, sampler, texture, flags)
end
---@param x number
---@param y number
---@param width number
---@param height number
function bcfx.setScissor(x, y, width, height)
	libbcfx.setScissor(x, y, width, height)
end
---@param state bcfx_render_state
---@param rgba Color
function bcfx.setState(state, rgba)
	libbcfx.setState(state, rgba)
end
---@param enable boolean
---@param front bcfx_stencil_state
---@param back bcfx_stencil_state
function bcfx.setStencil(enable, front, back)
	libbcfx.setStencil(enable, front, back)
end

---@class bcfx_InstanceDataBuffer
---@field handle Handle
---@field bufferOffset number
---@field numAttrib number
---@field numInstance number

---@param data bcfx_InstanceDataBuffer
---@param start number
---@param count number
function bcfx.setInstanceDataBuffer(data, start, count)
	libbcfx.setInstanceDataBuffer(data, start, count)
end
---@param id ViewId
---@param handle Handle
---@param flags number
---@param depth number
function bcfx.submit(id, handle, flags, depth)
	libbcfx.submit(id, handle, flags, depth)
end

---@class bcfx_clear_flag
---@field public NONE number
---@field public COLOR number
---@field public DEPTH number
---@field public STENCIL number

---@type bcfx_clear_flag
bcfx.clear_flag = libbcfx.clear_flag

---@class bcfx_vertex_attrib
---@field public Position number
---@field public Normal number
---@field public Tangent number
---@field public Bitangent number
---@field public Color0 number
---@field public Color1 number
---@field public Color2 number
---@field public Color3 number
---@field public Indices number
---@field public Weight number
---@field public TexCoord0 number
---@field public TexCoord1 number
---@field public TexCoord2 number
---@field public TexCoord3 number
---@field public TexCoord4 number
---@field public TexCoord5 number
---@field public TexCoord6 number
---@field public TexCoord7 number
---@field public Count number

---@type bcfx_vertex_attrib
bcfx.vertex_attrib = libbcfx.vertex_attrib

---@class bcfx_attrib_type
---@field public Uint8 number
---@field public Uint10 number
---@field public Int16 number
---@field public Half number
---@field public Float number

---@type bcfx_attrib_type
bcfx.attrib_type = libbcfx.attrib_type

---@class bcfx_index_type
---@field public Uint8 number
---@field public Uint16 number
---@field public Uint32 number

---@type bcfx_index_type
bcfx.index_type = libbcfx.index_type

---@class bcfx_shader_type
---@field public Vertex number
---@field public Fragment number

---@type bcfx_shader_type
bcfx.shader_type = libbcfx.shader_type

---@class bcfx_texture_wrap
---@field public Repeat number
---@field public Clamp number

---@type bcfx_texture_wrap
bcfx.texture_wrap = libbcfx.texture_wrap

---@class bcfx_texture_filter
---@field public Linear number
---@field public Nearest number

---@type bcfx_texture_filter
bcfx.texture_filter = libbcfx.texture_filter

---@class bcfx_front_face
---@field public CounterClockWise number
---@field public ClockWise number

---@type bcfx_front_face
bcfx.front_face = libbcfx.front_face

---@class bcfx_cull_face
---@field public Back number
---@field public Front number
---@field public FrontAndBack number

---@type bcfx_cull_face
bcfx.cull_face = libbcfx.cull_face

---@class bcfx_compare_func
---@field public Less number
---@field public LEqual number
---@field public Equal number
---@field public GEqual number
---@field public Greater number
---@field public NotEqual number
---@field public Never number
---@field public Always number

---@type bcfx_compare_func
bcfx.compare_func = libbcfx.compare_func

---@class bcfx_blend_func
---@field public Zero number
---@field public One number
---@field public SrcColor number
---@field public OneMinusSrcColor number
---@field public DstColor number
---@field public OneMinusDstColor number
---@field public SrcAlpha number
---@field public OneMinusSrcAlpha number
---@field public DstAlpha number
---@field public OneMinusDstAlpha number
---@field public ConstantColor number
---@field public OneMinusConstantColor number
---@field public ConstantAlpha number
---@field public OneMinusConstantAlpha number
---@field public SrcAlphaSaturate number

---@type bcfx_blend_func
bcfx.blend_func = libbcfx.blend_func

---@class bcfx_blend_equation
---@field public FuncAdd number
---@field public FuncSubtract number
---@field public FuncReverseSubtract number
---@field public Min number
---@field public Max number

---@type bcfx_blend_equation
bcfx.blend_equation = libbcfx.blend_equation

---@class bcfx_logic_operate
---@field public Copy number
---@field public CopyInverted number
---@field public Clear number
---@field public Set number
---@field public Noop number
---@field public Invert number
---@field public And number
---@field public NAnd number
---@field public Or number
---@field public NOr number
---@field public Xor number
---@field public Equiv number
---@field public AndReverse number
---@field public AndInverted number
---@field public OrReverse number
---@field public OrInverted number

---@type bcfx_logic_operate
bcfx.logic_operate = libbcfx.logic_operate

---@class bcfx_stencil_action
---@field public Keep number
---@field public Zero number
---@field public Replace number
---@field public Incr number
---@field public IncrWrap number
---@field public Decr number
---@field public DecrWrap number
---@field public Invert number

---@type bcfx_stencil_action
bcfx.stencil_action = libbcfx.stencil_action

---@class bcfx_view_mode
---@field public Default number
---@field public Sequential number
---@field public DepthAscending number
---@field public DepthDescending number
---@field public Count number

---@type bcfx_view_mode
bcfx.view_mode = libbcfx.view_mode

---@class bcfx_debug
---@field public NONE number
---@field public WIREFRAME number

---@type bcfx_debug
bcfx.debug = libbcfx.debug

---@class bcfx_discard
---@field public NONE number
---@field public VERTEX_STREAMS number
---@field public INDEX_BUFFER number
---@field public TRANSFORM number
---@field public BINDINGS number
---@field public STATE number
---@field public INSTANCE_DATA number
---@field public ALL number

---@type bcfx_discard
bcfx.discard = libbcfx.discard

---@class bcfx_texture_format
---@field public RGB8 number
---@field public RGBA8 number
---@field public D24S8 number

---@type bcfx_texture_format
bcfx.texture_format = libbcfx.texture_format

-- }======================================================

--[[
** {======================================================
** Color
** =======================================================
--]]

---@class Color:number

---@class bcfx_color
local color = {}
bcfx.color = color

---@type Color
color.chartreuse = libcolor.chartreuse
---@type Color
color.yellow = libcolor.yellow
---@type Color
color.orange = libcolor.orange
---@type Color
color.red = libcolor.red
---@type Color
color.fuchsia = libcolor.fuchsia
---@type Color
color.magenta = libcolor.magenta
---@type Color
color.purple = libcolor.purple
---@type Color
color.blue = libcolor.blue
---@type Color
color.indigo = libcolor.indigo
---@type Color
color.cyan = libcolor.cyan
---@type Color
color.turquoise = libcolor.turquoise
---@type Color
color.green = libcolor.green
---@type Color
color.black = libcolor.black
---@type Color
color.white = libcolor.white

---@overload fun(r:number, g:number, b:number):Color
---@overload fun(r:number, g:number, b:number, a:number):Color
---@param r number
---@param g number
---@param b number
---@param a number
---@return Color
function color.pack(r, g, b, a)
	return libcolor.pack(r, g, b, a)
end
---@param rgba Color
---@return number, number, number, number
function color.unpack(rgba)
	return libcolor.unpack(rgba)
end
---@param rgba Color
---@return number, number, number, number
function color.unpackf(rgba)
	return libcolor.unpackf(rgba)
end

-- }======================================================

---@class bcfx_mbio
local mbio = {}
bcfx.mbio = mbio

--[[
** {======================================================
** ReadFile
** =======================================================
--]]

---@param fileName string
---@return MemBuffer
function mbio.readFile(fileName)
	return libmbio.readFile(fileName)
end
---@param fileName string
---@param callback fun(mb:MemBuffer | nil, errCode:nil | number, errStr:nil | string):void
local function readFileAsync(fileName, callback)
	local ptr = libmbio.packReadFileParam(fileName)
	libuv.queue_work(libmbio.readFilePtr, ptr, function(status, result)
		callback(libmbio.unpackReadFileResult(result))
	end)
end
mbio.readFileAsync = readFileAsync
---@param fileName string
---@return MemBuffer
function mbio.readFileAsyncWait(fileName)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	readFileAsync(fileName, function(mb, errCode, errStr)
		resume(co, mb, errCode, errStr)
	end)
	return yield()
end

-- }======================================================

--[[
** {======================================================
** WriteFile
** =======================================================
--]]

---@param fileName string
---@param mb MemBuffer
function mbio.writeFile(fileName, mb)
	libmbio.writeFile(fileName, mb)
end
---@param fileName string
---@param mb MemBuffer
---@param callback fun(ret:number):void
local function writeFileAsync(fileName, mb, callback)
	local ptr = libmbio.packWriteFileParam(fileName, mb)
	libuv.queue_work(libmbio.writeFilePtr, ptr, function(status, result)
		local ret = libmbio.unpackWriteFileResult(result)
		callback(ret)
	end)
end
mbio.writeFileAsync = writeFileAsync
---@param fileName string
---@param mb MemBuffer
function mbio.writeFileAsyncWait(fileName, mb)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	writeFileAsync(fileName, mb, function(ret)
		resume(co, ret)
	end)
	return yield()
end

-- }======================================================

---@class bcfx_membuf
local membuf = {}
bcfx.membuf = membuf

--[[
** {======================================================
** MemBuffer
** =======================================================
--]]

---@class MemBuffer
---@field public getClear fun(self:MemBuffer):userdata, number, userdata, userdata
---@field public setReplace fun(self:MemBuffer, ptr:userdata, sz:number, release:userdata, ud:userdata):void

---@overload fun():MemBuffer
---@overload fun(ptr:userdata, sz:number):MemBuffer
---@overload fun(ptr:userdata, sz:number, release:userdata):MemBuffer
---@overload fun(ptr:userdata, sz:number, release:userdata, ud:userdata):MemBuffer
---@param ptr userdata
---@param sz number
---@param release userdata
---@param ud userdata
---@return MemBuffer
function membuf.MemBuffer(ptr, sz, release, ud)
	return libmembuf.MemBuffer(ptr, sz, release, ud)
end

---@overload fun(type:bcfx_data_type, data:number[]):MemBuffer
---@overload fun(type:bcfx_data_type, dataGetter:fun():number, ..., ...):MemBuffer
---@param ptr userdata
---@param sz number
---@param release userdata
---@param ud userdata
---@return MemBuffer
function membuf.makeMemBuffer(...)
	return libmembuf.makeMemBuffer(...)
end

---@class bcfx_data_type
---@field public Uint8 number
---@field public Uint16 number
---@field public Uint32 number
---@field public Int8 number
---@field public Int16 number
---@field public Int32 number
---@field public Half number
---@field public Float number

---@type bcfx_data_type
membuf.data_type = libmembuf.data_type

-- }======================================================

--[[
** {======================================================
** Utils
** =======================================================
--]]

---@class bcfx_utils
local utils = {}
bcfx.utils = utils

---@class bcfx_SamplerFlag
---@field public wrapU bcfx_texture_wrap
---@field public wrapV bcfx_texture_wrap
---@field public filterMin bcfx_texture_filter
---@field public filterMag bcfx_texture_filter

---@param flags bcfx_SamplerFlag
---@return bcfx_sampler_flag
function utils.packSamplerFlags(flags)
	return libutils.packSamplerFlags(flags)
end

---@class bcfx_RenderState
---@field public frontFace bcfx_front_face
---@field public enableCull boolean
---@field public cullFace bcfx_cull_face
---@field public enableDepth boolean
---@field public depthFunc bcfx_compare_func
---@field public alphaRef number
---@field public pointSize number
---@field public noWriteR boolean
---@field public noWriteG boolean
---@field public noWriteB boolean
---@field public noWriteA boolean
---@field public noWriteZ boolean
---@field public enableBlend boolean
---@field public srcRGB bcfx_blend_func
---@field public dstRGB bcfx_blend_func
---@field public srcAlpha bcfx_blend_func
---@field public dstAlpha bcfx_blend_func
---@field public blendEquRGB bcfx_blend_equation
---@field public blendEquA bcfx_blend_equation
---@field public enableLogicOp boolean
---@field public logicOp bcfx_logic_operate

---@param flags bcfx_RenderState
---@return bcfx_render_state
function utils.packRenderState(state)
	return libutils.packRenderState(state)
end

---@class bcfx_StencilState
---@field public func bcfx_compare_func
---@field public sfail bcfx_stencil_action
---@field public dpfail bcfx_stencil_action
---@field public dppass bcfx_stencil_action
---@field public ref number
---@field public mask number
---@field public writeMask number

---@param flags bcfx_StencilState
---@return bcfx_stencil_state
function utils.packStencilState(state)
	return libutils.packStencilState(state)
end

-- }======================================================

--[[
** {======================================================
** VertexLayout
** =======================================================
--]]

---@class bcfx_VertexLayout
---@field public add fun(self:bcfx_VertexLayout, attrib:bcfx_vertex_attrib, num:number, type:bcfx_attrib_type, normalized:boolean):void
---@field public skip fun(self:bcfx_VertexLayout, numbyte:number):void
---@field public clear fun(self:bcfx_VertexLayout):void

---@return bcfx_VertexLayout
function bcfx.VertexLayout()
	return libbcfx.VertexLayout()
end

-- }======================================================

return bcfx
