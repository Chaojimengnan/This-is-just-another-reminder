#pragma once
#include "TCHAR.h"
#include <vector>
using namespace std;

#define MAXSTRINGNAME 14
#define MAXSTRING 403
#define HOUROVERFLOW 10086	// Сʱ���



/// <summary>
/// �Զ���ʱ��ṹ��
/// </summary>
typedef struct {
	short hou;	// ʱ
	short min;	// ��
	short sec;	// ��
}CUSTIME, * PTRCUSTIME;

/// <summary>
/// �Զ����ʱ��
/// </summary>
typedef struct {

	CUSTIME aimtime;				// Ŀ��ʱ��
	TCHAR name[MAXSTRINGNAME];		// ��ʱ������
	TCHAR remindstring[MAXSTRING];	// �����ַ���
	bool isoneweek;					// �Ƿ�ֻ��һ��,OK֮ǰд���ˣ����ڱ�ʾ��ѭ����

	bool isday;						// �Ƿ�����ѭ��,FALSE������Ĳ�����Ч��TRUE�������������ö�ʱ����

	CUSTIME uppertime;				// һ��ʱ�������
	CUSTIME lowertime;				// һ��ʱ�������

	bool islong;					// �Ƿ�ʱ�䣬���TRUE��isonceĬ��ΪTRUE,isoneweekĬ��ΪFALSE

	int day;						// ����
	int month;						// ����
	int year;						// ����
}CUSTIMER, * PTRCUSTIMER;

typedef vector<CUSTIMER> LISTCUSTIMER;

///// <summary>
///// �Զ����ʱ��
///// </summary>
//typedef struct {
//	CUSTIME uppertime;				// һ��ʱ�������
//	CUSTIME lowertime;				// һ��ʱ�������
//	CUSTIME intervaltime;			// ���ʱ��
//	TCHAR name[MAXSTRING];			// ��ʱ������
//	TCHAR remindstring[MAXSTRING];	// �����ַ���
//	unsigned int index;				// ��������
//	bool isoneweek;					// �Ƿ�ֻ��һ��
//}CUSTIMEER, * PTRCUSTIMER;


///// <summary>
///// �Զ���һ�����������
///// </summary>
//typedef struct {
//	CUSTIME AimTime;				// ��׼��ʱ��
//	TCHAR Name[MAXSTRING];			// ��ʱ������
//	TCHAR RemindString[MAXSTRING];	// �����ַ���
//	unsigned int index;				// ��������
//	bool IsOneWeek;					// �Ƿ�ֻ��һ��
//}CUSREMINDER, * PTRCUSREMINDER;


/// <summary>
/// �Զ��峤ʱ�����������
/// </summary>
typedef struct {
	int day;						// ����
	int month;						// ����
	int offday;						// ��ȥ��ʱ��
	CUSTIME AimTime;				// ��׼��ʱ��
	TCHAR Name[MAXSTRING];			// ��ʱ������
	TCHAR RemindString[MAXSTRING];	// �����ַ���	
}CUSLREMINDER, * PTRCUSLREMINDER;


/// <summary>
/// �Զ���ʱ��ת��Ϊ��
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <returns>��Ӧ��ʱ��</returns>
int CusTimeToSec(CUSTIME custime);


/// <summary>
/// �Զ���ʱ�����sec�룬�Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="sec">��</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddSec(CUSTIME custime, int sec);


/// <summary>
/// �Զ���ʱ�����min�֣��Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="min">��</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddMin(CUSTIME custime, int min);

/// <summary>
/// �Զ���ʱ�����houСʱ���Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="hou">Сʱ</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddHou(CUSTIME custime, int hou);

/// <summary>
/// �Զ���ʱ�������һ���Զ���ʱ�䣬�Զ����ӽ�λ
/// </summary>
/// <param name="custime">�Զ���ʱ��</param>
/// <param name="custime2">�Զ���ʱ��2</param>
/// <returns>�µ��Զ���ʱ��</returns>
CUSTIME CusTimerAddCus(CUSTIME custime, CUSTIME custime2);