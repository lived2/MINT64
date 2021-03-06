#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char* pcString, BOOL attr);
DWORD strlen(const char* vcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);
void kCopyKernel64ImageTo2Mbyte(void);

// Main 함수
void Main(void)
{
	//DWORD i;
	const char* vcCKernelStartMsg = 	"Protected Mode C Language Kernel Started.......[    ]";
	const char* vcMinMemMsg =       	"Minimum Memory Size Check......................[    ]";
	const char* vcSupportIA32eMsg =		"IA-32e Mode Support Check......................[    ]";
	const char* vcInit64KernelArea =	"IA-32e Kernel Area Initialize..................[    ]";
	const char* vcPageTableInitMsg =	"IA-32e Page Tables Initialize..................[    ]";
	const char* vcCPUVendorIs =			"Processor Vendor String........................[";
	const char* vcCopyIA32eKernel =		"Copy IA-32e Kernel To 2Mbyte Address...........[    ]";
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char* vcVendorString[13] = {0, };
	DWORD cnt = 3;

	kPrintString(0, cnt, vcCKernelStartMsg, WHITE);
	kPrintString(strlen(vcCKernelStartMsg)-5, cnt, "Pass", BOLD|GREEN);
	cnt++;

	// 최소 메모리 크기를 만족하는지 검사
	kPrintString(0, cnt, vcMinMemMsg, WHITE);
	if (kIsMemoryEnough() == FALSE) {
		kPrintString(strlen(vcMinMemMsg)-5, cnt+1, "Fail", BOLD|RED);
		kPrintString(0, cnt+1, "Not Enough Memory~!! MINT64 OS Requires Over "
				"64Mbyte Memory~!!", BOLD|RED);
		while (1);
	} else {
		kPrintString(strlen(vcMinMemMsg)-5, cnt, "Pass", BOLD|GREEN);
	}
	cnt++;

	// 프로세서 제조사 정보 읽기
	kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	*(DWORD*)vcVendorString = dwEBX;
	*((DWORD*)vcVendorString+1) = dwEDX;
	*((DWORD*)vcVendorString+2) = dwECX;
	kPrintString(0, cnt, vcCPUVendorIs, WHITE);
	kPrintString(strlen(vcCPUVendorIs), cnt, (const char*)vcVendorString, BOLD|RED);
	kPrintString(strlen(vcCPUVendorIs) + strlen((const char*)vcVendorString), cnt, "]", WHITE);
	cnt++;

	// CPU의 IA-32 모드 지원 여부 체크
	kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	kPrintString(0, cnt, vcSupportIA32eMsg, WHITE);
	if (dwEDX & (1 << 29)) {
		kPrintString(strlen(vcSupportIA32eMsg)-5, cnt, "Pass", BOLD|GREEN);
	} else {
		kPrintString(strlen(vcSupportIA32eMsg)-5, cnt, "Fail", BOLD|RED);
		kPrintString(0, cnt+1, "CPU must support IA-32e(64bit) mode ", BOLD|RED);
		while (1);
	}
	cnt++;

	// IA-32e 모드의 커널 영역을 초기화
	kPrintString(0, cnt, vcInit64KernelArea, WHITE);
	if (kInitializeKernel64Area() == FALSE) {
		kPrintString(strlen(vcInit64KernelArea)-5, cnt+1, "Fail", BOLD|RED);
		kPrintString(0, cnt+1, "Kernel Area Initialization Fail~!!", BOLD|RED);
		while (1);
	}
	kPrintString(strlen(vcInit64KernelArea)-5, cnt, "Pass", BOLD|GREEN);
	cnt++;

	// IA-32e 모드 커널을 위한 페이지 테이블 생성
	kPrintString(0, cnt, vcPageTableInitMsg, WHITE);
	kInitializePageTables();
	kPrintString(strlen(vcPageTableInitMsg)-5, cnt, "Pass", BOLD|GREEN);
	cnt++;

	// IA-32e 모드 커널을 0x200000(2Mbyte) 어드레스로 이동
	kPrintString(0, cnt, vcCopyIA32eKernel, WHITE);
	kCopyKernel64ImageTo2Mbyte();
	kPrintString(strlen(vcCopyIA32eKernel)-5, cnt, "Pass", BOLD|GREEN);
	cnt++;

	// IA-32e 모드로 전환
	kPrintString(0, cnt, "Switch To IA-32e Mode", WHITE);
	// 원래는 아래 함수를 호출해야 하나 IA-32e 모드 커널이 없으므로 주석 처리
	kSwitchAndExecute64bitKernel();

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

// IA-32e 모드 커널을 0x200000(2Mbyte) 어드레스에 복사
void kCopyKernel64ImageTo2Mbyte(void)
{
	WORD wKernel32SectorCount, wTotalKernelSectorCount;
	DWORD* pdwSourceAddress, * pdwDestinationAddress;
	int i, size;

	// 0x7C05에 총 커널 섹터 수, 0x7C07에 보호 모드 커널 섹터 수가 들어 있음
	wTotalKernelSectorCount = *((WORD*)0x7C05);
	wKernel32SectorCount = *((WORD*)0x7C07);

	pdwSourceAddress = (DWORD*)(0x10000 + (wKernel32SectorCount * 512));
	pdwDestinationAddress = (DWORD*)0x200000;
	// IA-32e 모드 커널 섹터 크기만큼 복사
	size = 512 * (wTotalKernelSectorCount - wKernel32SectorCount) / 4;
	for (i = 0; i < size ; i++) {
		*pdwDestinationAddress = *pdwSourceAddress;
		pdwDestinationAddress++;
		pdwSourceAddress++;
	}
}
