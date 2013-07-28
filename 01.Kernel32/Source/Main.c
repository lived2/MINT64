#include "Types.h"

void kPrintString(int iX, int iY, const char* pcString, BOOL attr);
DWORD strlen(const char* vcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

// Main 함수
void Main(void)
{
	DWORD i;
	const char* vcCKernelStartMsg = 	"C Language Kernel Started......................[    ]";
	const char* vcMinMemMsg =       	"Minimum Memory Size Check......................[    ]";
	const char* vcInit64KernelArea =	"IA-32e Kernel Area Initialize..................[    ]";

	kPrintString(0, 3, vcCKernelStartMsg, WHITE);
	kPrintString(strlen(vcCKernelStartMsg)-5, 3, "Pass", BOLD|GREEN);

	// 최소 메모리 크기를 만족하는지 검사
	kPrintString(0, 4, vcMinMemMsg, WHITE);
	if (kIsMemoryEnough() == FALSE) {
		kPrintString(strlen(vcMinMemMsg)-5, 4, "Fail", BOLD|RED);
		kPrintString(0, 5, "Not Enough Memory~!! MINT64 OS Requires Over "
				"64Mbyte Memory~!!", BOLD|RED);
		while (1);
	} else {
		kPrintString(strlen(vcMinMemMsg)-5, 4, "Pass", BOLD|GREEN);
	}

	// IA-32e 모드의 커널 영역을 초기화
	kPrintString(0, 5, vcInit64KernelArea, WHITE);
	if (kInitializeKernel64Area() == FALSE) {
		kPrintString(strlen(vcInit64KernelArea)-5, 5, "Fail", BOLD|RED);
		kPrintString(0, 6, "Kernel Area Initialization Fail~!!", BOLD|RED);
		while (1);
	}
	kPrintString(strlen(vcInit64KernelArea)-5, 5, "Pass", BOLD|GREEN);

	while (1);
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

// IA-32e 모드용 커널 영역을 0으로 초기화
BOOL kInitializeKernel64Area(void)
{
	DWORD* pdwCurrentAddress;

	// 초기화를 시작할 어드레스인 0x100000(1MB)을 설정
	pdwCurrentAddress = (DWORD*)0x100000;

	// 마지막 어드레스인 0x600000(6MB)까지 루프를 돌면서 4바이트씩 0으로 채움
	while ((DWORD)pdwCurrentAddress < 0x600000) {
		*pdwCurrentAddress = 0x00;

		// 0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를
		// 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
		if (*pdwCurrentAddress != 0) {
			return FALSE;
		}

		// 다음 어드레스로 이동
		pdwCurrentAddress++;
	}

	return TRUE;
}

BOOL kIsMemoryEnough(void)
{
	DWORD* pdwCurrentAddress;

	// 0x100000(1MB)부터 검사 시작
	pdwCurrentAddress = (DWORD*)0x100000;

	// 0x4000000(64MB)까지 루프를 돌면서 확인
	while ((DWORD)pdwCurrentAddress < 0x4000000) {
		*pdwCurrentAddress = 0x12345678;

		// 0x12345678로 저장한 후 다시 읽었을 때 0x12345678이 나오지 않으면
		// 해당 어드레스를 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
		if (*pdwCurrentAddress != 0x12345678) {
			return FALSE;
		}

		// 1MB씩 이동하면서 확인
		pdwCurrentAddress += (0x100000/4);
	}
	return TRUE;
}
