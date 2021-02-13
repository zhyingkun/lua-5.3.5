# pbdemo

文件功能说明：

1. src
    1. pbdemo.c: C 语言形式直接使用 pbc 库的示例
2. tools
    1. pb/descriptor.pbcbin: 简化版 descriptor.proto 对应的 varint 二进制格式，由 pbc.proto 协议进行描述
    2. proto
        1. google/descriptor.proto: 谷歌完整版的 descriptor.proto
        2. descriptor.proto: 云风简化版的 descriptor.proto
        3. pbc.proto: 用于描述简化版 descriptor.proto 的 proto
    3. decoderaw.lua: 实现与 protoc 命令--decode_raw 参数类似的功能
    4. descriptor.lua: 根据 Google 版 descriptor.proto 协议配置，使用 varint 解码方法进行解析 pb 文件，并转化成对应的 proto 源码
    5. pbc.lua: 根据 pbc.proto 协议配置，使用 varint 解码方法进行解析 pbcbin 文件，并转化成对应的 proto 源码（目前 pbcbin 文件只有一个：descriptor.pbcbin）
    6. pbcbin.lua: 将 C 语言代码版的 descriptor.pbcbin 二进制数据写入独立文件中
    7. varint.lua: 根据 varint 编码规则，将 varint 二进制数据解析成对应的键值对（键固定是数字）
3. CMakeLists.txt: 用于编译该 Demo 的 CMake 配置文件
