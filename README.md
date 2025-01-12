# Jsonify

使用`C++ 11`规范撰写的`JSON`解析库，项目灵感来源[json-tutorial](https://github.com/miloyip/json-tutorial)。

## 项目规范

1. 使用 TDD 开发流程，坚持敏捷开发，快速迭代，持续重构。
2. 规范 Git commit 提交：
   - feat     - 新功能 feature
   - fix      - 修复 bug
   - docs     - 文档注释
   - style    - 代码格式(不影响代码运行的变动)
   - refactor - 重构、优化(既不增加新功能，也不是修复bug)
   - perf     - 性能优化
   - test     - 增加测试
   - chore    - 构建过程或辅助工具的变动
   - revert   - 回退 
   - build    - 打包

## JSON 语法

### 数字语法

```
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit
```

number 是以十进制表示，它主要由 4 部分顺序组成：负号、整数、小数、指数。只有整数是必需部分。注意和直觉可能不同的是，正号是不合法的。

整数部分如果是 0 开始，只能是单个 0；而由 1-9 开始的话，可以加任意数量的数字（0-9）。也就是说，`0123` 不是一个合法的 JSON 数字。

小数部分比较直观，就是小数点后是一或多个数字（0-9）。

JSON 可使用科学记数法，指数部分由大写 E 或小写 e 开始，然后可有正负号，之后是一或多个数字（0-9）。