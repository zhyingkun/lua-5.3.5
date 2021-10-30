# pbdemo

文件后缀说明：

1. proto：ProtocolBuffer 协议
2. pb：根据 descriptor.proto 协议进行编码的 varint 格式的二进制数据，数据内容是 proto 协议
3. pbcbin：根据 pbc.proto 协议进行编码的 varint 格式的二进制数据，数据内容是 proto 协议
4. wdbin：根据用户自定义协议进行编码的 varint 格式的二进制数据，数据内容是用户定义的键值对

文件功能说明：

1. pb
    1. desc_simple_data.pbcbin: 由 pbc 源码中的 descriptor.pbc.h 中的二进制数据保存得到的简化版 descriptor.proto 对应的二进制数据，由 pbc.proto 协议进行描述
    2. desc_simple_tbl.pbcbin: 由 table 数据序列化得到的简化版 descriptor.proto 对应的二进制数据，由 pbc.proto 协议进行描述
    3. pbc.pb: pbc.proto 经过 protoc 编译得到的 varint 格式二进制
2. proto
    1. google/descriptor.proto: 谷歌完整版的 descriptor.proto
    2. desc_simple.proto: 云风简化版的 descriptor.proto
    3. pbc.proto: 用于描述简化版 descriptor.proto 的 proto
3. wdbin
    1. simple_err.wdbin: 错误的二进制数据
    2. simple_right.wdbin: 正确的 varint 二进制数据
4. CMakeLists.txt: 用于安装一部分 lua 版的 pb 工具
5. compileproto.sh: 将 proto 编译成 pb
6. decoderaw.lua: 实现与 protoc 命令--decode_raw 参数类似的功能
7. decompilepb.lua: 根据 Google 版 descriptor.proto 协议配置，使用 varint 解码方法进行解析 pb 文件，并转化成对应的 proto 源码
8. descdata2bin.lua: 将 C 语言代码版的 desc_simple.pbcbin 二进制数据写入独立文件中
9. desctbl2bin.lua: 用 Lua 配置表和 pbc.proto 协议手动生成简化版 descriptor 的二进制 varint 格式，内容与 descriptor.pbc.h 一致
10. makewdbin.lua: 创建 wiredata
11. pbcbin2proto.lua: 根据 pbc.proto 协议配置，使用 varint 解码方法进行解析 pbcbin 文件，并转化成对应的 proto 源码
