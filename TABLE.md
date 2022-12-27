# Table

## Table 模块各函数功能分类

| 函数名  | 对 list 和 hash 的支持 | 是否支持 Userdata+MetaMethod |
| ------- | ---------------------- | ---------------------------- |
| concat  | list                   | true                         |
| insert  |                        |                              |
| unpack  |                        |                              |
| remove  |                        |                              |
| move    |                        |                              |
| sort    |                        |                              |
| reduce  |                        |                              |
| reverse |                        |                              |
| ------  | ---------------------- | ---------------------------- |
| every   | list + hash, blist     | true when it's list          |
| indexOf |                        |                              |
| ------  | ---------------------- | ---------------------------- |
| filter  | list + hash, blist     | false                        |
| map     | list + hash            | false                        |
| slice   | list                   | false                        |
| splice  | list                   | false                        |
| ------  | ---------------------- | ---------------------------- |
| pack    | list                   | false                        |
| create  | list + hash            | false                        |
| copy    | list+hash              | false                        |
