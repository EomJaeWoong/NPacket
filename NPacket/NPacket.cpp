// NPacket.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CNPacket::_ValueSizeCheck();

	CNPacket nPacket;

	nPacket << 3;
	nPacket << 2.5f;
	nPacket << L"EomJaeWoong";

	int a;
	float b;
	WCHAR pString[30];

	nPacket >> a;
	nPacket >> b;
	nPacket >> pString;

	printf("%d %f", a, b);
	wprintf(L"%s", pString);
	return 0;
}

