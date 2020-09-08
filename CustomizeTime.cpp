#include "CustomizeTime.h"

/// <summary>
/// 自定义时间转换为秒
/// </summary>
/// <param name="custime">自定义时间</param>
/// <returns>对应的时间</returns>
int CusTimeToSec(CUSTIME custime)
{
	return custime.hou * 3600 + custime.min * 60 + custime.sec;
}




/// <summary>
/// 自定义时间加上sec秒，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="sec">秒</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddSec(CUSTIME custime, int sec)
{
	custime.sec += sec;
	custime.min += custime.sec / 60;
	custime.hou += custime.min / 60;

	custime.sec = custime.sec % 60;
	custime.min = custime.min % 60;
	custime.hou = custime.hou % 60;

	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;
	// 当hour超出24或小于-1的时候报错

	return custime;
}


/// <summary>
/// 自定义时间加上min分，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="min">分</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddMin(CUSTIME custime, int min)
{
	custime.min += min;
	custime.hou += custime.min / 60;

	custime.min = custime.min % 60;
	custime.hou = custime.hou % 60;

	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;
	// 当hour超出24或小于-1的时候报错

	return custime;
}

/// <summary>
/// 自定义时间加上hou小时，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="hou">小时</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddHou(CUSTIME custime, int hou)
{
	custime.hou += hou;
	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;

	return custime;
}

/// <summary>
/// 自定义时间加上另一个自定义时间，自动增加进位
/// </summary>
/// <param name="custime">自定义时间</param>
/// <param name="custime2">自定义时间2</param>
/// <returns>新的自定义时间</returns>
CUSTIME CusTimerAddCus(CUSTIME custime, CUSTIME custime2)
{
	custime.sec += custime2.sec;
	custime.min += custime2.min;
	custime.hou += custime2.hou;

	custime.min += custime.sec / 60;
	custime.hou += custime.min / 60;

	custime.sec = custime.sec % 60;
	custime.min = custime.min % 60;
	custime.hou = custime.hou % 60;

	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;
	// 当hour超出24或小于-1的时候报错

	return custime;
}