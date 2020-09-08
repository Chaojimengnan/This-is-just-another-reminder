#include "CustomizeTime.h"

/// <summary>
/// �Զ���ʱ��ת��Ϊ��
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <returns>��Ӧ��ʱ��</returns>
int CusTimeToSec(CUSTIME custime)
{
	return custime.hou * 3600 + custime.min * 60 + custime.sec;
}




/// <summary>
/// �Զ���ʱ�����sec�룬�Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="sec">��</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddSec(CUSTIME custime, int sec)
{
	custime.sec += sec;
	custime.min += custime.sec / 60;
	custime.hou += custime.min / 60;

	custime.sec = custime.sec % 60;
	custime.min = custime.min % 60;
	custime.hou = custime.hou % 60;

	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;
	// ��hour����24��С��-1��ʱ�򱨴�

	return custime;
}


/// <summary>
/// �Զ���ʱ�����min�֣��Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="min">��</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddMin(CUSTIME custime, int min)
{
	custime.min += min;
	custime.hou += custime.min / 60;

	custime.min = custime.min % 60;
	custime.hou = custime.hou % 60;

	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;
	// ��hour����24��С��-1��ʱ�򱨴�

	return custime;
}

/// <summary>
/// �Զ���ʱ�����houСʱ���Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="hou">Сʱ</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddHou(CUSTIME custime, int hou)
{
	custime.hou += hou;
	if (custime.hou >= 24 or custime.hou < 0) throw HOUROVERFLOW;

	return custime;
}

/// <summary>
/// �Զ���ʱ�������һ���Զ���ʱ�䣬�Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="custime2">�Զ���ʱ��2</param>
/// <returns>�µ��Զ���ʱ��</returns>
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
	// ��hour����24��С��-1��ʱ�򱨴�

	return custime;
}