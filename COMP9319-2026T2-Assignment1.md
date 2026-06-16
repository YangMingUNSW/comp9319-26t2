# COMP9319 2026T2 — Assignment 1: LZW Encoding and Decoding

> 原始题目页面：<https://cgi.cse.unsw.edu.au/~wong/cs9319-2026a1.html>
> 课程：COMP9319 Web Data Compression and Search
> 截止时间：**Tuesday, 30th June, 5:00pm AEST**
> 分值：**15 分**（自动评测 / auto-marked）

---

## 1. 任务总览 (Task Overview)

用 **C / C++** 实现一个 LZW 编码器 `lencode` 和解码器 `ldecode`：

- 字典为 **15-bit**，支持 **32,768** 个条目（不含单个 ASCII 字符所占的条目）。
- 当字典**写满**时，需要**重置 (reset)** 字典，然后像从头开始一样继续编码。
- 重置只在「目前已处理输入对应的全部输出码都已发出之后」才发生
  （*The reset occurs only after all output codes for the input processed so far have been emitted.*）。

---

## 2. 输入 / 输出格式 (I/O Format)

编码后的文件采用**变长**字节格式：

| 内容类型 | 编码方式 | 最高位 (MSB) |
|----------|----------|--------------|
| ASCII 字符 | 单字节存储 | `0` |
| 字典索引 (dictionary index) | 两字节存储 | `1` |

- 读取时，**最高有效字节 (most significant byte) 先读**。
- 假设输入文件可以包含**任意 7-bit ASCII 字符**。

### 字节示例

```
01011110            => '^'    (ASCII, 单字节, MSB=0)
10000000 00000100   => index 4 (字典索引, 两字节, MSB=1)
```

即压缩输出中 `^WE` 这个片段会被替换为 `10000000 00000100`（索引 4）。

---

## 3. 关键规则 (Key Rules)

### 3.1 至少 3 个字符才输出索引

> **An index to a dictionary entry will only be output when its entry is holding at least 3 characters.**

也就是说：长度为 2 的字典条目（如 `^W`、`WE`、`ED`）**不会**用索引输出，而是按原始字符直接输出；只有长度 ≥ 3 的条目（如 `^WE`）才用 2 字节索引输出。

### 3.2 字典重置

- 字典满（32,768 条）时，在加入新条目**之前**重置。
- 重置前必须先把当前已处理输入对应的所有输出码发出。
- 重置后从头开始构建字典继续编码。

---

## 4. 编码示例 (Worked Example)

测试输入 `test1.txt`：

```
^WED^WE^WEE^WEB^WET
```

按照 Lecture Week 2 (Slide 7–17) 中相同的变量追踪方式 `(p, c, output, index, symbol)`，
`test1.lzw` 的内容推导如下表：

| p   | c   | output | index | symbol |
|-----|-----|--------|-------|--------|
| NIL | ^   |        |       |        |
| ^   | W   | ^      | 0     | ^W     |
| W   | E   | W      | 1     | WE     |
| E   | D   | E      | 2     | ED     |
| D   | ^   | D      | 3     | D^     |
| ^   | W   |        |       |        |
| ^W  | E   | ^W     | 4     | ^WE    |
| E   | ^   | E      | 5     | E^     |
| ^   | W   |        |       |        |
| ^W  | E   |        |       |        |
| ^WE | E   | 4      | 6     | ^WEE   |
| E   | ^   |        |       |        |
| E^  | W   | E^     | 7     | E^W    |
| W   | E   |        |       |        |
| WE  | B   | WE     | 8     | WEB    |
| B   | ^   | B      | 9     | B^     |
| ^   | W   |        |       |        |
| ^W  | E   |        |       |        |
| ^WE | T   | 4      | 10    | ^WET   |
| T   | EOF | T      |       |        |

> 注意：`output` 列中诸如 `^W`、`WE`、`E^` 等长度为 2 的条目是以**原始字符**输出的（因为不满 3 字符），
> 而 `4`（即 `^WE`，长度 3）则以 **2 字节索引** `10000000 00000100` 输出 —— 这正体现了 §3.1 的规则。

---

## 5. 命令行接口 (CLI)

两个程序都接收**输入文件路径**和**输出文件路径**作为参数：

```bash
lencode <input_file>  <output_file>
ldecode <input_file>  <output_file>
```

---

## 6. 测试与性能 (Testing & Performance)

- 测试文件位置：`~cs9319/a1`
- 运行 sanity test：
  ```bash
  ~cs9319/a1/autotest
  ```
- 单个测试**最大执行时间：5 秒**
- 目标文件大小：最大约 **1 MB**

---

## 7. 提交 (Submission)

**截止时间：Tuesday, 30th June 5:00pm AEST**

```bash
give cs9319 a1 lencode.c ldecode.c
```
或

