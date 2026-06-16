# reference/ — 仅供学习的外部参考（含测试文件与 autotest）

> ⚠️ **学术诚信**：本目录代码全部来自 GitHub 公开仓库，已在 UNSW MOSS/Turnitin 查重库中。
> **仅用于理解算法、复用测试输入、学习测试方法**。直接复制代码 = 学术不端。
> 📌 这里**不是**我的 assignment 解答 —— 本期作业尚未开始编写。

---

## 已克隆的仓库（sparse-checkout，仅 ass1 目录）

| 子目录 | 来源 / 期次 | 路径 | 格式与本期关系 |
|---|---|---|---|
| `VegasFlower-2023T2/assignment1/` | [VegasFlower](https://github.com/VegasFlower/comp9319-WebDataCompressionAndSearch) · 2023T2 | `lencode.c` `ldecode.c` + PDF题面 + `test1–7.bin` | ⭐ **主参考**，15-bit/2字节/≥3字符**与本期一致**（缺写满重置） |
| `SongyuQi-2025T3/9319/Ass1/` | [SongyuQi](https://github.com/SongyuQi-Francisco/UNSW-2025T3) · 2025T3 | 源码 + 编译产物 + **autotest** + test1–10 全套 | 22-bit 变长，**格式不兼容**；借 reset 思路 + 测试方法 |
| `dioxyq-25T3/ass1/` | [dioxyq](https://github.com/dioxyq/comp9319-25T3) · 25T3 | `src/` + `tests/` + **autotest** + **automark** + CMakeLists | 22-bit 变长，**格式不兼容**；借测试/打分脚本结构 |

> 每个子目录都是完整 git clone（含 `.git`，可 `git pull` 更新）。若不需要历史，可删 `.git` 省空间（当前总计约 42M）。

---

## 📋 他们是怎么 test 的（autotest 流程）

以 `SongyuQi-2025T3/9319/Ass1/autotest` 为例，官方 autotest 思路一致：

1. **编译**：`gcc -o lencode lencode.c`（或 `g++ ... .cpp`），`ldecode` 同理。
2. **编码检查**：`./lencode 输入.txt 输出.bin` → 与官方期望 `.lzw` 做 `diff -q`，一致即 CORRECT。
3. **解码检查（两轮）**：
   - a) 解码**你自己**编码出的 `.bin` → 与原始 `.txt` diff（验证你 encode↔decode **自洽**/round-trip）。
   - b) 解码**官方** `.lzw` → 与原始 `.txt` diff（验证你的 decoder 能读官方编码）。
4. 全程注意 **5 秒 / 文件 ≤1MB** 限制。

> 本期官方文件在 CSE：`~cs9319/a1/`（`test*.txt` 输入、`test*.lzw` 期望编码输出、`autotest` 脚本）。
> SongyuQi 的 autotest 里 `./lencode ... 10/245/...` 第三个数字是 **2025T3 特有的 reset 频率参数**，本期**没有**这个参数（本期是写满才重置）。

---

## ✅ 哪些能复用，⚠️ 哪些不能（关键）

| 文件类型 | 能否用于本期 2026 | 说明 |
|---|---|---|
| `*.txt`（原始输入） | ✅ **能复用** | 任意 7-bit ASCII 文本，适合做 round-trip：你的 `lencode`→`ldecode` 结果必须等于原 `.txt`。含大文件（test8 = 3MB）可压力测试 reset |
| `.lzw` / `.bin`（期望编码输出） | ⚠️ **不能直接对比** | 它们按**各自年份格式**生成。SongyuQi/dioxyq 是 **22-bit 变长**，与本期 **15-bit 固定两字节不兼容**，diff 必然不等 |
| `VegasFlower/*.bin` | 🔶 仅供参考 | 是 2023T2（格式与本期一致）其作者 encoder 的输出，但**无官方 `.lzw` 佐证**，不保证 100% 正确，别当标准答案 |
| `autotest` / `automark` | ✅ 学方法 | 看测试**流程**即可；本期请以 `~cs9319/a1/autotest` 为准 |

### 给本期的实用建议
- **拿 `.txt` 做 round-trip 测试**：`lencode in.txt out.bin && ldecode out.bin back.txt && diff in.txt back.txt`。无损还原是底线，且不依赖格式是否与他人一致。
- **大文件触发 reset**：用 test8.txt（3MB）这类大输入验证「字典写满重置」逻辑（本期相对 2023T2 唯一新增点）。
- **唯一权威对照**只有 CSE 上的 `~cs9319/a1/test*.lzw`（本期格式），本地这些 `.lzw` 仅用于理解，不作正确性判据。

详细算法导读与学习路径见上一级 [`notes.md`](../notes.md)。
