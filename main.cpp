/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
// TODO:
// 1：中文，英文语言切换支持（包括界面以及提示信息）
// 2: 部分布局的修改
// 3：注释的修改补全
// 4：规整部分宏和静态常亮
//-----------------------------------------------------
#include "FKBrowserApp.h"
//-----------------------------------------------------
// 入口
int main(int argc, char **argv)
{
    // 进行资源预加载
    // Q_INIT_RESOURCE(data);
    // 创建App对象
    CFKBrowserApp app(argc, argv);
    if (!app.IsTheOnlyBrowser())\
    {
        return 0;
    }
    // 创建一个空窗口
    app.CreateMainWindow();
    // 消息递交
    return app.exec();
}
//-----------------------------------------------------
