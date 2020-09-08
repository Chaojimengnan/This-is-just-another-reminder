#pragma once
#include "TCHAR.h"
#include <vector>
using namespace std;

#define MAXSTRINGNAME 14
#define MAXSTRING 403
#define HOUROVERFLOW 10086	// 小时溢出



/// <summary>
/// 自定义时间结构体
/// </summary>
typedef struct {
	short hou;	// 时
	short min;	// 分
	short sec;	// 秒
}CUSTIME, * PTRCUSTIME;

/// <summary>
/// 自定义计时器
/// </summary>
typedef struct {

	CUSTIME aimtime;				// 目标时间
	TCHAR name[MAXSTRINGNAME];		// 计时器名字
	TCHAR remindstring[MAXSTRING];	// 提醒字符串
	bool isoneweek;					// 是否只用一周,OK之前写错了，现在表示周循环了

	bool isday;						// 是否开启周循环,FALSE，下面的参数无效，TRUE，下面用来设置定时区间

	CUSTIME uppertime;				// 一天时间的上限
	CUSTIME lowertime;				// 一天时间的下限

	bool islong;					// 是否长时间，如果TRUE，isonce默认为TRUE,isoneweek默认为FALSE

	int day;						// 几号
	int month;						// 几月
	int year;						// 几年
}CUSTIMER, * PTRCUSTIMER;

typedef vector<CUSTIMER> LISTCUSTIMER;

///// <summary>
///// 自定义计时器
///// </summary>
//typedef struct {
//	CUSTIME uppertime;				// 一天时间的上限
//	CUSTIME lowertime;				// 一天时间的下限
//	CUSTIME intervaltime;			// 间隔时间
//	TCHAR name[MAXSTRING];			// 计时器名字
//	TCHAR remindstring[MAXSTRING];	// 提醒字符串
//	unsigned int index;				// 用于索引
//	bool isoneweek;					// 是否只用一周
//}CUSTIMEER, * PTRCUSTIMER;


///// <summary>
///// 自定义一天的提醒事项
///// </summary>
//typedef struct {
//	CUSTIME AimTime;				// 瞄准的时间
//	TCHAR Name[MAXSTRING];			// 计时器名字
//	TCHAR RemindString[MAXSTRING];	// 提醒字符串
//	unsigned int index;				// 用于索引
//	bool IsOneWeek;					// 是否只用一周
//}CUSREMINDER, * PTRCUSREMINDER;


/// <summary>
/// 自定义长时间的提醒事项
/// </summary>
typedef struct {
	int day;						// 几号
	int month;						// 几月
	int offday;						// 过去的时间
	CUSTIME AimTime;				// 瞄准的时间
	TCHAR Name[MAXSTRING];			// 计时器名字
	TCHAR RemindString[MAXSTRING];	// 提醒字符串	
}CUSLREMINDER, * PTRCUSLREMINDER;


/// <summary>
/// 自定义时间转换为秒
/// </summary>
/// <param name="custime">自定义时间</param>
/// <returns>对应的时间</returns>
int CusTimeToSec(CUSTIME custime);


/// <summary>
/// 自定义时间加上sec秒，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="sec">秒</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddSec(CUSTIME custime, int sec);


/// <summary>
/// 自定义时间加上min分，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="min">分</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddMin(CUSTIME custime, int min);

/// <summary>
/// 自定义时间加上hou小时，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="hou">小时</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddHou(CUSTIME custime, int hou);

/// <summary>
/// 自定义时间加上另一个自定义时间，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="custime2">自定义时间2</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddCus(CUSTIME custime, CUSTIME custime2);