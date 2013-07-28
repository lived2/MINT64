#ifndef __TYPES_H__
#define __TYPES_H__

#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned int
#define QWORD	unsigned long
#define BOOL	unsigned char

#define TRUE	1
#define FALSE	0
#define NULL	0

#pragma pack( push, 1 )

#define BLACK	0x00
#define BLUE	0x01
#define GREEN	0x02
#define CYAN	0x03
#define RED		0x04
#define MAGENTA	0x05
#define YELLOW	0x06
#define WHITE	0x07
#define BOLD	0x08

// 비디오 모드 중 텍스트 모드 화면을 구성하는 자료구조
typedef struct kCharactorStruct
{
	BYTE bCharactor;
	BYTE bAttribute;
} CHARACTER;

#pragma pack( pop )
#endif /*__TYPES_H__*/