```bash
give cs9319 a1 lencode.cpp ldecode.cpp
```

### 编译命令 (评测时在 CSE Linux 机器上)

```bash
gcc -o lencode lencode.c
gcc -o ldecode ldecode.c
```

---

## 8. 评分 (Marking)

- 总分 **15 分**，自动评测。
- 在 **CSE Linux 机器**上编译运行。
- **代码可读性会影响分数。**
- **编译失败 = 0 分。**
- **迟交罚则：** 每天扣 5%；逾期 **5 天后不再接受**提交。

---

## 9. 学习参考：GitHub 上的 COMP9319 LZW 实现

> ⚠️ **学术诚信提醒**：UNSW 使用 MOSS / Turnitin 对代码查重，且下列公开仓库本身已在查重库中。直接抄袭会被命中并构成学术不端。以下仅供**理解 LZW 字节格式与算法思路**，请务必自行实现。

经过逐仓库比对，**相似度最高的是 2023T2 的 VegasFlower 版本**（字节格式与本期逐条一致）。学习策略：**以它为主、深入读透**，其余仓库只取个别可借鉴的点。

---

### 9.1 ⭐ 主参考（相似度最高，深入精读）：VegasFlower 2023T2

- 仓库：[VegasFlower/comp9319-WebDataCompressionAndSearch](https://github.com/VegasFlower/comp9319-WebDataCompressionAndSearch) → 目录 `assignment1/`
- 内容：`lencode.c`、`ldecode.c`、原始题面 `COMP9319 2023T2 Assignment 1.pdf`、`test1–7.bin`

**与本期 2026T2 规范逐条吻合：**

| 2026T2 规范 | 该仓库 `lencode.c` | 一致性 |
|---|---|---|
| 15-bit / 32,768 条字典 | `#define DICTIONARY_SIZE 32768` | ✅ |
| 索引两字节，MSB=1 | `convertIndexToBinary`: `1 << 15` | ✅ |
| 高位字节先写 | `bytes[0]=(x>>8)` 先 `fwrite` | ✅ |
| 条目 ≥3 字符才输出索引 | `if (strlen(currentSequence) >= 3)` | ✅ |
| CLI 两参数 `lencode <in> <out>` | `if (argc != 3)` + `gcc -o lencode lencode.c` | ✅ |

#### 9.1.1 `lencode.c` 编码逻辑精读

```c
// 主循环：维护 currentSequence (=p)，逐字符读 c
nextSequence = currentSequence + c;          // p + c
if (在字典中) {                               // 匹配更长串，继续累积
    currentSequence = nextSequence;
    last_index = entry->index;
} else {                                       // 失配 → 输出 p，并把 p+c 加入字典
    if (strlen(currentSequence) >= 3)          // ★ ≥3 字符 → 输出两字节索引
        write 2 bytes: convertIndexToBinary(last_index);
    else                                       //   否则按原始 ASCII 字符输出
        write currentSequence 的原始字节;
    if (nextIndex < DICTIONARY_SIZE)           // 加新条目
        insertDictionary(p+c, nextIndex++);
    currentSequence = c;  last_index = -1;
}
// 循环结束后还要把残余的 currentSequence 按同样规则 flush 一次
```

学习要点：
- **格式判定**：解码端靠每个字节的最高位区分 —— `0` = 单字节 ASCII，`1` = 两字节索引的高字节。
- **≥3 字符规则**：长度 1~2 的串（如 `^W`、`WE`）即便在字典里有索引，也仍按原始字符输出；只有 ≥3 才用 2 字节索引（省不了的短串不用索引，避免「2 字节索引 ≥ 2 个 ASCII 字符」反而变大）。
- **结尾 flush**：EOF 后 `currentSequence` 里还剩内容，必须再输出一次（代码末尾那段 `lastEntry` 处理）。

#### 9.1.2 `ldecode.c` 解码逻辑精读

```c
读首字符直接输出，作为 previous_symbol;
while (读入一个字节 currentByte) {
    if (currentByte & 0x80) {                  // 最高位=1 → 两字节索引
        再读一个字节 nextByte;
        idx = ((currentByte & 0x7F) << 8 | nextByte) & 0x7FFF;
        entry = searchDictionaryByIndex(idx);
        if (entry != NULL) current_symbol = entry->symbol;
        else { /* ★ KwKwK 特例：索引尚未建立 */
               current_symbol = previous_symbol + previous_symbol[0]; ... }
    } else {                                    // 最高位=0 → 单字节 ASCII
        current_symbol = (char)currentByte;
    }
    输出 current_symbol;
    new_symbol = previous_symbol + current_symbol[0];   // 建新条目 p + c[0]
    if (new_symbol 不在字典 && nextIndex < SIZE)
        insertDictionary(new_symbol, nextIndex++);
    previous_symbol = current_symbol;
}
```

学习要点：
- **双向字典**：`entries[]`（按串查 index，编码用）+ `reverseEntries[]`（按 index 查串，解码用）。解码侧用反向表 O(1) 取串。
- **★ KwKwK 特例**（LZW 经典难点）：当读到的索引**还没被加入字典**（编码端「先用后建」的情形），解码端需用 `previous_symbol + previous_symbol 的首字符` 来重建。务必看懂这段 `else` 分支。
- 编/解码两端**对字典的更新节奏必须完全对称**，否则索引会错位。

---

### 9.2 ⭐ 唯一要自己补的核心点：**字典重置 (dictionary reset)**

> 这是 2023T2 → 2026T2 **唯一新增的实质要求**，也是主参考代码**缺失**的部分。深入学完 9.1 后，重点攻克这里。

- **VegasFlower(2023T2) 的做法**：字典写满后**只停止新增、不重置**：
  ```c
  if (nextIndex < DICTIONARY_SIZE) { insertDictionary(...); nextIndex++; }
  ```
- **2026T2 新要求**：字典满时必须 **reset 字典**再继续编码
  （*reset it before adding a new entry … the reset occurs only after all output codes for the input processed so far have been emitted*）。

**影响与改造方向：**
- 输入小、字典塞不满（≤1MB 且重复模式少）→ 主参考行为与本期一致，能跑对。
- 输入大到塞满 32,768 条 → 主参考**与本期规范不符、编码结果错误**；autotest 很可能专门用大文件触发。
- 你需要在 `lencode` 和 `ldecode` 中**对称地**加入：当 `nextIndex` 达到上限时，先把当前待输出码全部 emit，再 `initializeDictionary()` 重置、`nextIndex` 归零，然后像从头一样继续。
- **借鉴点**：reset 的具体写法可参考下方 9.3 的 SongyuQi 2025T3（但要把它的「每 N 字节重置」改成本期的「写满时重置」）。

---

### 9.3 借鉴点（其余仓库，只取局部，不作主参考）

| 仓库 | 期/格式 | 可借鉴的点 | 为何不作主参考 |
|---|---|---|---|
| [SongyuQi-Francisco/UNSW-2025T3](https://github.com/SongyuQi-Francisco/UNSW-2025T3) → `9319/Ass1` | 2025T3，22-bit 变长 + 频率 N 重置 | **字典 reset 的写法**（先把残余 `p` 按最长匹配输出，再 `initialize_dictionary` 重置） | 22-bit 变长(0/10/11 前缀)+3 参数+4字节 header，输出格式**与本期不兼容**；reset 是「每 N 字节」非「写满时」 |
| [dioxyq/comp9319-25T3](https://github.com/dioxyq/comp9319-25T3) → `ass1/` | 25T3，变长 + 字典大小参数 | `autotest`/`automark` 脚本组织方式 | 变长方案 + 第三参数，输出格式与本期不兼容 |
| [Jerenyaoyelu/comp9319-ass1](https://github.com/Jerenyaoyelu/comp9319-ass1) | 2019T2 | LZW 整体结构（C++ 版） | 年份早、方案细节不同 |

#### 其他（非本题，仅留档）
- [gakkistyle/comp9319](https://github.com/gakkistyle/comp9319) — 23T2 课程资料
- [AsterWang/COMP9319_Ass2](https://github.com/AsterWang/COMP9319_Ass2) / [Jerenyaoyelu/rlebwt](https://github.com/Jerenyaoyelu/rlebwt) / [TomHuynhSG/COMP9319_BWT](https://github.com/TomHuynhSG/COMP9319_BWT) — 均为 **Assignment 2 (BWT)**
- [GitHub Topic: comp9319](https://github.com/topics/comp9319) — 课程相关仓库聚合
- 历年题目对照：[2024T2 Assignment 1](https://cgi.cse.unsw.edu.au/~wong/cs9319-2024a1.html)（22-bit / 4,194,304 条，与本期 15-bit 不同）

---

### 9.4 建议学习路径

1. **读懂格式** → 先用 9.1 的 `lencode.c`，对照 §4 的 `^WED^WE^WEE^WEB^WET` 例子手推一遍字节输出。
2. **读懂解码** → 9.1 的 `ldecode.c`，重点啃 **KwKwK 特例** 与双向字典。
3. **自己重写** → 不看原码、按本期规范从零实现 `lencode`/`ldecode`（避免 MOSS 命中）。
4. **补 reset** → 加入「写满时重置」逻辑，参考 9.3 SongyuQi 的 reset 思路，编/解码两端对称。
5. **验证** → 用 `~cs9319/a1/autotest`（小文件）+ 自造大文件（>32768 条目触发 reset）测试，注意 5 秒/1MB 限制。

---

*本文档由课程页面整理而成，日期：2026-06-16。如题目页面有更新请以官方页面为准。*
