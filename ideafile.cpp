#include "ideafile.h"

bool ListSaveFile(LISTCUSTIMER listtimer,const char filename[],const char mode[])
{
    FILE* fp;
    fopen_s(&fp, filename, mode);
    if (fp == NULL)
        return 0;
    int len = listtimer.size();
    fwrite((char*)&len, sizeof(int), 1, fp);
    LISTCUSTIMER::iterator i;
    for (i = listtimer.begin(); i != listtimer.end(); ++i)
    {
        fwrite((char*)&*i, sizeof(CUSTIMER), 1, fp);
    }
    fclose(fp);
    return 1;
}

bool ListLoadFile(LISTCUSTIMER &listtimer,const char filename[],const char mode[])
{
    FILE* fp;
    fopen_s(&fp, filename, mode);
    if (fp == NULL)
        return 0;
    int temp;
    fread((char*)&temp, sizeof(int), 1, fp);
    for (int i = 0; i < temp; i++)
    {
        CUSTIMER temp;
        fread((char*)&temp, sizeof(CUSTIMER), 1, fp);
        listtimer.push_back(temp);
    }
    fclose(fp);
    return 1;
}

