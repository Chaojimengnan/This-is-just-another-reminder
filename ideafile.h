#pragma once
#include <list>
#include "CustomizeTime.h"

/// <summary>
/// 将列表内容保存到文件中
/// </summary>
/// <param name="listtimer">列表</param>
/// <param name="filename">文件名</param>
/// <param name="mode">打开方式</param>
/// <returns>是否成功</returns>
bool ListSaveFile(LISTCUSTIMER listtimer,const char filename[],const char mode[]);


/// <summary>
/// 从文件中加载到列表
/// </summary>
/// <param name="listtimer">列表</param>
/// <param name="filename">文件名</param>
/// <param name="mode">打开方式</param>
/// <returns>是否成功</returns>
bool ListLoadFile(LISTCUSTIMER &listtimer,const char filename[],const char mode[]);

