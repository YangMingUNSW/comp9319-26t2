# COMP9319 Assignment 1 — LZW 学习笔记

> 配套文件：题目整理见 [`spec.md`](./spec.md)；参考源码见 [`reference/`](./reference/)；本期解答见 [`lencode.c`](./lencode.c) / [`ldecode.c`](./ldecode.c) 与 [`README.md`](./README.md)。
> ⚠️ 本笔记仅用于**理解算法**。参考源码均来自公开仓库（已在 MOSS/Turnitin 查重库中），**不可照抄**，需自行实现。

---

## 0. 一句话目标

实现 `lencode` / `ldecode`：用 **15-bit (32,768 条) 字典**做 LZW 压缩/解压，
ASCII 用 1 字节 (MSB=0)、字典索引用 2 字节 (MSB=1)、索引项 **≥3 字符才输出**、字典**写满时重置**。

---

## 1. 字节格式（必须先吃透）

| 类型 | 字节数 | 最高位 | 说明 |
|---|---|---|---|
| ASCII 字符 | 1 | `0` | 直接是 7-bit ASCII |
| 字典索引 | 2 | `1` | 高字节先写；低 15 位是 index |

读取时按**首字节最高位**判断走哪条路：`byte & 0x80 == 0` → ASCII；`== 1` → 再读一字节拼成 15-bit 索引。

参考实现（VegasFlower 2023T2）：
- 编码写索引：`index | (1<<15)`，再 `byte0=(x>>8)&0xFF; byte1=x&0xFF`
- 解码读索引：`idx = ((b0 & 0x7F) << 8 | b1) & 0x7FFF`

---

## 2. 核心算法骨架

### 编码 (lencode)
```
p = 第一个字符
for each 后续字符 c:
    if (p+c) 在字典:  p = p+c            // 继续延长
    else:
        输出 p                            // ★ ≥3字符→2字节索引, 否则原始字符
        把 (p+c) 加入字典                  // 写满则 reset (本期新增)
        p = c
输出 p                                     // 结尾 flush, 别漏
```

### 解码 (ldecode)
```
读首字节直接输出, prev = 它
for each 编码单元 (1或2字节):
    if 索引:
        if 索引在字典: cur = 字典[索引]
        else:          cur = prev + prev[0]   // ★ KwKwK 特例
    else: cur = 该 ASCII 字符
    输出 cur
    把 (prev + cur[0]) 加入字典               // 写满则 reset, 与编码对称
    prev = cur
```

---

## 3. 三个最容易错的点

### 3.1 ≥3 字符才输出索引
长度 1~2 的串即使在字典里也按**原始字符**输出。
原因：2 字节索引并不比 2 个 ASCII 字符短，强行用索引反而不压缩 / 还原困难。
> 看 `lencode.c` 里 `if (strlen(currentSequence) >= 3)` 那个分支。

### 3.2 KwKwK 特例（LZW 经典坑）
解码时可能遇到一个**字典里还不存在的索引**（编码端「先输出、下一步才建该条目」）。
处理：`cur = prev + prev 的首字符`。
> 看 `ldecode.c` 里 `searchDictionaryByIndex` 返回 NULL 的 `else` 分支。

### 3.3 字典重置（★ 本期新增，参考代码都没有现成的「写满时重置」）
- 2023T2 主参考：写满后只是停止新增（`if (nextIndex < DICTIONARY_SIZE)`）→ **不符合本期**。
- 本期要求：**写满时重置**，且「重置只在当前已处理输入的全部输出码都发出后才发生」。
- 编码与解码**必须对称**重置，否则索引错位。
- reset 写法可借鉴 SongyuQi 2025T3（它是「每 N 字节」重置；把触发条件换成「`nextIndex` 到达上限」即可）。

---

## 4. 参考源码导读（`reference/`）

| 文件 | 来源 | 看什么 | 注意 |
|---|---|---|---|
| `VegasFlower-2023T2/lencode.c` | 2023T2 | 编码主循环、≥3 字符规则、2字节索引写法、结尾 flush | **格式与本期一致**，但无写满重置 |
| `VegasFlower-2023T2/ldecode.c` | 2023T2 | 双向字典、MSB 判定、**KwKwK 特例** | 同上 |
| `SongyuQi-2025T3/lencode.cpp` | 2025T3 | **字典 reset 的实现思路** | 22-bit 变长 + 3 参数 + header，格式**不兼容本期**，只借 reset |
| `SongyuQi-2025T3/ldecode.cpp` | 2025T3 | reset 时解码端如何对称处理 | 同上 |

---

## 5. 学习路径（建议顺序）

1. **手推例子**：用题面 `^WED^WE^WEE^WEB^WET`，照 §4 表（在 `spec.md`）逐步推出每个字节。
2. **精读编码**：`VegasFlower-2023T2/lencode.c`，确认每行对应 §2 骨架。
3. **精读解码**：`VegasFlower-2023T2/ldecode.c`，重点 KwKwK + 双向字典。
4. **理解 reset**：读 `SongyuQi-2025T3/*.cpp`，搞懂「输出残余 → 重置字典 → 继续」的流程。
5. **自己从零写**（等你决定开始 assignment 时）：不看原码，按本期规范实现，再补「写满时重置」。
6. **验证**：`~cs9319/a1/autotest`（小文件）+ 自造 >32768 条目的大文件（触发 reset）；注意 **5 秒 / 1MB** 限制，编译 `gcc -o lencode lencode.c`。

---

## 6. 自检清单（写完作业后逐条核对）

- [ ] ASCII 单字节 MSB=0，索引两字节 MSB=1，高字节先写
- [ ] 仅当串 ≥3 字符时输出索引，否则原始字符
- [ ] 编码结尾正确 flush 残余 `p`
- [ ] 解码正确处理 KwKwK 特例
- [ ] 字典写满时编/解码**对称重置**，且在残余码发出后才重置
- [ ] 编码→解码能**无损还原**任意 7-bit ASCII 输入
- [ ] 1MB 文件在 5 秒内完成
- [ ] `gcc -o lencode lencode.c` / `gcc -o ldecode ldecode.c` 无报错
- [ ] 代码可读性（命名/注释）—— 影响分数

---

*生成日期：2026-06-16。当前阶段：资料整理与学习，尚未开始编写 assignment。*
