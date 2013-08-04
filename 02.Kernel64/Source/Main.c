#include "Types.h"

void kPrintString(int iX, int iY, const char* pcString, BOOL attr);
DWORD strlen(const char* vcString);

// Main 함수
void Main(void)
{
	const char* vcCKernel64StartMsg = 	"IA-32e C Language Kernel Started...............[    ]";
	DWORD cnt = 10;

	kPrintString(0, cnt++, "Switch To IA-32e Mode Success~!!", WHITE);
	kPrintString(0, cnt, vcCKernel64StartMsg, WHITE);
	kPrintString(strlen(vcCKernel64StartMsg)-5, cnt, "Pass", BOLD|GREEN);
	cnt++;
}

void kPrintString(int iX, int iY, const char* pcString, BOOL attr)
{
	CHARACTER* pstScreen = (CHARACTER*)0xB8000;
	int i;

	pstScreen += (iY*80) + iX;
	for (i = 0; pcString[i] != 0; i++) {
		pstScreen[i].bCharactor = pcString[i];
		pstScreen[i].bAttribute = attr;
	}
}

DWORD strlen(const char* vcString)
{
	DWORD dwLen = 0;
	while (vcString[dwLen] != 0)
		dwLen++;
	return dwLen;
}
