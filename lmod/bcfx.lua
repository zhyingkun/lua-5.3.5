local libbcfx = require("libbcfx")

local libvector = libbcfx.math.vector
local libmatrix = libbcfx.math.matrix
local libeuler = libbcfx.math.euler
local libquaternion = libbcfx.math.quaternion
local libgraphics3d = libbcfx.math.graphics3d
local libtransform = libbcfx.math.transform

local libutils = libbcfx.utils
local libcolor = libbcfx.color
local libimage = libbcfx.image
local libmesh = libbcfx.mesh

local libuv = require("libuv")

local queueWorkAsync = libuv.queueWorkAsync
local queueWorkAsyncWait = libuv.queueWorkAsyncWait
local OK = libuv.err_code.OK
local ASYNC_WAIT_MSG = "AsyncWait api must running in coroutine"

---@class bcfx:table
local bcfx = {}

---@class bcfx_image:table
local image = {}
bcfx.image = image

--[[
** {======================================================
** Image Decode
** =======================================================
--]]

---@param mb luaL_MemBuffer
---@param format bcfx_texture_format
---@return luaL_MemBuffer, integer, integer, integer, integer @ data, w, h, nrChannels, wantChannels
function image.imageDecode(mb, format)
	return libimage.imageDecode(mb, format)
end
---@param mb luaL_MemBuffer
---@param format bcfx_texture_format
---@param callback fun(mb:luaL_MemBuffer, width:integer, height:integer, nrChannels:integer, wantChannels:integer):void
function image.imageDecodeAsync(mb, format, callback)
	local ptr = libimage.packImageDecodeParam(mb, format)
	queueWorkAsync(libimage.imageDecodePtr, ptr, function(result, status)
		if status ~= OK then printerr("image.imageDecodeAsync callback error: ", status) end
		callback(libimage.unpackImageDecodeResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@param format bcfx_texture_format
---@return luaL_MemBuffer, integer, integer, integer, integer @ data, w, h, nrChannels, wantChannels
function image.imageDecodeAsyncWait(mb, format)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	local ptr = libimage.packImageDecodeParam(mb, format)
	local result = queueWorkAsyncWait(libimage.imageDecodePtr, ptr)
	return libimage.unpackImageDecodeResult(result)
end

-- }======================================================

--[[
** {======================================================
** Image Encode
** =======================================================
--]]

-- stride in bytes: the distance in bytes from the first byte of
-- a row of pixels to the first byte of the next row of pixels

---@param mb luaL_MemBuffer
---@param x integer
---@param y integer
---@param comp integer @ color channel num
---@param type bcfx_image_type
---@param sorq integer @ stride or quality, stride in bytes for png, quality for jpg
---@return luaL_MemBuffer
function image.imageEncode(mb, x, y, comp, type, sorq)
	return libimage.imageEncode(mb, x, y, comp, type, sorq)
end
---@param mb luaL_MemBuffer
---@param x integer
---@param y integer
---@param comp integer @ color channel num
---@param type bcfx_image_type
---@param sorq integer @ stride or quality, stride in bytes for png, quality for jpg
---@param callback fun(mb:luaL_MemBuffer):void
function image.imageEncodeAsync(mb, x, y, comp, type, sorq, callback) -- stride or quality
	local ptr = libimage.packImageEncodeParam(mb, x, y, comp, type, sorq)
	queueWorkAsync(libimage.imageEncodePtr, ptr, function(result, status)
		if status ~= OK then printerr("image.imageEncodeAsync callback error: ", status) end
		callback(libimage.unpackImageEncodeResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@param x integer
---@param y integer
---@param comp integer @ color channel num
---@param type bcfx_image_type
---@param sorq integer @ stride or quality, stride in bytes for png, quality for jpg
---@return luaL_MemBuffer
function image.imageEncodeAsyncWait(mb, x, y, comp, type, sorq)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	local ptr = libimage.packImageEncodeParam(mb, x, y, comp, type, sorq)
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
		if status ~= OK then printerr("image.imageFlipVerticalAsync callback error: ", status) end
		callback(libimage.unpackImageFlipVerticalResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@param width integer
---@param height integer
---@return luaL_MemBuffer
function image.imageFlipVerticalAsyncWait(mb, width, height)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
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
** Euler Angle
** =======================================================
--]]

---@class bcfx_math_euler:table
local euler = {}
math.euler = euler

---@class EulerAngle:userdata
---@field public direction fun(self:EulerAngle):Vec3
---@field public toQuaternion fun(self:EulerAngle):Quaternion
---@field public toMatrix fun(self:EulerAngle):Mat4x4
---@field public get fun(self:EulerAngle):number, number, number @ pitch, roll, yaw
---@field public getVec3 fun(self:EulerAngle):Vec3
---@field public pitch number
---@field public roll number
---@field public yaw number

---@overload fun(ea:EulerAngle):EulerAngle
---@overload fun(vec:Vec3):EulerAngle
---@overload fun():EulerAngle
---@overload fun(pitch:number):EulerAngle
---@overload fun(pitch:number, roll:number):EulerAngle
---@overload fun(pitch:number, roll:number, yaw:number):EulerAngle
---@param pitch number @ [-180, 180]
---@param roll number @ [-180, 180]
---@param yaw number @ [-180, 180]
---@return EulerAngle
function euler.EulerAngle(pitch, roll, yaw)
	return libeuler.EulerAngle(pitch, roll, yaw)
end

-- }======================================================

--[[
** {======================================================
** Graphics 3D
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
---@param scale number
---@param axis Vec3
---@return Mat4x4
function graphics3d.scaleAxis(scale, axis)
	return libgraphics3d.scaleAxis(scale, axis)
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
---@param left number @ map to -1
---@param right number @ map to 1
---@param bottom number @ map to -1
---@param top number @ map to 1
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
---@param xCoeff Vec3
---@param yCoeff Vec3
---@param zCoeff Vec3
---@return Mat4x4
function graphics3d.shear(xCoeff, yCoeff, zCoeff)
	return libgraphics3d.shear(xCoeff, yCoeff, zCoeff)
end
---@param normal Vec3
---@param delta number
---@return Mat4x4
function graphics3d.reflection(normal, delta)
	return libgraphics3d.reflection(normal, delta)
end
---@param axis Vector
---@return Mat
function graphics3d.projection(axis)
	return libgraphics3d.projection(axis)
end
---@param axis Vector
---@return Mat
function graphics3d.perpendicular(axis)
	return libgraphics3d.perpendicular(axis)
end
---@param vec Vec3
---@return Mat3x3
function graphics3d.crossProduct(vec)
	return libgraphics3d.crossProduct(vec)
end

-- }======================================================

--[[
** {======================================================
** Transform
** =======================================================
--]]

---@class bcfx_math_transform:table
local transform = {}
math.transform = transform

---@class Transform:userdata
---@field public getLocation fun(self:Transform):Vec3
---@field public getRotation fun(self:Transform):EulerAngle
---@field public getScale fun(src:Transform):Vec3
---@field public setLocation fun(src:Transform, location:Vec3):void
---@field public setRotation fun(src:Transform, rotation:EulerAngle):void
---@field public setScale fun(src:Transform, scale:Vec3):void
---@field public toMatrix fun(src:Transform):Mat4x4
---@field public location Vec3
---@field public rotation EulerAngle
---@field public scale Vec3

---@overload fun():Transform
---@overload fun(location:Vec3):Transform
---@overload fun(location:Vec3, rotation:EulerAngle):Transform
---@overload fun(location:Vec3, rotation:EulerAngle, scale:Vec3):Transform
---@param location Vec3
---@param rotation EulerAngle
---@param scale Vec3
---@return Transform
function transform.Transform(location, rotation, scale)
	return libtransform.Transform(location, rotation, scale)
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

---@overload fun(mat:Mat):Mat
---@overload fun(row:integer, col:integer):Mat
---@overload fun(row:integer, col:integer, values:number[]):Mat
---@param row integer
---@param col integer
---@param values number[]
---@return Mat
function matrix.Matrix(row, col, values)
	return libmatrix.Matrix(row, col, values)
end

---@class Mat3x3:Mat

---@overload fun(mat:Mat3x3):Mat3x3
---@overload fun(...:number):Mat3x3
---@vararg number
---@return Mat3x3
function matrix.Mat3x3(...)
	return libmatrix.Mat3x3(...)
end

---@class Mat4x4:Mat

---@overload fun(mat:Mat3x3):Mat4x4
---@overload fun(mat:Mat4x4):Mat4x4
---@overload fun(...:number):Mat4x4
---@vararg number
---@return Mat4x4
function matrix.Mat4x4(...)
	return libmatrix.Mat4x4(...)
end

-- }======================================================

--[[
** {======================================================
** Quaternion
** =======================================================
--]]

---@class bcfx_math_quaternion:table
local quaternion = {}
math.quaternion = quaternion

---@class Quaternion:userdata
---@field public imaginary fun(self:Quaternion):Vec3
---@field public toEulerAngle fun(self:Quaternion):EulerAngle
---@field public toMatrix fun(self:Quaternion):Mat4x4
---@field public toAngleAxis fun(self:Quaternion):number, Vec3 @ angle, axis
---@field public add fun(self:Quaternion, src2:Quaternion):Quaternion
---@field public subtract fun(self:Quaternion, src2:Quaternion):Quaternion
---@field public multiply fun(self:Quaternion, src2:Quaternion):Quaternion
---@field public scale fun(self:Quaternion, scale:number):Quaternion
---@field public conjugate fun(self:Quaternion):Quaternion
---@field public normalize fun(self:Quaternion):Quaternion
---@field public inverse fun(self:Quaternion):Quaternion
---@field public dotProduct fun(self:Quaternion, src2:Quaternion):number
---@field public rotateVec3 fun(self:Quaternion, src:Vec3):Vec3
---@field public w number
---@field public x number
---@field public y number
---@field public z number

---@overload fun(quat:Quaternion):Quaternion
---@overload fun(imaginary:Vec3):Quaternion
---@overload fun(angle:number, axis:Vec3):Quaternion
---@overload fun():Quaternion
---@overload fun(w:number):Quaternion
---@overload fun(w:number, x:number):Quaternion
---@overload fun(w:number, x:number, y:number):Quaternion
---@overload fun(w:number, x:number, y:number, z:number):Quaternion
---@param w number @ [0, 1]
---@param x number @ [0, 1]
---@param y number @ [0, 1]
---@param z number @ [0, 1]
---@return Quaternion
function quaternion.Quaternion(w, x, y, z)
	return libquaternion.Quaternion(w, x, y, z)
end
---@param s Quaternion
---@param e Quaternion
---@param t number @ [0, 1]
---@return Quaternion
function quaternion.lerp(s, e, t)
	return libquaternion.lerp(s, e, t)
end
---@param s Quaternion
---@param e Quaternion
---return boolean
function quaternion.shortestPath(s, e)
	return libquaternion.shortestPath(s, e)
end
---@param s Quaternion
---@param e Quaternion
---@param t number @ [0, 1]
---@return Quaternion
function quaternion.slerp(s, e, t)
	return libquaternion.slerp(s, e, t)
end
---@param from Vec3
---@param to Vec3
---@return Quaternion
function quaternion.fromTo(from, to)
	return libquaternion.fromTo(from, to)
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

---@alias Vector2Src1DstSignature fun(src1:Vector, src2:Vector):Vector
---@alias Vector1Src1DstSignature fun(src:Vector):Vector

---@class Vector:userdata
---@field public add Vector2Src1DstSignature
---@field public subtract Vector2Src1DstSignature
---@field public componentWiseProduct Vector2Src1DstSignature
---@field public scale fun(src:Vector, scale:number):Vector
---@field public dotProduct Vector2Src1DstSignature
---@field public lengthSquared fun(self:Vector):number
---@field public length fun(self:Vector):number
---@field public distanceSquared fun(src1:Vector, src2:Vector):number
---@field public distance fun(src1:Vector, src2:Vector):number
---@field public normalize Vector1Src1DstSignature
---@field public copy Vector2Src1DstSignature
---@field public isZero fun(self:Vector):boolean
---@field public projection Vector2Src1DstSignature
---@field public perpendicular Vector2Src1DstSignature
---@field public x number
---@field public y number
---@field public z number
---@field public w number

---@overload fun(count:integer):Vector
---@overload fun(count:integer, values:number[]):Vector
---@param count integer
---@param values number[]
---@return Vector
function vector.Vector(count, values)
	return libvector.Vector(count, values)
end

---@class Vec2:Vector

---@overload fun():Vec2
---@overload fun(x:number):Vec2
---@overload fun(x:number, y:number):Vec2
---@param x number
---@param y number
---@return Vec2
function vector.Vec2(x, y)
	return libvector.Vec2(x, y)
end

---@class Vec3:Vector
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

---@class Vec4:Vector

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

---@param src1 Vector
---@param src2 Vector
---@return Vector
function vector.max(src1, src2)
	return libvector.max(src1, src2)
end

---@param src1 Vector
---@param src2 Vector
---@return Vector
function vector.min(src1, src2)
	return libvector.min(src1, src2)
end

---@type Vec3
vector.zero = libvector.zero
---@type Vec3
vector.one = libvector.one
---@type Vec3
vector.up = libvector.up
---@type Vec3
vector.down = libvector.down
---@type Vec3
vector.forward = libvector.forward
---@type Vec3
vector.backward = libvector.backward
---@type Vec3
vector.right = libvector.right
---@type Vec3
vector.left = libvector.left

-- }======================================================

---@class bcfx_mesh:table
local mesh = {}
bcfx.mesh = mesh

--[[
** {======================================================
** Mesh Parse
** =======================================================
--]]

---@class Mesh:table
---@field public vertex luaL_MemBuffer
---@field public index luaL_MemBuffer

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
		if status ~= OK then printerr("mesh.meshParseAsync callback error: ", status) end
		callback(libmesh.unpackMeshParseResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@return Mesh[], string[]
function mesh.meshParseAsyncWait(mb)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	local ptr = libmesh.packMeshParseParam(mb);
	local result = queueWorkAsyncWait(libmesh.meshParsePtr, ptr)
	return libmesh.unpackMeshParseResult(result)
end

-- }======================================================

--[[
** {======================================================
** Material Parse
** =======================================================
--]]

---@class Material:table
---@field public name string
---@field public ka Vec3
---@field public kd Vec3
---@field public ks Vec3
---@field public ns number
---@field public ni number
---@field public d number
---@field public illum integer
---@field public mapKa string
---@field public mapKd string
---@field public mapKs string
---@field public mapNs string
---@field public mapD string
---@field public mapBump string

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
		if status ~= OK then printerr("mesh.materialParseAsync callback error: ", status) end
		callback(libmesh.unpackMaterialParseResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@return Material[]
function mesh.materialParseAsyncWait(mb)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
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

---@overload fun(mainWin:lightuserdata):void
---@overload fun(mainWin:lightuserdata, initMask:bcfx_init_flag_mask):void
---@param mainWin lightuserdata
---@param initMask bcfx_init_flag_mask @ combine bcfx_init_flag_mask with '|'
function bcfx.init(mainWin, initMask)
	libbcfx.init(mainWin, initMask)
end
---@overload fun():void
---@overload fun(renderCount:integer):void
---@param renderCount integer @ max draw call
function bcfx.apiFrame(renderCount)
	libbcfx.apiFrame(renderCount)
end
function bcfx.shutdown()
	libbcfx.shutdown()
end

-- }======================================================

--[[
** {======================================================
** Frame ID
** =======================================================
--]]

---@return integer
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

-- }======================================================

--[[
** {======================================================
** Create Render Resource
** =======================================================
--]]

---@class Handle:integer

---@type Handle
bcfx.kInvalidHandle = libbcfx.kInvalidHandle

---@param mb luaL_MemBuffer
---@param layout bcfx_VertexLayout
---@return Handle
function bcfx.createVertexBuffer(mb, layout)
	return libbcfx.createVertexBuffer(mb, layout)
end
---@param size integer
---@param layout bcfx_VertexLayout
---@return Handle
function bcfx.createDynamicVertexBuffer(size, layout)
	return libbcfx.createDynamicVertexBuffer(size, layout)
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
---@param mb string | luaL_MemBuffer
---@param type bcfx_shader_type
---@param path string
---@return Handle
function bcfx.createIncludeShader(mb, type, path)
	return libbcfx.createIncludeShader(mb, type, path)
end
---@overload fun(vs:Handle):Handle
---@overload fun(vs:Handle, fs:Handle):Handle
---@param vs Handle @ vertex shader handle
---@param fs Handle @ fragment shader handle
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
---@param flags bcfx_sampler_flag
---@return Handle
function bcfx.createSampler(flags)
	return libbcfx.createSampler(flags)
end
---@overload fun(format:bcfx_texture_format, mb:luaL_MemBuffer, width:integer):Handle
---@overload fun(format:bcfx_texture_format, mb:luaL_MemBuffer, width:integer, bGenMipmap:boolean):Handle
---@param format bcfx_texture_format
---@param mb luaL_MemBuffer
---@param width integer
---@param bGenMipmap boolean
---@return Handle
function bcfx.createTexture1D(format, mb, width, bGenMipmap)
	return libbcfx.createTexture1D(format, mb, width, bGenMipmap)
end
---@overload fun(format:bcfx_texture_format, mba:luaL_MemBuffer[], width:integer):Handle
---@overload fun(format:bcfx_texture_format, mba:luaL_MemBuffer[], width:integer, bGenMipmap:boolean):Handle
---@param format bcfx_texture_format
---@param mba luaL_MemBuffer[]
---@param width integer
---@param bGenMipmap boolean
---@return Handle
function bcfx.createTexture1DArray(format, mba, width, bGenMipmap)
	return libbcfx.createTexture1DArray(format, mba, width, bGenMipmap)
end
---@overload fun(format:bcfx_texture_format, mb:luaL_MemBuffer, width:integer, height:integer):Handle
---@overload fun(format:bcfx_texture_format, mb:luaL_MemBuffer, width:integer, height:integer, bGenMipmap:boolean):Handle
---@param format bcfx_texture_format
---@param mb luaL_MemBuffer
---@param width integer
---@param height integer
---@param bGenMipmap boolean
---@return Handle
function bcfx.createTexture2D(format, mb, width, height, bGenMipmap)
	return libbcfx.createTexture2D(format, mb, width, height, bGenMipmap)
end
---@overload fun(format:bcfx_texture_format, mba:luaL_MemBuffer[], width:integer, height:integer):Handle
---@overload fun(format:bcfx_texture_format, mba:luaL_MemBuffer[], width:integer, height:integer, bGenMipmap:boolean):Handle
---@param format bcfx_texture_format
---@param mba luaL_MemBuffer[]
---@param width integer
---@param height integer
---@param bGenMipmap boolean
---@return Handle
function bcfx.createTexture2DArray(format, mba, width, height, bGenMipmap)
	return libbcfx.createTexture2DArray(format, mba, width, height, bGenMipmap)
end
---@overload fun(format:bcfx_texture_format, mba:luaL_MemBuffer[], width:integer, height:integer):Handle
---@overload fun(format:bcfx_texture_format, mba:luaL_MemBuffer[], width:integer, height:integer, bGenMipmap:boolean):Handle
---@param format bcfx_texture_format
---@param mba luaL_MemBuffer[]
---@param width integer
---@param height integer
---@param bGenMipmap boolean
---@return Handle
function bcfx.createTexture3D(format, mba, width, height, bGenMipmap)
	return libbcfx.createTexture3D(format, mba, width, height, bGenMipmap)
end
---@overload fun(format:bcfx_texture_format, mb6:luaL_MemBuffer[], width:integer):Handle
---@overload fun(format:bcfx_texture_format, mb6:luaL_MemBuffer[], width:integer, bGenMipmap:boolean):Handle
---@param format bcfx_texture_format
---@param mb6 luaL_MemBuffer[]
---@param width integer
---@param bGenMipmap boolean
---@return Handle
function bcfx.createTextureCubeMap(format, mb6, width, bGenMipmap)
	return libbcfx.createTextureCubeMap(format, mb6, width, bGenMipmap)
end
---@param format bcfx_texture_format
---@param mba luaL_MemBuffer[]
---@param width integer
---@param height integer
---@return Handle
function bcfx.createTexture2DMipmap(format, mba, width, height)
	return libbcfx.createTexture2DMipmap(format, mba, width, height)
end
---@param format bcfx_texture_format
---@param width integer
---@param height integer
---@return Handle
function bcfx.createRenderTexture(format, width, height)
	return libbcfx.createRenderTexture(format, width, height)
end
---@vararg Handle @ render texture handle
---@return Handle
function bcfx.createFrameBuffer(...)
	return libbcfx.createFrameBuffer(...)
end

---@param numVec4PerInstance integer
---@param numInstance integer
function bcfx.instanceDataBufferSize(numVec4PerInstance, numInstance)
	--[[
	local sizePerFloat = 4
	local sizePerVec4 = 4 * sizePerFloat
	local sizePerInstance = sizePerVec4 * numVec4PerInstance
	return sizePerInstance * numInstance
	--]]
	return 16 * numVec4PerInstance * numInstance
end
---@param mb luaL_MemBuffer
---@param numVec4PerInstance integer
---@return Handle
function bcfx.createInstanceDataBuffer(mb, numVec4PerInstance)
	return libbcfx.createInstanceDataBuffer(mb, numVec4PerInstance)
end
---@param numInstance integer
---@param numVec4PerInstance integer
---@return Handle
function bcfx.createDynamicInstanceDataBuffer(numInstance, numVec4PerInstance)
	return libbcfx.createDynamicInstanceDataBuffer(numInstance, numVec4PerInstance)
end

-- }======================================================

--[[
** {======================================================
** Update Render Resource
** =======================================================
--]]

---@param handle Handle
---@param mb luaL_MemBuffer
function bcfx.updateShader(handle, mb)
	libbcfx.updateShader(handle, mb)
end
---@param handle Handle
---@param vs Handle
---@param fs Handle
function bcfx.updateProgram(handle, vs, fs)
	libbcfx.updateProgram(handle, vs, fs)
end
---@param handle Handle
---@param offset integer @ start byte in buffer for update
---@param mb luaL_MemBuffer
function bcfx.updateDynamicBuffer(handle, offset, mb)
	libbcfx.updateDynamicBuffer(handle, offset, mb)
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
---@param handle Handle @ framebuffer handle
function bcfx.setViewFrameBuffer(id, handle)
	libbcfx.setViewFrameBuffer(id, handle)
end
---@param id ViewId
---@param clearMask bcfx_clear_flag_mask @ combining bcfx_clear_flag_mask with '|'
---@param rgba Color @ color buffer clear value
---@param depth number @ [-1.0, 1.0], depth buffer clear value, usually 1.0
---@param stencil integer @ [0, 255], stencil buffer clear value, usually 0
function bcfx.setViewClear(id, clearMask, rgba, depth, stencil)
	libbcfx.setViewClear(id, clearMask, rgba, depth, stencil)
end
---@param id ViewId
---@param x integer @ rect: origin is LeftBottom, x towards right, y towards top, unit is pixel
---@param y integer
---@param width integer
---@param height integer
function bcfx.setViewRect(id, x, y, width, height)
	libbcfx.setViewRect(id, x, y, width, height)
end
---@param id ViewId
---@param x integer @ rect: origin is LeftBottom, x towards right, y towards top, unit is pixel
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
---@param near number
---@param far number
function bcfx.setViewDepthRange(id, near, far)
	libbcfx.setViewDepthRange(id, near, far)
end
---@param id ViewId
---@param debugMask bcfx_debug_flag_mask @ combine bcfx_debug_flag_mask with '|'
function bcfx.setViewDebug(id, debugMask)
	libbcfx.setViewDebug(id, debugMask)
end
---@param id ViewId
function bcfx.resetView(id)
	libbcfx.resetView(id)
end

-- }======================================================

--[[
** {======================================================
** Frame View Capture
** =======================================================
--]]

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
---@vararg number | integer | boolean | Vec4 | Mat3x3 | Mat4x4
function bcfx.setUniform(handle, ...)
	libbcfx.setUniform(handle, ...)
end
---@param id ViewId
function bcfx.touch(id)
	libbcfx.touch(id)
end
---@overload fun(stream:integer, handle:Handle):void
---@overload fun(stream:integer, handle:Handle, attribMask:bcfx_vertex_attrib_mask):void
---@param stream integer
---@param handle Handle
---@param attribMask bcfx_vertex_attrib_mask @ combine bcfx_vertex_attrib_mask with '|'
function bcfx.setVertexBuffer(stream, handle, attribMask)
	libbcfx.setVertexBuffer(stream, handle, attribMask)
end
---@overload fun(handle:Handle):void
---@overload fun(handle:Handle, start:integer):void
---@overload fun(handle:Handle, start:integer, count:integer):void
---@overload fun(handle:Handle, start:integer, count:integer, baseVertex:integer):void
---@param handle Handle @ maybe kInvalidHandle
---@param start integer @ for kInvalidHandle, count in vertex, not float, not byte, otherwise count in indices
---@param count integer @ for kInvalidHandle, count in vertex, not float, not byte, otherwise count in indices
---@param baseVertex integer
function bcfx.setIndexBuffer(handle, start, count, baseVertex)
	libbcfx.setIndexBuffer(handle, start, count, baseVertex)
end
---@param mat Mat4x4
function bcfx.setTransform(mat)
	libbcfx.setTransform(mat)
end
---@param stage integer @ start from 1
---@param uniform Handle @ uniform handle
---@param texture Handle @ texture handle
---@param sampler Handle @ sampler handle
function bcfx.setTexture(stage, uniform, texture, sampler)
	libbcfx.setTexture(stage, uniform, texture, sampler)
end
---@param x integer @ rect: origin is LeftBottom, x towards right, y towards top, unit is pixel
---@param y integer
---@param width integer
---@param height integer
function bcfx.setScissor(x, y, width, height)
	libbcfx.setScissor(x, y, width, height)
end
---@overload fun(state:bcfx_render_state):void
---@overload fun(state:bcfx_render_state, rgba:Color):void
---@param state bcfx_render_state
---@param rgba Color @ blend color
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
---@field public handle Handle
---@field public bufferOffset integer @ skip num of bytes, for multi instance data using the same vertex buffer
---@field public numAttrib integer @ num of vec4 per instance
---@field public numInstance integer

---@overload fun(numInstance:integer):void
---@overload fun(numInstance:integer, handle:Handle):void
---@overload fun(numInstance:integer, handle:Handle, startInstance:integer):void
---@param numInstance integer
---@param handle Handle @ instance data buffer handle
---@param startInstance integer @ skip num of instance, not vec4, not float, not byte
function bcfx.setInstanceDataBuffer(numInstance, handle, startInstance)
	libbcfx.setInstanceDataBuffer(numInstance, handle, startInstance)
end
---@overload fun(id:ViewId, handle:Handle):void
---@overload fun(id:ViewId, handle:Handle, flags:bcfx_discard_flag_mask):void
---@overload fun(id:ViewId, handle:Handle, flags:bcfx_discard_flag_mask, sortDepth:integer):void
---@overload fun(id:ViewId, handle:Handle, flags:bcfx_discard_flag_mask, sortDepth:integer, primitiveType:bcfx_primitive_type):void
---@param id ViewId
---@param handle Handle @ shader program
---@param discardMask bcfx_discard_flag_mask @ combining bcfx_discard_flag_mask with '|'
---@param sortDepth integer @ [0, 2^24-1], 24bit in sortkey
---@param primitiveType bcfx_primitive_type
function bcfx.submit(id, handle, discardMask, sortDepth, primitiveType)
	libbcfx.submit(id, handle, discardMask, sortDepth, primitiveType)
end

-- }======================================================

--[[
** {======================================================
** Builtin
** =======================================================
--]]

---@param type bcfx_builtin_mesh_type
---@return Handle, Handle, bcfx_primitive_type @ vertex, index, primitive
function bcfx.getBuiltinMesh(type)
	return libbcfx.getBuiltinMesh(type)
end
---@param type bcfx_builtin_shader_type
---@return Handle @ shader program
function bcfx.getBuiltinShaderProgram(type)
	return libbcfx.getBuiltinShaderProgram(type)
end

-- }======================================================

---@class bcfx_init_flag
---@field public FramebufferSRGB integer

---@type bcfx_init_flag
bcfx.init_flag = libbcfx.init_flag

---@class bcfx_init_flag_mask
---@field public FramebufferSRGB integer

---@type bcfx_init_flag_mask
bcfx.init_flag_mask = libbcfx.init_flag_mask

---@class bcfx_clear_flag
---@field public Color integer
---@field public Depth integer
---@field public Stencil integer
---@field public Count integer

---@type bcfx_clear_flag
bcfx.clear_flag = libbcfx.clear_flag

---@class bcfx_clear_flag_mask
---@field public None integer
---@field public Color integer
---@field public Depth integer
---@field public Stencil integer
---@field public All integer

---@type bcfx_clear_flag_mask
bcfx.clear_flag_mask = libbcfx.clear_flag_mask

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

---@class bcfx_vertex_attrib_mask
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
---@field public All integer

---@type bcfx_vertex_attrib_mask
bcfx.vertex_attrib_mask = libbcfx.vertex_attrib_mask

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
---@field public Count integer

---@type bcfx_shader_type
bcfx.shader_type = libbcfx.shader_type

---@class bcfx_texture_wrap
---@field public Repeat integer
---@field public ClampToEdge integer
---@field public ClampToBorder integer

---@type bcfx_texture_wrap
bcfx.texture_wrap = libbcfx.texture_wrap

---@class bcfx_texture_filter
---@field public Linear integer
---@field public Nearest integer

---@type bcfx_texture_filter
bcfx.texture_filter = libbcfx.texture_filter

---@class bcfx_texture_compare_mode
---@field public None integer
---@field public RefToTexture integer

---@type bcfx_texture_compare_mode
bcfx.texture_compare_mode = libbcfx.texture_compare_mode

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

---@class bcfx_debug_flag
---@field public Wireframe integer
---@field public Pointset integer

---@type bcfx_debug_flag
bcfx.debug_flag = libbcfx.debug_flag

---@class bcfx_debug_flag_mask
---@field public Wireframe integer
---@field public Pointset integer

---@type bcfx_debug_flag_mask
bcfx.debug_flag_mask = libbcfx.debug_flag_mask

---@class bcfx_discard_flag
---@field public VertexStreams integer
---@field public IndexBuffer integer
---@field public Transform integer
---@field public Bindings integer
---@field public State integer
---@field public InstanceData integer
---@field public Count integer

---@type bcfx_discard_flag
bcfx.discard_flag = libbcfx.discard_flag

---@class bcfx_discard_flag_mask
---@field public None integer
---@field public VertexStreams integer
---@field public IndexBuffer integer
---@field public Transform integer
---@field public Bindings integer
---@field public State integer
---@field public InstanceData integer
---@field public All integer

---@type bcfx_discard_flag_mask
bcfx.discard_flag_mask = libbcfx.discard_flag_mask

---@class bcfx_primitive_type
---@field public Default integer
---@field public Points integer
---@field public Lines integer
---@field public LineStrip integer
---@field public LineLoop integer
---@field public Triangles integer
---@field public TriangleStrip integer
---@field public TriangleFan integer

---@type bcfx_primitive_type
bcfx.primitive_type = libbcfx.primitive_type

---@class bcfx_texture_format
---@field public R8 integer
---@field public R16 integer
---@field public RG8 integer
---@field public RG16 integer
---@field public RGB8 integer
---@field public RGBA8 integer
---@field public SRGB8 integer
---@field public SRGBA8 integer
---@field public R32F integer
---@field public RGB16F integer
---@field public RGBA16F integer
---@field public RGB32F integer
---@field public RGBA32F integer
---@field public D24S8 integer

---@type bcfx_texture_format
bcfx.texture_format = libbcfx.texture_format

---@class bcfx_uniform_type
---@field public Float integer
---@field public Int integer
---@field public Bool integer
---@field public Vec4 integer
---@field public Mat3x3 integer
---@field public Mat4x4 integer
---@field public Sampler1D integer
---@field public Sampler1DArray integer
---@field public Sampler2D integer
---@field public Sampler2DArray integer
---@field public Sampler3D integer
---@field public SamplerCubeMap integer
---@field public SamplerBuffer integer

---@type bcfx_uniform_type
bcfx.uniform_type = libbcfx.uniform_type

---@class bcfx_builtin_mesh_type
---@field public Triangle integer
---@field public Square integer
---@field public Circle integer
---@field public Tetrahedron integer
---@field public Cube integer
---@field public Sphere integer
---@field public Cylinder integer
---@field public Cone integer

---@type bcfx_builtin_mesh_type
bcfx.builtin_mesh_type = libbcfx.builtin_mesh_type

---@class bcfx_builtin_shader_type
---@field public Default integer

---@type bcfx_builtin_shader_type
bcfx.builtin_shader_type = libbcfx.builtin_shader_type

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

---@class bcfx_color_pack_type
---@field public ABGR8 integer
---@field public A2BGR10 integer

---@type bcfx_color_pack_type
color.pack_type = libcolor.pack_type

---@overload fun(r:integer | number, g:integer | number, b:integer | number):Color
---@overload fun(r:integer | number, g:integer | number, b:integer | number, a:integer | number):Color
---@overload fun(r:integer | number, g:integer | number, b:integer | number, a:integer | number, t:bcfx_color_pack_type):Color
---@param r integer | number
---@param g integer | number
---@param b integer | number
---@param a integer | number
---@param t bcfx_color_pack_type
---@return Color
function color.pack(r, g, b, a, t)
	return libcolor.pack(r, g, b, a, t)
end
---@overload fun(rgba:Color):integer, integer, integer, integer
---@overload fun(rgba:Color, t:bcfx_color_pack_type):integer, integer, integer, integer
---@param rgba Color
---@param t bcfx_color_pack_type
---@return integer, integer, integer, integer @r, g, b, a
function color.unpack(rgba, t)
	return libcolor.unpack(rgba, t)
end
---@overload fun(rgba:Color):number, number, number, number
---@overload fun(rgba:Color, t:bcfx_color_pack_type):number, number, number, number
---@param rgba Color
---@param t bcfx_color_pack_type
---@return number, number, number, number @r, g, b, a
function color.unpackNF(rgba, t)
	return libcolor.unpackNF(rgba, t)
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
---@field public wrapW bcfx_texture_wrap
---@field public wrapMirror boolean
---@field public compareMode bcfx_texture_compare_mode
---@field public compareFunc bcfx_compare_func
---@field public filterMin bcfx_texture_filter
---@field public filterMag bcfx_texture_filter
---@field public enableMipmap boolean
---@field public filterMipmap bcfx_texture_filter
---@field public enableAniso boolean
---@field public borderColor Color

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
---@field public nextGroup fun(self:bcfx_VertexLayout, bufferOffset:integer):void
---@field public addAttrib fun(self:bcfx_VertexLayout, attrib:bcfx_vertex_attrib, num:integer, type:bcfx_attrib_type, normalized:boolean):void
---@field public skipAttrib fun(self:bcfx_VertexLayout, numByte:integer):void
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
