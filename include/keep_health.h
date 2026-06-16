#pragma once
#include "allocator.hpp"
#include <queue>
#include <unordered_map>
#include <string>
#include <variant>
#include <string_view>

namespace Tasks
{
	// 电池健康 / Keep health

	/*
	* 【题目描述】
	某地有一座动力电池充换电站，负责管理电动汽车动力电池的入库、取用、归还与报废。
	电池的健康状态由充放电循环次数衡量，简称 SOH。
	请你编写程序，模拟该换电站的电池管理系统。其中，换电站支持四种操作：
	1. 买进电池：购买一颗新电池并存入仓库。电池拥有一个唯一的名字，由大写字母和数字组成，
	长度不超过 8 个字符，初始充放电循环次数为 0。
    2. 取用电池：从仓库中取出一颗电池供车辆使用。
	选取规则为：优先取充放电循环次数最少的电池；若有多颗并列，则取入库时间最早的电池。
	操作完成后输出被取出的电池的名字。
	3. 归还电池：某颗给定名字的电池被归还到仓库进行充电。
	充电会使该电池的充放电循环次数 +1。充电完成后，若该电池的充放电循环次数达到 K（即 ≥ K），
	则该电池报废，不再放回仓库，并输出该电池的名字作为报废提示；
	否则，该电池重新入库，不输出任何内容。
	4. 查询库存：统计当前仓库中有多少颗电池，输出该数量。
	最初仓库是空的。保证所有操作均合法：取用电池时仓库非空，归还的电池是当前已被取出的电池，买进的电池名字未被使用过，
	且所有电池名字均为长度不超过 8 的ASCII 字符串（仅含大小写字母和数字）。

	【输入格式】
	从标准输入读入数据。
	输入的第一行包含两个正整数 Q, K，分别表示操作次数和电池报废的循环次数阈值。
	接下来 Q 行，每行描述一个操作，格式如下：
	• 1 <name>：买进一颗名字为 <name> 的新电池。
	• 2：取用一颗电池。
	• 3 <name>：归还名字为 <name> 的电池。
	• 4：查询仓库中的电池数量。

	【输出格式】
	输出到标准输出。
	对于操作 2（取用电池），输出被取出的电池的名字，占一行。
	对于操作 3（归还电池），若电池报废，输出该电池的名字，占一行；否则不输出。
	对于操作 4（查询库存），输出当前仓库中的电池数量，占一行。

	【数据范围与约定】
	对于 30% 的数据，Q ≤ 1000，K ≤ 10。
	对于 100% 的数据，Q ≤ 2 × 10^5，K ≤ 10^5。
	*/

	/*
	* 使用小根堆
	*/
	class KeepHealth
	{
	public:
		KeepHealth(uint32_t Q, uint32_t K) : _pool(), _alloc(_pool), Q(Q), K(K),
			_outer(Alloc<std::pair<const _string, uint32_t>>(_alloc)), _warehouse(Alloc<_battery>(_alloc)),
			_src(Alloc<std::pair<uint16_t, _string>>(_alloc)), _res(Alloc <std::variant <_string_view, uint32_t>>(_alloc)){}
		void input();
		void solve();
		void show_result() const;

	private:
		static constexpr std::size_t _block_size = 500000000;
		template <typename T>
		using Alloc = allocator::PoolAllocator <T, _block_size>;

		using _string = std::string; // SSO优化
		using _string_view = std::string_view;

		void parse_stringline(char*);
		void _buy(const _string&);
		void _use();
		void _return(const _string&);
		void _check();
	private:
		
		memory_pool::MemoryPool <_block_size> _pool;
		allocator::PoolAllocator <char, _block_size> _alloc;

		
		struct _battery
		{
			_battery(const _string& name, uint32_t count = 0);
			_string_view _name;
			uint32_t _charge_count;
			uint32_t _time_stamp;

			struct Compare
			{
				bool operator()(const _battery& a, const _battery& b) const;
			};
		};
		static uint32_t TimeStamp;
		uint32_t Q;
		uint32_t K;

		std::unordered_map <_string_view, uint32_t, std::hash<_string_view>, std::equal_to<_string_view>, 
			Alloc<std::pair<const _string_view, uint32_t>>> _outer; // 外部
		std::priority_queue <_battery, std::vector<_battery, Alloc<_battery>>, _battery::Compare> _warehouse; // 内部
		// 1-买进电池 2-取用电池 3-归还电池 4-查询库存  |  name
		std::vector <std::pair<uint16_t, _string>, Alloc<std::pair<uint16_t, _string>>> _src; // 输入
		// name | 库存
		std::vector <std::variant <_string_view, uint32_t>, Alloc <std::variant <_string_view, uint32_t>>> _res; // 输出

	};

}