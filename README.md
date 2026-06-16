---

# AHCPC2026 — 算法设计赛道题解

本仓库包含了 **2026 年安徽省大学生程序设计竞赛（算法设计赛道）** 全部 10 道题目（A–J）的完整解法，使用 **C++23** 编写。代码中应用了自定义的栈上内存池和 STL 兼容分配器，旨在消除动态内存分配的开销，适合竞赛场景下的大规模数据处理。

## 特性

- **C++23 标准**：利用 `std::print`、`std::variant`、`std::visit`、`std::expected`（若使用）等现代特性，提高代码可读性和安全性。
- **自定义内存池**：基于栈上 `std::array` 的 bump allocator，分配 O(1)，零碎片，非常适合一次性分配大量对象的场景。
- **PoolAllocator**：符合 STL 分配器要求的适配器，可直接用于 `std::vector`、`std::unordered_map`、`std::priority_queue` 等容器，所有容器共享同一内存池。
- **高精度计时器**：编译期可选时间单位（纳秒/微秒/毫秒/秒），方便性能调优。
- **单入口分发**：运行时输入题目字母 `A`–`J` 即可执行对应解法，支持管道输入。

## 项目结构

```
.
├── CMakeLists.txt
├── LICENSE
├── Tasks.cpp                # 主入口
├── Tasks.h                  # 分发器（switch 选择题目）
├── include/
│   ├── allocator.hpp        # PoolAllocator 实现
│   ├── memory_pool.hpp      # MemoryPool（bump allocator）
│   ├── timer.hpp            # 计时器模板
│   ├── brake_distance.h     # 题目 A
│   ├── caching_technology.h # 题目 B
│   ├── delivery_plan.h      # 题目 C
│   ├── energy_classification.h # 题目 D
│   ├── fast_charging.h      # 题目 E
│   ├── gear_calibration.h   # 题目 F
│   ├── independent_service.h # 题目 G
│   ├── journey_endurance.h  # 题目 H
│   ├── keep_health.h        # 题目 I
│   └── line_optimization.h  # 题目 J
└── src/
    ├── brake_distance.cpp
    ├── caching_technology.cpp
    ├── delivery_plan.cpp
    ├── energy_classification.cpp
    ├── fast_charging.cpp
    ├── gear_calibration.cpp
    ├── independent_service.cpp
    ├── journey_endurance.cpp
    ├── keep_health.cpp
    └── line_optimization.cpp
```

## 构建与运行

### 环境要求

- CMake ≥ 3.10
- 支持 C++23 的编译器：GCC 14+、Clang 18+、MSVC 2022 17.8+

### 构建

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### 运行

```bash
./Tasks
```

程序会提示输入题目字母（`A`–`J`），然后按原题 PDF 规定的格式输入数据。也可通过管道重定向：

```bash
echo -e "A\n110" | ./Tasks
```

## 题目详解

### A. Brake Distance（间距调整）

**题意**：从 1 出发，允许三种操作：+1、×2、-1（仅当当前值 >1），求到达目标 n 的最少步数，并输出字典序最小的操作序列（A/B/C）。

**算法**：逆向贪心。从 n 逆向推到 1，每一步选择最优操作：
- 若 n 为偶数且 n ≠ 2,3,7，则除以 2（对应正向 B）。
- 若 n % 4 == 1，则 n--（对应正向 A）。
- 否则 n++（对应正向 C）。

最后反转序列得到正向操作串。时间复杂度 O(log n)。

**关键代码**：
```cpp
while (n > 1) 
{
    if (n == 2 || n == 3 || n == 7) { --n; *p++ = 'A'; }
    else if (n % 2 == 0)            { n /= 2; *p++ = 'B'; }
    else if (n % 4 == 1)            { --n; *p++ = 'A'; }
    else                            { ++n; *p++ = 'C'; }
}
std::reverse(result, p);
```

### B. Caching Technology（缓存技术）

**题意**：模拟容量为 C 的全相联 LRU 缓存，支持 READ 和 WRITE 操作，采用写回策略。输入为一行包含分号分隔的操作串，可能含有空白字符。输出每条操作的日志和最终缓存状态。

**算法**：
- 使用 `std::list` 维护 MRU→LRU 顺序，`std::unordered_map` 实现 O(1) 查找。
- 缓存满时淘汰 LRU 端，若脏则写回主存。
- 写缺失：先加载再写入并置脏。
- 输入解析：手动逐字符扫描，识别 `READ`/`WRITE` 和数字。

**复杂度**：每个操作 O(1) 均摊，总复杂度 O(|S|)。

**注意**：主存初始值等于 key，只有淘汰脏数据时才更新主存。

### C. Delivery Plan（平滑信号）

**题意**：给定序列 A 和参数 k，可修改任意元素的值，要求对于所有窗口长度 w∈[1,k]，滑动均值滤波后的序列均为常数。求最少修改次数。

**算法**：转化为找出现次数最多的元素（众数）。可以证明：序列平滑当且仅当所有元素相等。因此答案 = n - max_freq。
- 小数据（n < 65535）：排序后统计频次。
- 大数据：使用 `unordered_map` 哈希统计。

**复杂度**：O(n) 或 O(n log n)。

### D. Energy Classification（能源划分）

**题意**：将长度为 n 的序列划分为恰好 m 个连续段，每段风险 = 段长 × (段内最大值 - 段内最小值)，求所有段风险最大值的最小值。

**算法**：二分答案 + 贪心分段。二分最大风险值 limit，判断能否用 ≤ m 段覆盖整个序列。贪心扩展每段直到超过 limit 为止。

**复杂度**：O(n log(range))，其中 range ≈ n × (max - min)。

### E. Fast Charging（智能调度）

