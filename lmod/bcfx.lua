local libbcfx = require("libbcfx")
local libimage = libbcfx.image
local libgraphics3d = libbcfx.math.graphics3d
local libmatrix = libbcfx.math.matrix
local libvector = libbcfx.math.vector
local libmesh = libbcfx.mesh
local libcolor = libbcfx.color
local libutils = libbcfx.utils

local libuv = require("libuv")

local queueWorkAsync = libuv.queueWorkAsync
local queueWorkAsyncWait = libuv.queueWorkAsyncWait

---@class bcfx:table
local bcfx = {}

---@class bcfx_image:table
local image = {}
bcfx.image = image

--[[
** {======================================================
** ImageDecode
** =======================================================
--]]

---@param mb luaL_MemBuffer
---@param format bcfx_texture_format
---@return luaL_MemBuffer, integer, integer, integer, integer @data, w, h, nrChannels, wantChannels
function image.imageDecode(mb, format)
	return libimage.imageDecode(mb, format)
end
---@param mb luaL_MemBuffer
---@param format bcfx_texture_format
---@param callback fun(mb:luaL_MemBuffer, width:integer, height:integer, nrChannels:integer, wantChannels:integer):void
function image.imageDecodeAsync(mb, format, callback)
	local ptr = libimage.packImageDecodeParam(mb, format)
	queueWorkAsync(libimage.imageDecodePtr, ptr, function(result, status)
		callback(libimage.unpackImageDecodeResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@param format bcfx_texture_format
---@return luaL_MemBuffer, integer, integer, integer, integer @data, w, h, nrChannels, wantChannels
function image.imageDecodeAsyncWait(mb, format)
	local ptr = libimage.packImageDecodeParam(mb, format)
	local result = queueWorkAsyncWait(libimage.imageDecodePtr, ptr)
	return libimage.unpackImageDecodeResult(result)
end

-- }======================================================

--[[
** {======================================================
** ImageEncode
** =======================================================
--]]

---@param mb luaL_MemBuffer
---@param x integer
---@param y integer
---@param comp integer
---@param type bcfx_image_type
---@param sorq integer @strider or quality
---@return luaL_MemBuffer
function image.imageEncode(mb, x, y, comp, type, sorq)
	return libimage.imageEncode(mb, x, y, comp, type, sorq)
end
---@param mb luaL_MemBuffer
---@param x integer
---@param y integer
---@param comp integer
---@param type bcfx_image_type
---@param sorq integer @strider or quality
---@param callback fun(mb:luaL_MemBuffer):void
function image.imageEncodeAsync(mb, x, y, comp, type, sorq, callback) -- stride or quality
	local ptr = libimage.packImageEncodeParam(mb, x, y, comp, type, sorq);
	queueWorkAsync(libimage.imageEncodePtr, ptr, function(result, status)
		callback(libimage.unpackImageEncodeResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@param x integer
---@param y integer
---@param comp integer
---@param type bcfx_image_type
---@param sorq integer @strider or quality
---@return luaL_MemBuffer
function image.imageEncodeAsyncWait(mb, x, y, comp, type, sorq)
	local ptr = libimage.packImageEncodeParam(mb, x, y, comp, type, sorq);
	local result = queueWorkAsyncWait(libimage.imageEncodePtr, ptr)
	return libimage.unpackImageEncodeResult(result)
end

-- }======================================================

--[[
** {======================================================
** Image Utils
** =======================================================
--]]

---@param mb luaL_MemBuffer
---@param width integer
---@param height integer
---@return luaL_MemBuffer
function image.imageFlipVertical(mb, width, height)
	return libimage.imageFlipVertical(mb, width, height)
end
---@param mb luaL_MemBuffer
---@param width integer
---@param height integer
---@param callback fun(mb:luaL_MemBuffer):void
function image.imageFlipVerticalAsync(mb, width, height, callback)
	local ptr = libimage.packImageFlipVerticalParam(mb, width, height);
	queueWorkAsync(libimage.imageFlipVerticalPtr, ptr, function(result, status)
		callback(libimage.unpackImageFlipVerticalResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@param width integer
---@param height integer
---@return luaL_MemBuffer
function image.imageFlipVerticalAsyncWait(mb, width, height)
	local ptr = libimage.packImageFlipVerticalParam(mb, width, height);
	local result = queueWorkAsyncWait(libimage.imageFlipVerticalPtr, ptr)
	return libimage.unpackImageFlipVerticalResult(result)
end

-- }======================================================

---@class bcfx_image_type
---@field public PNG integer
---@field public BMP integer
---@field public TGA integer
---@field public JPG integer

---@type bcfx_image_type
image.image_type = libimage.image_type

---@class bcfx_math:table
local math = {}
bcfx.math = math

--[[
** {======================================================
** Graphics3D
** =======================================================
--]]

---@class bcfx_math_graphics3d:table
local graphics3d = {}
math.graphics3d = graphics3d

---@param vec Vec3
---@return Mat4x4
function graphics3d.translate(vec)
	return libgraphics3d.translate(vec)
end
---@param angle number @ in degree, not radian
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
---@overload fun(fovy:number, aspect:number, zNear:number):Mat4x4
---@overload fun(fovy:number, aspect:number, zNear:number, zFar:number):Mat4x4
---@param fovy number @ in degree, not radian
---@param aspect number @ width / height
---@param zNear number @ zNear > 0, means distance
---@param zFar number @ zFar > zNear > 0, means distance
---@return Mat4x4
function graphics3d.perspective(fovy, aspect, zNear, zFar)
	return libgraphics3d.perspective(fovy, aspect, zNear, zFar)
end
---@param left number
---@param right number
---@param bottom number
---@param top number
---@param near number @ map to -1, if near < far, orthogonal will leave handedness unchanged
---@param far number @ map to 1, if far < near, orthogonal will toggle handedness
---@return Mat4x4
function graphics3d.orthogonal(left, right, bottom, top, near, far)
	return libgraphics3d.orthogonal(left, right, bottom, top, near, far)
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

---@class bcfx_math_matrix:table
local matrix = {}
math.matrix = matrix

---@alias Matrix2Src1DstSignature fun(src1:Mat, src2:Mat):Mat
---@alias Matrix1Src1DstSignature fun(src:Mat):Mat

---@class Mat:userdata
---@field public set fun(self:Mat, row:integer, col:integer, value:number):void
---@field public get fun(self:Mat, row:integer, col:integer):number
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

---overload fun(mat:Mat):Mat
---overload fun(row:integer, col:integer):Mat
---overload fun(row:integer, col:integer, values:table):Mat
---@param row integer
---@param col integer
---@param values table
---@return Mat
function matrix.Matrix(row, col, values)
	return libmatrix.Matrix(row, col, values)
end

---@class Mat3x3:Mat

---overload fun(mat:Mat3x3):Mat3x3
---overload fun(...:number):Mat3x3
---@vararg number
---@return Mat3x3
function matrix.Mat3x3(...)
	return libmatrix.Mat3x3(...)
end

---@class Mat4x4:Mat

---overload fun(mat:Mat3x3):Mat4x4
---overload fun(mat:Mat4x4):Mat4x4
---overload fun(...:number):Mat4x4
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

---@class bcfx_math_vector:table
local vector = {}
math.vector = vector

---@alias Vector2Src1DstSignature fun(src1:Vec, src2:Vec):Vec
---@alias Vector1Src1DstSignature fun(src:Vec):Vec

---@class Vec:userdata
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

---@class bcfx_mesh:table
local mesh = {}
bcfx.mesh = mesh

--[[
** {======================================================
** MeshParse
** =======================================================
--]]

---@class Mesh:table
---@field vertex luaL_MemBuffer
---@field index luaL_MemBuffer

---@param mb luaL_MemBuffer
---@return Mesh[], string[]
function mesh.meshParse(mb)
	return libmesh.meshParse(mb)
end
---@param mb luaL_MemBuffer
---@param callback fun(meshs:Mesh[], materialNames:string[]):void
function mesh.meshParseAsync(mb, callback)
	local ptr = libmesh.packMeshParseParam(mb);
	queueWorkAsync(libmesh.meshParsePtr, ptr, function(result, status)
		callback(libmesh.unpackMeshParseResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@return Mesh[], string[]
function mesh.meshParseAsyncWait(mb)
	local ptr = libmesh.packMeshParseParam(mb);
	local result = queueWorkAsyncWait(libmesh.meshParsePtr, ptr)
	return libmesh.unpackMeshParseResult(result)
end

-- }======================================================

--[[
** {======================================================
** MaterialParse
** =======================================================
--]]

---@class Material:table
---@field name string
---@field ka Vec3
---@field kd Vec3
---@field ks Vec3
---@field ns number
---@field ni number
---@field d number
---@field illum integer
---@field mapKa string
---@field mapKd string
---@field mapKs string
---@field mapNs string
---@field mapD string
---@field mapBump string

---@param mb luaL_MemBuffer
---@return Material[]
function mesh.materialParse(mb)
	return libmesh.materialParse(mb)
end
---@param mb luaL_MemBuffer
---@param callback fun(materials:Material[]):void
function mesh.materialParseAsync(mb, callback)
	local ptr = libmesh.packMaterialParseParam(mb);
	queueWorkAsync(libmesh.materialParsePtr, ptr, function(result, status)
		callback(libmesh.unpackMaterialParseResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@return Material[]
function mesh.materialParseAsyncWait(mb)
	local ptr = libmesh.packMaterialParseParam(mb);
	local result = queueWorkAsyncWait(libmesh.materialParsePtr, ptr)
	return libmesh.unpackMaterialParseResult(result)
end

-- }======================================================

--[[
** {======================================================
** Inject API
** =======================================================
--]]

---@param create lightuserdata
---@param self lightuserdata
---@param invalid lightuserdata
---@param join lightuserdata
---@param equal lightuserdata
function bcfx.setThreadFuncs(create, self, invalid, join, equal)
	libbcfx.setThreadFuncs(create, self, invalid, join, equal)
end
---@param init lightuserdata
---@param destroy lightuserdata
---@param post lightuserdata
---@param wait lightuserdata
---@param tryWait lightuserdata
function bcfx.setSemFuncs(init, destroy, post, wait, tryWait)
	libbcfx.setSemFuncs(init, destroy, post, wait, tryWait)
end
---@param makeCurrent lightuserdata
---@param swapBuffers lightuserdata
---@param swapInterval lightuserdata
---@param getProcAddress lightuserdata
---@param getFramebufferSize lightuserdata
function bcfx.setWinCtxFuncs(makeCurrent, swapBuffers, swapInterval, getProcAddress, getFramebufferSize)
	libbcfx.setWinCtxFuncs(makeCurrent, swapBuffers, swapInterval, getProcAddress, getFramebufferSize)
end
---@param getTime lightuserdata
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

---@alias FrameCompletedSignature fun(frameId:integer):void

---@overload fun():void
---@overload fun(callback:FrameCompletedSignature):void
---@param callback FrameCompletedSignature
function bcfx.setFrameCompletedCallback(callback)
	libbcfx.setFrameCompletedCallback(callback)
end
---@param mainWin lightuserdata
function bcfx.init(mainWin)
	libbcfx.init(mainWin)
end
---@overload fun():void
---@overload fun(renderCount:integer):void
---@param renderCount integer
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

---@class Handle:integer

---@param layout bcfx_VertexLayout
---@return Handle
function bcfx.createVertexLayout(layout)
	return libbcfx.createVertexLayout(layout)
end
---@param mb luaL_MemBuffer
---@param layoutHandle Handle
---@return Handle
function bcfx.createVertexBuffer(mb, layoutHandle)
	return libbcfx.createVertexBuffer(mb, layoutHandle)
end
---@param size integer
---@param layoutHandle Handle
---@return Handle
function bcfx.createDynamicVertexBuffer(size, layoutHandle)
	return libbcfx.createDynamicVertexBuffer(size, layoutHandle)
end
---@param mb luaL_MemBuffer
---@param type bcfx_index_type
---@return Handle
function bcfx.createIndexBuffer(mb, type)
	return libbcfx.createIndexBuffer(mb, type)
end
---@param size integer
---@param type bcfx_index_type
---@return Handle
function bcfx.createDynamicIndexBuffer(size, type)
	return libbcfx.createDynamicIndexBuffer(size, type)
end
---@param mb string | luaL_MemBuffer
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
---@overload fun(name:string, type:bcfx_uniform_type, num:integer):Handle
---@param name string
---@param type bcfx_uniform_type
---@param num integer
---@return Handle
function bcfx.createUniform(name, type, num)
	return libbcfx.createUniform(name, type, num)
end
---@param mb luaL_MemBuffer
---@param width integer
---@param height integer
---@param format bcfx_texture_format
---@return Handle
function bcfx.createTexture(mb, width, height, format)
	return libbcfx.createTexture(mb, width, height, format)
end
---@param width integer
---@param height integer
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
---@param offset integer
---@param mb luaL_MemBuffer
function bcfx.updateDynamicVertexBuffer(handle, offset, mb)
	libbcfx.updateDynamicVertexBuffer(handle, offset, mb)
end
---@param handle Handle
---@param offset integer
---@param mb luaL_MemBuffer
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

---@class ViewId:integer

---@param id ViewId
---@param win lightuserdata
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
---@param depth number @ [-1.0, 1.0]
---@param stencil integer @ [0, 255]
function bcfx.setViewClear(id, flags, rgba, depth, stencil)
	libbcfx.setViewClear(id, flags, rgba, depth, stencil)
end
---@param id ViewId
---@param x integer
---@param y integer
---@param width integer
---@param height integer
function bcfx.setViewRect(id, x, y, width, height)
	libbcfx.setViewRect(id, x, y, width, height)
end
---@param id ViewId
---@param x integer
---@param y integer
---@param width integer
---@param height integer
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
---@param callback fun(frameId:integer, id:ViewId, width:integer, height:integer, mb:luaL_MemBuffer):void
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
---@vararg Vec4 | Mat3x3 | Mat4x4
function bcfx.setUniform(handle, ...)
	libbcfx.setUniform(handle, ...)
end
---@param id ViewId
function bcfx.touch(id)
	libbcfx.touch(id)
end
---@param stream integer
---@param handle Handle
function bcfx.setVertexBuffer(stream, handle)
	libbcfx.setVertexBuffer(stream, handle)
end
---@param handle Handle
---@param start integer
---@param count integer
function bcfx.setIndexBuffer(handle, start, count)
	libbcfx.setIndexBuffer(handle, start, count)
end
---@param mat Mat4x4
function bcfx.setTransform(mat)
	libbcfx.setTransform(mat)
end
---@param stage integer
---@param sampler Handle
---@param texture Handle
---@param flags bcfx_sampler_flag
function bcfx.setTexture(stage, sampler, texture, flags)
	libbcfx.setTexture(stage, sampler, texture, flags)
end
---@param x integer
---@param y integer
---@param width integer
---@param height integer
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

---@class bcfx_InstanceDataBuffer:table
---@field handle Handle
---@field bufferOffset integer
---@field numAttrib integer
---@field numInstance integer

---@param data bcfx_InstanceDataBuffer
---@param start integer
---@param count integer
function bcfx.setInstanceDataBuffer(data, start, count)
	libbcfx.setInstanceDataBuffer(data, start, count)
end
---@param id ViewId
---@param handle Handle
---@param flags integer
---@param depth integer
function bcfx.submit(id, handle, flags, depth)
	libbcfx.submit(id, handle, flags, depth)
end

---@class bcfx_clear_flag
---@field public NONE integer
---@field public COLOR integer
---@field public DEPTH integer
---@field public STENCIL integer

---@type bcfx_clear_flag
bcfx.clear_flag = libbcfx.clear_flag

---@class bcfx_vertex_attrib
---@field public Position integer
---@field public Normal integer
---@field public Tangent integer
---@field public Bitangent integer
---@field public Color0 integer
---@field public Color1 integer
---@field public Color2 integer
---@field public Color3 integer
---@field public Indices integer
---@field public Weight integer
---@field public TexCoord0 integer
---@field public TexCoord1 integer
---@field public TexCoord2 integer
---@field public TexCoord3 integer
---@field public TexCoord4 integer
---@field public TexCoord5 integer
---@field public TexCoord6 integer
---@field public TexCoord7 integer
---@field public Count integer

---@type bcfx_vertex_attrib
bcfx.vertex_attrib = libbcfx.vertex_attrib

---@class bcfx_attrib_type
---@field public Uint8 integer
---@field public Uint10 integer
---@field public Int16 integer
---@field public Half integer
---@field public Float integer

---@type bcfx_attrib_type
bcfx.attrib_type = libbcfx.attrib_type

---@class bcfx_index_type
---@field public Uint8 integer
---@field public Uint16 integer
---@field public Uint32 integer

---@type bcfx_index_type
bcfx.index_type = libbcfx.index_type

---@class bcfx_shader_type
---@field public Vertex integer
---@field public Fragment integer

---@type bcfx_shader_type
bcfx.shader_type = libbcfx.shader_type

---@class bcfx_texture_wrap
---@field public Repeat integer
---@field public Clamp integer

---@type bcfx_texture_wrap
bcfx.texture_wrap = libbcfx.texture_wrap

---@class bcfx_texture_filter
---@field public Linear integer
---@field public Nearest integer

---@type bcfx_texture_filter
bcfx.texture_filter = libbcfx.texture_filter

---@class bcfx_front_face
---@field public CounterClockWise integer
---@field public ClockWise integer

---@type bcfx_front_face
bcfx.front_face = libbcfx.front_face

---@class bcfx_cull_face
---@field public Back integer
---@field public Front integer
---@field public FrontAndBack integer

---@type bcfx_cull_face
bcfx.cull_face = libbcfx.cull_face

---@class bcfx_compare_func
---@field public Less integer
---@field public LEqual integer
---@field public Equal integer
---@field public GEqual integer
---@field public Greater integer
---@field public NotEqual integer
---@field public Never integer
---@field public Always integer

---@type bcfx_compare_func
bcfx.compare_func = libbcfx.compare_func

---@class bcfx_blend_func
---@field public Zero integer
---@field public One integer
---@field public SrcColor integer
---@field public OneMinusSrcColor integer
---@field public DstColor integer
---@field public OneMinusDstColor integer
---@field public SrcAlpha integer
---@field public OneMinusSrcAlpha integer
---@field public DstAlpha integer
---@field public OneMinusDstAlpha integer
---@field public ConstantColor integer
---@field public OneMinusConstantColor integer
---@field public ConstantAlpha integer
---@field public OneMinusConstantAlpha integer
---@field public SrcAlphaSaturate integer

---@type bcfx_blend_func
bcfx.blend_func = libbcfx.blend_func

---@class bcfx_blend_equation
---@field public FuncAdd integer
---@field public FuncSubtract integer
---@field public FuncReverseSubtract integer
---@field public Min integer
---@field public Max integer

---@type bcfx_blend_equation
bcfx.blend_equation = libbcfx.blend_equation

---@class bcfx_logic_operate
---@field public Copy integer
---@field public CopyInverted integer
---@field public Clear integer
---@field public Set integer
---@field public Noop integer
---@field public Invert integer
---@field public And integer
---@field public NAnd integer
---@field public Or integer
---@field public NOr integer
---@field public Xor integer
---@field public Equiv integer
---@field public AndReverse integer
---@field public AndInverted integer
---@field public OrReverse integer
---@field public OrInverted integer

---@type bcfx_logic_operate
bcfx.logic_operate = libbcfx.logic_operate

---@class bcfx_stencil_action
---@field public Keep integer
---@field public Zero integer
---@field public Replace integer
---@field public Incr integer
---@field public IncrWrap integer
---@field public Decr integer
---@field public DecrWrap integer
---@field public Invert integer

---@type bcfx_stencil_action
bcfx.stencil_action = libbcfx.stencil_action

---@class bcfx_view_mode
---@field public Default integer
---@field public Sequential integer
---@field public DepthAscending integer
---@field public DepthDescending integer
---@field public Count integer

---@type bcfx_view_mode
bcfx.view_mode = libbcfx.view_mode

---@class bcfx_debug
---@field public NONE integer
---@field public WIREFRAME integer

---@type bcfx_debug
bcfx.debug = libbcfx.debug

---@class bcfx_discard
---@field public NONE integer
---@field public VERTEX_STREAMS integer
---@field public INDEX_BUFFER integer
---@field public TRANSFORM integer
---@field public BINDINGS integer
---@field public STATE integer
---@field public INSTANCE_DATA integer
---@field public ALL integer

---@type bcfx_discard
bcfx.discard = libbcfx.discard

---@class bcfx_texture_format
---@field public RGB8 integer
---@field public RGBA8 integer
---@field public D24S8 integer

---@type bcfx_texture_format
bcfx.texture_format = libbcfx.texture_format

---@class bcfx_uniform_type
---@field public Sampler2D integer
---@field public Vec4 integer
---@field public Mat3x3 integer
---@field public Mat4x4 integer

---@type bcfx_uniform_type
bcfx.uniform_type = libbcfx.uniform_type

-- }======================================================

--[[
** {======================================================
** Color
** =======================================================
--]]

---@class Color:integer

---@class bcfx_color:table
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

---@overload fun(r:integer | number, g:integer | number, b:integer | number):Color
---@overload fun(r:integer | number, g:integer | number, b:integer | number, a:integer | number):Color
---@param r integer | number
---@param g integer | number
---@param b integer | number
---@param a integer | number
---@return Color
function color.pack(r, g, b, a)
	return libcolor.pack(r, g, b, a)
end
---@param rgba Color
---@return integer, integer, integer, integer @r, g, b, a
function color.unpack(rgba)
	return libcolor.unpack(rgba)
end
---@param rgba Color
---@return number, number, number, number @r, g, b, a
function color.unpackf(rgba)
	return libcolor.unpackf(rgba)
end

-- }======================================================

--[[
** {======================================================
** Pack data to MemBuffer
** =======================================================
--]]

---@alias DataGetterSignature fun():number, any @ return any count of numbers

---@overload fun(type:bcfx_data_type, data:number[]):luaL_MemBuffer
---@overload fun(type1:bcfx_data_type, dataGetter1:DataGetterSignature, type2:bcfx_data_type, dataGetter2:DataGetterSignature, ...):luaL_MemBuffer
---@return luaL_MemBuffer
function bcfx.makeMemBuffer(...)
	return libbcfx.makeMemBuffer(...)
end

---@class bcfx_data_type
---@field public Uint8 integer
---@field public Uint16 integer
---@field public Uint32 integer
---@field public Int8 integer
---@field public Int16 integer
---@field public Int32 integer
---@field public Half integer
---@field public Float integer

---@type bcfx_data_type
bcfx.data_type = libbcfx.data_type

-- }======================================================

--[[
** {======================================================
** Utils
** =======================================================
--]]

---@class bcfx_utils:table
local utils = {}
bcfx.utils = utils

---@class bcfx_SamplerFlag:table
---@field public wrapU bcfx_texture_wrap
---@field public wrapV bcfx_texture_wrap
---@field public filterMin bcfx_texture_filter
---@field public filterMag bcfx_texture_filter

---@class bcfx_sampler_flag:integer

---@param flags bcfx_SamplerFlag
---@return bcfx_sampler_flag
function utils.packSamplerFlags(flags)
	return libutils.packSamplerFlags(flags)
end

---@class bcfx_RenderState:table
---@field public frontFace bcfx_front_face
---@field public enableCull boolean
---@field public cullFace bcfx_cull_face
---@field public enableDepth boolean
---@field public depthFunc bcfx_compare_func
---@field public alphaRef integer
---@field public pointSize integer
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

---@class bcfx_render_state:integer

---@param state bcfx_RenderState
---@return bcfx_render_state
function utils.packRenderState(state)
	return libutils.packRenderState(state)
end

---@class bcfx_StencilState:table
---@field public func bcfx_compare_func
---@field public sfail bcfx_stencil_action
---@field public dpfail bcfx_stencil_action
---@field public dppass bcfx_stencil_action
---@field public ref integer
---@field public mask integer
---@field public writeMask integer

---@class bcfx_stencil_state:integer

---@param state bcfx_StencilState
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

---@class bcfx_VertexLayout:userdata
---@field public add fun(self:bcfx_VertexLayout, attrib:bcfx_vertex_attrib, num:integer, type:bcfx_attrib_type, normalized:boolean):void
---@field public skip fun(self:bcfx_VertexLayout, numbyte:integer):void
---@field public clear fun(self:bcfx_VertexLayout):void

---@return bcfx_VertexLayout
function bcfx.VertexLayout()
	return libbcfx.VertexLayout()
end

-- }======================================================

local thread = libuv.thread
local sem = thread.sem

local glfw = require("glfw")
local func_ptr = glfw.func_ptr

bcfx.setThreadFuncs(
	thread.thread_create,
	thread.thread_self,
	thread.thread_invalid,
	thread.thread_join,
	thread.thread_equal
)
bcfx.setSemFuncs(
	sem.sem_init,
	sem.sem_destroy,
	sem.sem_post,
	sem.sem_wait,
	sem.sem_trywait
)
bcfx.setWinCtxFuncs(
	func_ptr.MakeContextCurrent,
	func_ptr.SwapBuffers,
	func_ptr.SwapInterval,
	func_ptr.GetProcAddress,
	func_ptr.GetFramebufferSize
)
bcfx.setMiscFuncs(
	func_ptr.GetTime
)

return bcfx
