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

### 字符串语法

```
string = quotation-mark *char quotation-mark
char = unescaped /
   escape (
       %x22 /          ; "    quotation mark  U+0022
       %x5C /          ; \    reverse solidus U+005C
       %x2F /          ; /    solidus         U+002F
       %x62 /          ; b    backspace       U+0008
       %x66 /          ; f    form feed       U+000C
       %x6E /          ; n    line feed       U+000A
       %x72 /          ; r    carriage return U+000D
       %x74 /          ; t    tab             U+0009
       %x75 4HEXDIG )  ; uXXXX                U+XXXX
escape = %x5C          ; \
quotation-mark = %x22  ; "
unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
```

JSON 字符串是由前后两个双引号夹着零至多个字符。字符分为无转义字符或转义序列。转义序列有 9 种，都是以反斜线开始，如常见的 `\n` 代表换行符。比较特殊的是 `\uXXXX`，当中 XXXX 为 16 进位的 UTF-16 编码，本单元将不处理这种转义序列，留待下回分解。

无转义字符就是普通的字符，语法中列出了合法的码点范围（码点还是在下单元才介绍）。要注意的是，该范围不包括 0 至 31、双引号和反斜线，这些码点都必须要使用转义方式表示。