**题意**：三个数 A,B,C，每次操作可任选两个各 +1，或任选一个 +2，求使三者相等的最小操作次数。

**算法**：数学构造。设排序后 a≤b≤c。最终值必为 c 或 c+1。推导得：
- 若 (2c - a - b) 为偶数，ans = (2c - a - b) / 2；
- 否则 ans = (2c - a - b + 3) / 2。

**复杂度**：O(1)。

### F. Gear Calibration（出厂校准）

**题意**：m 条产线，每条有四种元素产量和成本，每种元素需达到最低目标，求最小总成本。每条产线最多用一次。

**算法**：
- 小数据（m < 50）：暴力枚举子集（DFS + 剪枝）。
- 大数据（m ≤ 100）：IDA* + 状态压缩哈希。使用启发式函数 estimate_min_cost 预估剩余最小成本，结合哈希表记录已访问状态的最小成本进行剪枝。

**复杂度**：指数级，但 m ≤ 100 且元素值 ≤ 30，实际可接受。

### G. Independent Service（服务划分）

**题意**：树上有节点权值 d[i] 和边权，支持三种操作：修改节点权值、修改边权、查询给定一组服务中心的服务总量（每个节点归属最近服务中心，距离相同取编号小的）。

**算法**：对每次类型 3 查询，以所有服务中心为源点运行多源 Dijkstra（边权小，可用堆优化），同时记录每个节点所属的服务中心 ID，最后累加 d[i]。修改操作直接更新数组或邻接表。

**复杂度**：每次查询 O((n+m) log n)，总查询中心数 ≤ 3×10^5。

### H. Journey Endurance（电池续航）

**题意**：有向图（含自环和重边），Q 次询问，每次问从 s 出发恰好走 K 步到每个终点的最小“路径上最大边权”（Minimax）。

**算法**：倍增 Floyd 变体。定义 dp[t][i][j] 为从 i 走 2^t 步到 j 的最小最大边权。转移：dp[t][i][j] = min over k of max(dp[t-1][i][k], dp[t-1][k][j])。对于每个询问，二进制分解 K，依次合并向量。

**复杂度**：预处理 O(N³ log K)，单次询问 O(N² log K)。N ≤ 100，K ≤ 10^9，Q ≤ 1000。

### I. Keep Health（电池健康）

**题意**：模拟电池仓库，支持买进、取用（优先最少循环次数，其次最早入库）、归还（循环次数+1，若 ≥ K 则报废）、查询库存。

**算法**：使用 `std::priority_queue` 维护仓库，自定义比较器（循环次数小优先，时间戳小优先）。归还时若循环次数达到 K 则输出名字且不入队，否则重新入队。取用时弹出堆顶并记录到外部映射。

**复杂度**：每个操作 O(log Q)，Q ≤ 2×10^5。

### J. Line Optimization（生产优化）

**题意**：DAG，每条边有原耗时 w 和优化后耗时 c，可选择最多 K 条边使用优化后的耗时，求从 1 到 n 的最短路径最小值。

**算法**：动态规划。按拓扑序（节点编号递增）转移：dp[v][k] = min(dp[u][k] + w, dp[u][k-1] + c)。

**复杂度**：O((n+m)K)。n ≤ 2×10^5，m ≤ 5×10^5，K ≤ 100。

## 自定义基础设施详解

### MemoryPool（`memory_pool.hpp`）

- 在栈上分配一个 `std::array<char, BlockSize>` 作为原始存储。
- 维护一个 `_current_pos` 指针，每次分配时进行 **bump**（只前进，不回收）。
- 自动对齐到 `alignof(T)`，保证类型安全。
- 分配失败返回 `nullptr`，各题目中直接 `abort()` 终止。

优点：分配速度极快（仅指针加法），无内存碎片，适合一次性分配大量对象的场景。

### PoolAllocator（`allocator.hpp`）

- 符合 C++ 命名要求 `Allocator`，可作为 `std::allocator` 的替换。
- 内部持有 `MemoryPool*`，所有 `allocate` / `deallocate` 委托给它。
- 支持 `rebind`，使得同一内存池可用于不同类型（如 `pair<const K,V>`）。
- `propagate_on_container_copy/move/swap` 均设为 `true`，保证容器拷贝时分配器正确传播。
- `is_always_equal` 设为 `true`，所有从同一池派生的分配器视为相等。

使用方式示例：
```cpp
memory_pool::MemoryPool<1000000> pool;
allocator::PoolAllocator<int, 1000000> alloc(pool);
std::vector<int, decltype(alloc)> vec(alloc);
```

### Timer（`timer.hpp`）

- 基于 `std::chrono::high_resolution_clock`。
- 编译期通过 `if constexpr` 选择时间单位，无运行时开销。
- 提供 `start()`、`stop()`、`show_time()` 方法。

## 注意事项

- **栈空间**：`CMakeLists.txt` 中将可执行文件的栈大小设为 **500 MB**，以适应大型内存池（尤其是题目 H）。如果你的环境限制了栈大小，请自行调整。
- **内存池不回收单个分配**：整个池在 `MemoryPool` 对象析构时一并释放。这在单次运行的竞赛场景中是合理的。
- **输入解析**：题目 B 的输入包含分号和空格，使用手动逐字符扫描而非 `cin >>` 分词。
- **编译选项**：推荐使用 `-O2` 或 `-O3` 优化级别。

## 许可证

本项目采用 MIT 许可证。详情请参阅 ./LICENSE 文件。pp`，理解整体结构。
2. 选择一个未完成任务，按照上述步骤创建 `.h` 和 `.cpp`。
3. 在 `Tasks.h` 中注册，编译运行测试。
4. 使用 `timer` 测量性能，确保满足题目时限。

---

