#include "Types.h"
#include "Keyboard.h"

void kPrintString(int iX, int iY, const char* pcString, BOOL attr);
DWORD strlen(const char* vcString);

// Main 함수
void Main(void)
{
	char vcTemp[2] = {0, };
	BYTE bFlags;
	BYTE bTemp;
	int i = 0;
	const char* vcCKernel64StartMsg = 	"IA-32e C Language Kernel Started...............[    ]";
	const char* vcKeyboardActivate = 	"Keyboard Activate..............................[    ]";
	DWORD cnt = 10;

	kPrintString(0, cnt++, "Switch To IA-32e Mode Success~!!", WHITE);
	kPrintString(0, cnt, vcCKernel64StartMsg, WHITE);
	kPrintString(strlen(vcCKernel64StartMsg)-5, cnt, "Pass", BOLD|GREEN);
	cnt++;

	kPrintString(0, cnt, vcKeyboardActivate, WHITE);
	if (kActivateKeyboard() == TRUE) {
		kPrintString(strlen(vcKeyboardActivate)-5, cnt, "Pass", BOLD|GREEN);
		kChangeKeyboardLED(FALSE, FALSE, FALSE);
	} else {
		kPrintString(strlen(vcKeyboardActivate)-5, cnt, "Fail", BOLD|RED);
		while (1);
	}
	cnt++;

	while (1) {
		// 출력 버퍼(포트 0x60)가 차 있으면 스캔 코드를 읽을 수 있음
		if (kIsOutputBufferFull() == TRUE) {
			// 출력 버퍼(포트 0x60)에서 스캔 코드를 읽어서 저장
			bTemp = kGetKeyboardScanCode();

			// 스캔 코드를 ASCII 코드로 변환하는 함수를 호출하여 ASCII 코드와
			// 눌림 또는 떨어짐 정보를 반환
			if (kConvertScanCodeToASCIICode(bTemp, &(vcTemp[0]), &bFlags) == TRUE) {
				// 키가 눌러졌으면 키의 ASCII 코드 값을 화면에 출력
				if (bFlags & KEY_FLAGS_DOWN) {
					if (bTemp == 28) { // Enter 키 처리
						cnt++;
						i = 0;
					}
					else
						kPrintString(i++, cnt, vcTemp, WHITE);
				}
			}
		}
	}
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
