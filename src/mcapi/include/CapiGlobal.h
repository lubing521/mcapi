/*
 * Copyright (c) 2006-2008
 * Author: Weiming Zhou
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  
 */

/* 
 *	CapiGlobal.h
 *
 *	DESCRIPTION
 *		global Prototypes and definition for capi library.
 *
 *	HISTORY
 *		10-03-2003	create by zhouweiming.
 *
 */
#ifndef __CAPIGLOBAL_H__
#define __CAPIGLOBAL_H__

#if _WIN32_WINNT < 0x0404
#undef _WIN32_WINNT
#define _WIN32_WINNT  0x0404
#endif

#ifdef _WIN32 
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <memory.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _TEST 
#define PRIVATE public
#else
#define PRIVATE private
#endif

#define CAPI_SUCCESS        0
#define CAPI_FAILED         1
#define CAPI_NO_MEMORY      2
#define CAPI_NOT_FOUND      3


typedef int             BOOL;
typedef long int        LONG;
typedef unsigned long   DWORD;
typedef unsigned int    UINT;
typedef int             INT;
typedef void *          HANDLE;

#ifndef _WIN32 
#define _stdcall
#define __stdcall
#define WINAPI
typedef long long int LONGLONG;
typedef long long int int64_t;
#define Sleep(x)      usleep(x)

#define  TRUE         1
#define  FALSE        0

#endif


/* 单向链表的节点结构 */
typedef struct SINGLENODE_st {
	struct SINGLENODE_st *pNext;     /* 下一个节点指针 */
    void	*pData;                  /* 数据指针 */
} SINGLENODE, *PSINGLENODE;

typedef struct DOUBLENODE_st {
    struct DOUBLENODE_st *pNext;     /* 下一个节点指针 */
    struct DOUBLENODE_st *pPrev;     /* 前一个节点指针 */
    void    *pData;                  /* 数据指针 */
} DOUBLENODE;


extern const int g_ncore;

/**	通用类型数据比较函数

	@param	void *pData1 -要比较的第1个参数	
	@param	void *pData2 -要比较的第2个参数	
        @return	INT - 小于0表示pData1小于pData2, 等于0表示pData1等于pData2,
                  大于0表示pData1大于pData2
*/
typedef INT (*COMPAREFUNC) ( void *pData1, void *pData2 );

/**	通用类型数据释放函数

	@param	void *pData -要释放的数据	
	@return	 -无	
*/
typedef void (*DESTROYFUNC) ( void *pData );

/**	通用类型数据的遍历执行函数

	@param	void *pData - 要操作的数据指针	
	@return	void - 无	
*/
typedef void (*TRAVERSEFUNC)( void *pData );


/**	通用类型数据的遍历执行函数

	@param	void *pData - 要操作的数据指针	
	@return	void - 无	
*/
typedef INT (*VISITFUNC)(void *pData);

/**	通用数据拷贝函数

	@param	void *pData - 要拷贝的数据	
	@return	void * - 成功返回拷贝的数据，失败返回NULL	
*/
typedef void *(*COPYFUNC)(void *pData);

/**	基数排序的获取关键字转换成序号的函数

	@param	void *pData - 关键字指针	
    @param	UINT *uKeyIndex - 关键字的位数	
	@return	UINT - 关键字转换后的序号	
*/
typedef UINT (*GETKEYFUNC)( void *pData, UINT uKeyIndex );

/**	计算哈希值的回调函数

        @param  void *pKey - 要计算哈希值的关键词	
        @param  UINT uBucketNum - 哈希表中的BUCKET数组大小	
	@return UINT - 返回计算出的哈希值，一般用作哈希表BUCKET数组下标。	
*/
typedef UINT (*HASHFUNC)(void *pKey, UINT uBucketNum);


#define CAPI_EXIT_TASK      1
#define CAPI_NOT_EXIT_TASK  0


/* memory allocation functions */
void *CapiMalloc(size_t size);
void *CapiRealloc(void *p, size_t size);
void CapiFree(void *p);


/*** external functions ***/
UINT HashInt( void *pKey, UINT uBucketCount );

UINT HashString( void *pStr, UINT uBucketCount );
UINT HashBin( void *pData, UINT uLength, UINT uBucketCount );


UINT GetStrKeyNoCase( void *pszData, UINT uKeyIndex );
UINT GetStrKey( void *pszData, UINT uKeyIndex );
UINT GetIntKey( void *pData, UINT uKeyIndex );
UINT GetWordKey( UINT uData, UINT uKeyIndex );

#include "Atomic.h"
#include "CapiCommon.h"
#include "mcapi_threads.h"

#ifdef __cplusplus
}
#endif

#endif /* __CAPIGLOBAL_H__ */
