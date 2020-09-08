#pragma once
#include <list>
#include "CustomizeTime.h"

/// <summary>
/// ���б����ݱ��浽�ļ���
/// </summary>
/// <param name="listtimer">�б�</param>
/// <param name="filename">�ļ���</param>
/// <param name="mode">�򿪷�ʽ</param>
/// <returns>�Ƿ�ɹ�</returns>
bool ListSaveFile(LISTCUSTIMER listtimer,const char filename[],const char mode[]);


/// <summary>
/// ���ļ��м��ص��б�
/// </summary>
/// <param name="listtimer">�б�</param>
/// <param name="filename">�ļ���</param>
/// <param name="mode">�򿪷�ʽ</param>
/// <returns>�Ƿ�ɹ�</returns>
bool ListLoadFile(LISTCUSTIMER &listtimer,const char filename[],const char mode[]);

