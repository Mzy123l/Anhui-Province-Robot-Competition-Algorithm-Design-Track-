#include "Tasks.h"


int main()
{
    Tasks::Tasks tasks;
    char choice;
    std::print("测试序号: ");
    std::cin >> choice;
    std::print("输入: ");
    tasks.work(choice);
}
