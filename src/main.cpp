/*
 * 模块名称  : 程序入口
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 程序主入口——初始化系统，启动主菜单循环
 */

#include "ui/menu.h"

/**
 * @brief  程序主入口
 * @note   流程：欢迎界面 → 初始化路网 → 尝试加载历史数据 → 进入主菜单循环
 */
int main() {
    MenuSystem menu;

    menu.show_welcome();

    // TODO: 检测是否存在历史数据文件
    // 若存在 → 提示是否加载
    // 若不存在 → 加载内置示例路网数据

    menu.run();

    return 0;
}
