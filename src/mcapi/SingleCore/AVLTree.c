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

#include <stdlib.h>
#include "CapiGlobal.h"
#include "BinTree.h"
#include "AVLTree.h"


/**	AVL树的创建函数

	@param	void -无	
	@return	AVLTREE * -成功返回创建的AVL树指针，失败返回NULL。	
*/
AVLTREE *AVLTree_Create(void)
{
    AVLTREE *pTree;

    pTree = (AVLTREE *)malloc(sizeof(AVLTREE));
    if ( pTree != NULL )
    {
        pTree->pRoot = NULL;
        pTree->uNodeCount = 0;
        pTree->pCursor = NULL;
    }
    return pTree;
}

/**	AVL树的释放函数，将以某个指定节点为根节点的AVL树及树中的全部节点都释放掉
    采用后序遍历方式进行释放

	@param	AVLTREENODE *pTreeNode - 要释放的根节点	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void AVLTreeNode_Destroy(AVLTREENODE *pTreeNode, DESTROYFUNC DestroyFunc)
{
    if ( pTreeNode != NULL )
    {
        if ( pTreeNode->pLeft != NULL )
        {
            AVLTreeNode_Destroy(pTreeNode->pLeft, DestroyFunc);
        }
        if ( pTreeNode->pRight != NULL )
        {
            AVLTreeNode_Destroy(pTreeNode->pRight, DestroyFunc);
        }
        if ( DestroyFunc != NULL && pTreeNode->pData != NULL )
        {
            (*DestroyFunc)(pTreeNode->pData);
        }
        free(pTreeNode);
    }
}

/**	AVL树的释放函数，将一颗AVL树释放掉

	@param	AVLTREE *pTree - 要释放的AVL树指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void AVLTree_Destroy(AVLTREE *pTree, DESTROYFUNC DestroyFunc)
{
    if ( pTree == NULL )
    {
        return;
    }
    AVLTreeNode_Destroy( pTree->pRoot, DestroyFunc );
    free( pTree );
}


/**	AVL树的查找函数，调用了二叉搜索树的查找函数进行查找

	@param	AVLTREE *pTree - 要查找的AVL树指针	
	@param	void *pData - 要查找的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数
	@return	void * - 成功返回查找到的目标数据，失败返回NULL	
*/
void *AVLTree_Find(AVLTREE *pTree, void *pData, COMPAREFUNC CompareFunc)
{
    return BinTree_Find(pTree->pRoot, pData, CompareFunc);
}





/**	AVL树的获取左右子树负载因子之和的函数
    获取操作节点的左子树和右子树的负载因子并相加

    @param	AVLTREENODE *pStartNode - 要操作的节点	
    @return	UINT - 左子树和右子树负载因子之和	
*/
UINT AVLTree_GetSubTreeCount(AVLTREENODE *pStartNode)
{
    UINT    uLeftCount;
    UINT    uRightCount;
    AVLTREENODE *pNode = pStartNode;

    if ( pNode->pLeft)
    {
        uLeftCount = pNode->pLeft->uCount;
    }
    else
    {
        uLeftCount = 0;
    }
    if ( pNode->pRight)
    {
        uRightCount = pNode->pRight->uCount;
    }
    else
    {
        uRightCount = 0;
    }

    return (uLeftCount + uRightCount);
}

/**	AVL树的左旋函数

	@param	AVLTREE *pTree - AVL树指针	
	@param	AVLTREENODE *pStartNode - 要进行左旋操作的节点指针	
	@return	void - 无	
*/
void AVLTree_RotateLeft( AVLTREE *pTree, AVLTREENODE *pStartNode )
{
    BinTree_RotateLeft((BINTREEBASENODE *)pStartNode, &(pTree->pRoot));
}

/**	AVL树的右旋函数

	@param	AVLTREE *pTree - AVL树指针	
	@param	AVLTREENODE *pStartNode - 要进行右旋操作的节点指针	
	@return	void - 无	
*/
void AVLTree_RotateRight(AVLTREE *pTree, AVLTREENODE *pStartNode )
{
    BinTree_RotateRight((BINTREEBASENODE *)pStartNode, &(pTree->pRoot));
}

/**	AVL树的双旋转操作函数，先左旋再右旋

	@param	AVLTREENODE **ppRoot - AVL树的根节点指针	
	@param	AVLTREENODE *pStartNode - 要旋转的节点	
	@return	void - 无
*/
void AVLTree_RotateLeftRight(AVLTREENODE **ppRoot, AVLTREENODE *pStartNode)
{
    AVLTREENODE *pANode;
    AVLTREENODE *pBNode;
    AVLTREENODE *pCNode;

    pANode = pStartNode;
    pBNode = pANode->pLeft;
    pCNode = pBNode->pRight;

    BinTree_RotateLeft(pBNode, ppRoot);
    BinTree_RotateRight(pANode, ppRoot);

    switch ( pCNode->nMagic )
    {
    case 1:
        /* 插入在C节点左子树的情况 */
        pANode->nMagic = -1;
        pBNode->nMagic = 0;
        break;
    case -1:
        /* 插入在C节点右子树的情况 */
        pANode->nMagic = 0;
        pBNode->nMagic = 1;
        break;
    default:
        /* C节点就是插入点的情况 */
        pANode->nMagic = 0;
        pBNode->nMagic = 0;
        break;        
    }
    pCNode->nMagic = 0;
}

/**	AVL树的双旋转函数

	@param	AVLTREENODE **ppRoot - AVL树根节点指针
	@param	AVLTREENODE *pStartNode - 要旋转的节点指针	
	@return	void - 无
*/void AVLTree_RotateRightLeft(AVLTREENODE **ppRoot, AVLTREENODE *pStartNode)
{
    AVLTREENODE *pANode;
    AVLTREENODE *pBNode;
    AVLTREENODE *pCNode;

    pANode = pStartNode;
    pBNode = pANode->pRight;
    pCNode = pBNode->pLeft;

    BinTree_RotateRight(pBNode, ppRoot);
    BinTree_RotateLeft(pANode, ppRoot);

    switch ( pCNode->nMagic )
    {
    case 1:
        /* 插入在C节点左子树的情况 */
        pANode->nMagic = 0;
        pBNode->nMagic = -1;
        break;
    case -1:
        /* 插入在C节点右子树的情况 */
        pANode->nMagic = 1;
        pBNode->nMagic = 0;
        break;
    default:
        /* C节点就是插入点的情况 */
        pANode->nMagic = 0;
        pBNode->nMagic = 0;
        break;        
    }
    pCNode->nMagic = 0;
}

/**	AVL树的调整平衡因子函数，主要是调整插入操作时的平衡

    @param	AVLTREENODE *pStartNode - 要调整平衡的起始节点	
    @param	void *pData - 插入的数据指针	
    @param	COMPAREFUNC CompareFunc - 数据比较回调函数	
    @return	void - 无	
*/
static void AVLTree_FixBalance(AVLTREENODE *pStartNode, 
                               void *pData, COMPAREFUNC CompareFunc)
{
    AVLTREENODE *pNode;
    AVLTREENODE *pSearchNode;
    INT nResult; 

    pNode = pStartNode;
    while ( pNode != NULL )
    {
        pSearchNode = pNode;

        nResult = (*CompareFunc)(pNode->pData, pData);
        if ( nResult < 0 )
        {
            pNode = pNode->pRight;
            pSearchNode->nMagic -= 1;
        }
        else if ( nResult > 0 )
        {
            pNode = pNode->pLeft;
            pSearchNode->nMagic += 1;
        }
        else
        {
            /* 找到相同关键词的节点, 中止 */
            break;
        }
    }
    return;
}

/**	AVL树插入操作中的调整负载因子函数
    它会将根节点到插入节点路径上所有节点（不包括插入节点）的
    负载因子加1

    @param	AVLTREENODE *pInsertNode - 插入节点	
    @return	void - 无	
*/
static void AVLTree_FixCountForInsert(AVLTREENODE *pInsertNode)
{
    AVLTREENODE *pNode = pInsertNode;

    pNode = pNode->pParent;
    while ( pNode != NULL )
    {
        pNode->uCount += 1;
        pNode = pNode->pParent;
    }
}


/**	AVL树的插入操作函数

	@param	AVLTREE *pTree - AVL树	
	@param	void *pData - 要插入的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	INT - CAPI_SUCCESS表示成功，失败返回CAPI_FAILED	
*/
INT AVLTree_Insert(AVLTREE *pTree, void *pData, COMPAREFUNC CompareFunc)
{
    INT nRet;
    if ( pTree == NULL || pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    nRet = AVLTreeNode_Insert(&(pTree->pRoot), pData, CompareFunc); 
    if ( nRet != CAPI_FAILED )
    {
        pTree->uNodeCount += 1;
    }
    return nRet;
}

/**	AVL树的插入数据函数，将一个数据插入到AVL树中

	@param	AVLTREENODE **ppRootNode - 要插入的AVL树根节点指针
	@param	void *pData - 要插入的数据	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	INT - CAPI_SUCCESS表示插入成功，CAPI_FAILED表示失败，失败的原因有
                  内存分配失败和已存在相同关键词数据两种情况
*/
INT AVLTreeNode_Insert(AVLTREENODE **ppRootNode, void *pData, 
                       COMPAREFUNC CompareFunc)
{
    AVLTREENODE *pNode;
    AVLTREENODE *pANode;
    AVLTREENODE *pNewNode;
    AVLTREENODE *pSearchNode;
    AVLTREENODE *pBNode;
    AVLTREENODE *pCNode;
    INT         nRet;

    nRet = 0;
    pANode = NULL;
    pSearchNode = NULL;

    /* 查找要插入的节点位置，并且在查找过程中要记录最后一个不平衡的节点 */
    pNode = *ppRootNode;
    while ( pNode != NULL )
    {
        pSearchNode = pNode;
        if ( pSearchNode->nMagic == -1 || pSearchNode->nMagic == 1 )
        {
            pANode = pSearchNode;
        }
        nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            /* 找到相同关键词的节点，插入失败 */
            return CAPI_FAILED;
        }
    }

    pNewNode = (AVLTREENODE *)malloc( sizeof(AVLTREENODE) );
    if ( pNewNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNewNode->pData = pData;
    pNewNode->pLeft = NULL;
    pNewNode->pRight = NULL;
    pNewNode->pParent = pSearchNode;
    pNewNode->nMagic = 0;
    pNewNode->uCount = 1;

    if ( pSearchNode == NULL )
    {
        *ppRootNode = pNewNode;
        return CAPI_SUCCESS;
    }

    if ( nRet < 0 )
    {
        pSearchNode->pRight = pNewNode;
    }
    else
    {
        pSearchNode->pLeft = pNewNode;
    }

    /* 修改新插入节点到根节点路径上的所有节点的元素个数 */
    AVLTree_FixCountForInsert(pNewNode);

    /* 不存在不平衡的节点，直接插入后再修改平衡因子即可 */
    if ( pANode == NULL )
    {
        /* 修改从根节点到插入节点的平衡因子 */
        AVLTree_FixBalance(*ppRootNode, pData, CompareFunc);
       
        return CAPI_SUCCESS;
    }
    
    nRet = (*CompareFunc)(pANode->pData, pData);

    /* 以下处理存在不平衡节点的情况 */
    if ( ( pANode->nMagic == 1 && nRet < 0 )
        || (pANode->nMagic == -1 && nRet > 0 ) )
    {
        /* A节点平衡因子为1且插入节点插入在右子树的情况
         * 以及A节点平衡因子为-1且插入在左子树的情况
         * 这两种情况插入后还是平衡树，只需修改相关节点平衡因子
         */
        AVLTree_FixBalance(pANode, pData, CompareFunc);
    }
    else if ( pANode->nMagic == 1 && nRet > 0 )
    {
        if ( (*CompareFunc)(pANode->pLeft->pData, pData) > 0 )
        {
            /* LL型不平衡, 插入在A节点的左子树的左子树中 */
            pBNode = pANode->pLeft;
            BinTree_RotateRight(pANode, ppRootNode);
            AVLTree_FixBalance(pBNode, pData, CompareFunc);
            pANode->nMagic = 0;
            pBNode->nMagic = 0;

            pBNode->uCount = pANode->uCount;
            pANode->uCount = AVLTree_GetSubTreeCount(pANode) + 1;
        }
        else
        {
            INT nRetVal;

            /* LR型不平衡, 插入在A节点的左子树的右子树中 */
            pBNode = pANode->pLeft;
            pCNode = pBNode->pRight;
            nRetVal = (*CompareFunc)(pCNode->pData, pData) ;
            if ( nRetVal > 0 )
            { 
                /* 新节点插在 C 节点的左子树中的情况 */
                pCNode->nMagic += 1;
            }
            else if ( nRetVal < 0 )
            {
                /* 新节点插在 C 节点的右子树中的情况 */
                pCNode->nMagic -= 1;
            }

            /* 双旋转操作，A、B、C 节点的 nMagic 值已经填充正确 */
            AVLTree_RotateLeftRight(ppRootNode, pANode);

            /* Fixed by fanxueqiang on 2011-11-28 */
            /* NOTE: AVLTree_FixBalance() 会影响根节点， 而根节点已经在
             * AVLTree_RotateRightLeft() 函数中填写正确，不能再修改 */
            if ( nRetVal > 0 )
            {
                /* 新节点插在 C 节点的左子树中的情况 */
                /* AVLTree_FixBalance() 的操作只对在插入后会使整棵树的高度 + 1
                 * 的情况有效（如在一棵满二叉树插入），其他情况都可能有错误。
                 * 而在这里的情况，不一定使以 B 节点为整棵树时的高度 + 1，
                 * 只能确保 B 节点的右子树的高度 + 1，
                 * 所以从 B 节点的右子树开始 */
                /*AVLTree_FixBalance(pBNode, pData, CompareFunc);*/
                AVLTree_FixBalance(pBNode->pRight, pData, CompareFunc);
            }
            else if ( nRetVal < 0 )
            {
                /* 新节点插在 C 节点的右子树中的情况 */
                /*AVLTree_FixBalance(pANode, pData, CompareFunc);*/
                AVLTree_FixBalance(pANode->pLeft, pData, CompareFunc);
            }
            else
            {
            }

            pCNode->uCount = pANode->uCount;
            pBNode->uCount = AVLTree_GetSubTreeCount(pBNode) + 1;
            pANode->uCount = AVLTree_GetSubTreeCount(pANode) + 1;
        }
    }
    else /* pANode->nMagic == -1 && nRet < 0 的情况*/
    {
        if ( (*CompareFunc)(pANode->pRight->pData, pData) > 0 )
        {
            INT nRetVal;

            /* RL型不平衡, 插入在A节点的右子树的左子树中 */
            pBNode = pANode->pRight;
            pCNode = pBNode->pLeft;
            nRetVal = (*CompareFunc)(pCNode->pData, pData);
            if ( nRetVal > 0 )
            { 
                pCNode->nMagic += 1;
            }
            else if ( nRetVal < 0 )
            {
                pCNode->nMagic -= 1;
            }

            AVLTree_RotateRightLeft(ppRootNode, pANode);

            if ( nRetVal > 0 )
            {
                /*AVLTree_FixBalance(pANode, pData, CompareFunc);*/
                AVLTree_FixBalance(pANode->pRight, pData, CompareFunc);
            }
            else if ( nRetVal < 0 )
            {
                /*AVLTree_FixBalance(pBNode, pData, CompareFunc);*/
                AVLTree_FixBalance(pBNode->pLeft, pData, CompareFunc);
            }
            else
            {
            }

            pCNode->uCount = pANode->uCount;
            pBNode->uCount = AVLTree_GetSubTreeCount(pBNode) + 1;
            pANode->uCount = AVLTree_GetSubTreeCount(pANode) + 1;
        }
        else
        {
            /* RR型不平衡, 插入在A节点的右子树的右子树中 */
            pBNode = pANode->pRight;
            BinTree_RotateLeft(pANode, ppRootNode);
            AVLTree_FixBalance(pBNode, pData, CompareFunc);
            pANode->nMagic = 0;
            pBNode->nMagic = 0;
            pBNode->uCount = pANode->uCount;
            pANode->uCount = AVLTree_GetSubTreeCount(pANode) + 1;

        }
    }
    return CAPI_SUCCESS;
}

/**	AVL树删除操作中的调整负载因子函数
    它会将根节点到A节点路径上所有节点（包括A节点）的
    负载因子减1

    @param	AVLTREENODE *pANode - 删除操作中的A节点	
    @return	void - 无	
*/
static void AVLTree_FixCountForDelete(AVLTREENODE *pANode)
{
    AVLTREENODE *pNode = pANode;

    while ( pNode != NULL )
    {
        pNode->uCount -= 1;
        pNode = pNode->pParent;
    }
}

/**	AVL树的删除操作调整平衡函数，由删除函数调用

	@param	AVLTREENODE **ppRoot - 指向AVL树的根节点指针的指针	
	@param	AVLTREENODE *pNode - 要调整平衡的节点	
	@param	void *pData - 删除的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	void - 无	
*/
void AVLTree_AdjustBalanceForDelete(AVLTREENODE **ppRoot, AVLTREENODE *pNode, 
                                    void *pData, COMPAREFUNC CompareFunc )
{
    AVLTREENODE     *pANode;
    AVLTREENODE     *pParentNode;
    AVLTREENODE     *pBNode;
    AVLTREENODE     *pCNode;

    pANode = pNode;
    while ( pANode != NULL )
    {
        switch( pANode->nMagic )
        {
        case 0:
            if ( pANode == *ppRoot )
            {
                /* pANode为根节点，高度减少了1，但左右子树高度相等，无需调整 */
                break;
            }
            else 
            {
                pParentNode = pANode->pParent;
                if ( pANode == pParentNode->pLeft )
                {
                    pParentNode->nMagic -= 1;
                }
                else
                {
                    pParentNode->nMagic += 1;
                }
                /* 将pANode 指向它的父节点，继续调整它的父节点的不平衡情况 */
                pANode = pParentNode;
                continue;
            }
        case -1: 
        case 1:
            /* pANode原来的平衡因子为0，删除操作发生后，高度未改变，因此不需要
             * 再调整，退出即可 
             */
            break;
        case -2: /* L型不平衡情况 */
            pBNode = pANode->pRight;
            if ( pBNode->nMagic == 0 )
            {
                /* L0型不平衡情况 */
                BinTree_RotateLeft(pANode, ppRoot);
                pANode->nMagic = -1;
                pBNode->nMagic = 1;

                pBNode->uCount = pANode->uCount;
                pANode->uCount = AVLTree_GetSubTreeCount(pANode)+1;
                break;
            }
            else if ( pBNode->nMagic == -1 )
            {
                /* L-1型不平衡情况 */
                pParentNode = pANode->pParent;
                if ( pParentNode != NULL )
                {
                    if ( pANode == pParentNode->pLeft )
                    {
                        pParentNode->nMagic -= 1;
                    }
                    else
                    {
                        pParentNode->nMagic += 1;
                    }
                }
                
                BinTree_RotateLeft(pANode, ppRoot);
                pANode->nMagic = 0;
                pBNode->nMagic = 0;
                pBNode->uCount = pANode->uCount;
                pANode->uCount = AVLTree_GetSubTreeCount(pANode)+1;

                /* 将pANode 指向它的父节点，继续调整它的父节点的不平衡情况 */
                pANode = pParentNode;
            }
            else /* pBNode->nMagic == 1的情况 */
            {
                /* L1型的情况 */
                pParentNode = pANode->pParent;
                if ( pParentNode != NULL )
                {
                    if ( pANode == pParentNode->pLeft )
                    {
                        pParentNode->nMagic -= 1;
                    }
                    else
                    {
                        pParentNode->nMagic += 1;
                    }
                }
                pBNode = pANode->pRight;
                pCNode = pBNode->pLeft;

                AVLTree_RotateRightLeft(ppRoot, pANode);
                
                pCNode->uCount = pANode->uCount;
                pANode->uCount = AVLTree_GetSubTreeCount(pANode)+1;
                pBNode->uCount = AVLTree_GetSubTreeCount(pBNode)+1;

                /* 将pANode 指向它的父节点，继续调整它的父节点的不平衡情况 */
                pANode = pParentNode;

            }
            continue; /* 继续while() 循环 */
        case 2: /* R型不平衡情况 */
            pBNode = pANode->pLeft;
            if ( pBNode->nMagic == 0 )
            {
                /* R0型不平衡情况 */
                BinTree_RotateRight(pANode, ppRoot);
                pANode->nMagic = 1;
                pBNode->nMagic = -1;
                pBNode->uCount = pANode->uCount;
                pANode->uCount = AVLTree_GetSubTreeCount(pANode)+1;
                break;
            }
            else if ( pBNode->nMagic == -1 )
            {
                /* R-1型不平衡情况 */
                pParentNode = pANode->pParent;
                if ( pParentNode != NULL )
                {
                    if ( pANode == pParentNode->pLeft )
                    {
                        pParentNode->nMagic -= 1;
                    }
                    else
                    {
                        pParentNode->nMagic += 1;
                    }
                }
                pBNode = pANode->pLeft;
                pCNode = pBNode->pRight;
                AVLTree_RotateLeftRight(ppRoot, pANode);
                pCNode->uCount = pANode->uCount;
                pANode->uCount = AVLTree_GetSubTreeCount(pANode)+1;
                pBNode->uCount = AVLTree_GetSubTreeCount(pBNode)+1;
                /* 将pANode 指向它的父节点，继续调整它的父节点的不平衡情况 */
                pANode = pParentNode;
            }
            else /* pBNode->nMagic == 1的情况 */
            {
                /* R1型的情况 */
                pParentNode = pANode->pParent;
                if ( pParentNode != NULL )
                {
                    if ( pANode == pParentNode->pLeft )
                    {
                        pParentNode->nMagic -= 1;
                    }
                    else
                    {
                        pParentNode->nMagic += 1;
                    }
                }
                BinTree_RotateRight(pANode, ppRoot);
                pANode->nMagic = 0;
                pBNode->nMagic = 0;
                pBNode->uCount = pANode->uCount;
                pANode->uCount = AVLTree_GetSubTreeCount(pANode)+1;
                /* 将pANode 指向它的父节点，继续调整它的父节点的不平衡情况 */
                pANode = pParentNode;
            }
            continue; /* 继续while() 循环 */
        default:
            break;
        } /* switch ( pANode->nMagic ) */
        break;
    }
}



/**	AVL树的删除操作函数

	@param	AVLTREE *pTree - AVL树指针	
	@param	void *pData - 要删除的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功	
*/
INT AVLTree_Delete(AVLTREE *pTree, void *pData, COMPAREFUNC CompareFunc,
                   DESTROYFUNC DestroyFunc)
{
    AVLTREENODE *pCursor;
    void *pPopData;

    if ( pTree->pRoot == NULL || pData == NULL 
        || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    
    pCursor = pTree->pCursor;

    pPopData = AVLTreeNode_Pop(&(pTree->pRoot), &pCursor, pData, CompareFunc);
    if ( pPopData != NULL )
    {
        pTree->pCursor = pCursor;

        if ( pCursor != NULL )
        {
            if ( (*CompareFunc)(pCursor->pData, pData) < 0 )
            {
                AVLTree_EnumNext(pTree);
            }
        }

        if ( DestroyFunc != NULL )
        {
            (*DestroyFunc)(pPopData);
        }

        pTree->uNodeCount -= 1;

        return CAPI_SUCCESS;
    }
    else
    {
        return CAPI_NOT_FOUND;
    }
}


/**	AVL树的删除节点函数

	@param	AVLTREENODE **ppRoot - 指向AVL树根节点指针的指针	
	@param	void *pData - 要删除的数据	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功		
*/
INT AVLTreeNode_Delete(AVLTREENODE **ppRoot, AVLTREENODE **ppCursor, void *pData, 
                       COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc)
{

    AVLTREENODE  *pNode;
    AVLTREENODE  *pANode;
    AVLTREENODE  *pDelNode;
    void         *pDelData;
    INT         nRet = 0;

    pNode = *ppRoot;

    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            break;
        }
    }

    if ( pNode == NULL )
    {
        return CAPI_NOT_FOUND;
    }

    pDelData = pNode->pData;
    if ( pNode->pLeft != NULL && pNode->pRight != NULL )
    {
        /* 处理查找到的pNode有两个子节点的情况 */
        pDelNode = pNode->pLeft;   
        while (pDelNode->pRight != 0)
        {
            pDelNode = pDelNode->pRight;
        }
        pANode = pDelNode->pParent;  

        pNode->pData = pDelNode->pData;
        
		if ( pDelNode == pANode->pLeft )
        {
            pANode->nMagic -= 1;
        }
        else
        {
            pANode->nMagic += 1;
        }
        
		if (pDelNode != pNode->pLeft) 
        {
            pANode->pRight = pDelNode->pLeft;
        }
        else
        {
            pANode->pLeft = pDelNode->pLeft;
        }

        if ( pDelNode->pLeft != NULL )
        {
            pDelNode->pLeft->pParent = pANode;
        }
    }
    else
    {
        pANode = pNode;
        /* 处理最多只有一个子节点的情况 */
        if ( pNode->pLeft != NULL )
        {
            /* 只有左节点的情况 */
            pDelNode = pNode->pLeft;
            pNode->pData = pDelNode->pData;
            pNode->pLeft = NULL;
            pANode->nMagic -= 1;
        }
        else if ( pNode->pRight != NULL )
        {
            /* 只有右节点的情况 */
            pDelNode = pNode->pRight;
            pNode->pData = pDelNode->pData;
            pNode->pRight = NULL;
            pANode->nMagic += 1;
        }
        else
        {
            /* 处理删除节点的左右子节点都不存在的情况 */
            pANode = pNode->pParent;
            pDelNode = pNode;
            if ( pANode == NULL )
            {
                *ppRoot = pANode;
            }
            else if ( pANode->pLeft == pNode )
            {
                pANode->pLeft = NULL;
                pANode->nMagic -= 1;
            }
            else
            {
                pANode->pRight = NULL;
                pANode->nMagic += 1;
            }
        }
    }
   
    /* 删除对应节点 */
    if ( pDelNode != NULL )
    {
        if ( pDelNode == *ppCursor )
        {
            AVLTREENODE *pNextNode = AVLTreeNode_GetNext(pDelNode);

            *ppCursor = pNextNode;
        }
        if ( DestroyFunc != NULL )
        {
            (*DestroyFunc)(pDelData);
        }
        free(pDelNode);
    }

    AVLTree_FixCountForDelete(pANode);

    /* 调整平衡 */
    if ( pANode != NULL )
    {
        AVLTree_AdjustBalanceForDelete(ppRoot, pANode, pData, CompareFunc);
    }
    
    return CAPI_SUCCESS;
}



/**	AVL树的删除节点函数

	@param	AVLTREENODE **ppRoot - 指向AVL树根节点指针的指针	
	@param	void *pData - 要删除的数据	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	void * - NULL表示失败，非空表示弹出的对应节点数据		
*/
void * AVLTreeNode_Pop(AVLTREENODE **ppRoot, AVLTREENODE **ppCursor, void *pData, 
                       COMPAREFUNC CompareFunc)
{

    AVLTREENODE  *pNode;
    AVLTREENODE  *pANode;
    AVLTREENODE  *pDelNode;
    void         *pDelData;
    INT         nRet = 0;

    pNode = *ppRoot;

    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            break;
        }
    }

    if ( pNode == NULL )
    {
        return NULL;
    }

    pDelData = pNode->pData;
    if ( pNode->pLeft != NULL && pNode->pRight != NULL )
    {
        /* 处理查找到的pNode有两个子节点的情况 */
        pDelNode = pNode->pLeft;   
        while (pDelNode->pRight != 0)
        {
            pDelNode = pDelNode->pRight;
        }
        pANode = pDelNode->pParent;  

        pNode->pData = pDelNode->pData;
        
		if ( pDelNode == pANode->pLeft )
        {
            pANode->nMagic -= 1;
        }
        else
        {
            pANode->nMagic += 1;
        }
        
		if (pDelNode != pNode->pLeft) 
        {
            pANode->pRight = pDelNode->pLeft;
        }
        else
        {
            pANode->pLeft = pDelNode->pLeft;
        }

        if ( pDelNode->pLeft != NULL )
        {
            pDelNode->pLeft->pParent = pANode;
        }
    }
    else
    {
        pANode = pNode;
        /* 处理最多只有一个子节点的情况 */
        if ( pNode->pLeft != NULL )
        {
            /* 只有左节点的情况 */
            pDelNode = pNode->pLeft;
            pNode->pData = pDelNode->pData;
            pNode->pLeft = NULL;
            pANode->nMagic -= 1;
        }
        else if ( pNode->pRight != NULL )
        {
            /* 只有右节点的情况 */
            pDelNode = pNode->pRight;
            pNode->pData = pDelNode->pData;
            pNode->pRight = NULL;
            pANode->nMagic += 1;
        }
        else
        {
            /* 处理删除节点的左右子节点都不存在的情况 */
            pANode = pNode->pParent;
            pDelNode = pNode;
            if ( pANode == NULL )
            {
                *ppRoot = pANode;
            }
            else if ( pANode->pLeft == pNode )
            {
                pANode->pLeft = NULL;
                pANode->nMagic -= 1;
            }
            else
            {
                pANode->pRight = NULL;
                pANode->nMagic += 1;
            }
        }
    }
   
    /* 删除对应节点 */
    if ( pDelNode != NULL )
    {
        if ( pDelNode == *ppCursor )
        {
            AVLTREENODE *pNextNode = AVLTreeNode_GetNext(pDelNode);

            *ppCursor = pNextNode;
        }
        free(pDelNode);
    }

    AVLTree_FixCountForDelete(pANode);

    /* 调整平衡 */
    if ( pANode != NULL )
    {
        AVLTree_AdjustBalanceForDelete(ppRoot, pANode, pData, CompareFunc);
    }
    
    return pDelData;
}




static int AVLTree_CheckMagic_Rec(AVLTREENODE *pNode, VISITFUNC VisitFunc)
{
    int nLeftDeep, nRightDeep;
    int nDepth = 0;

    if (pNode == NULL)
    {
        return 0;
    }

    nLeftDeep = AVLTree_CheckMagic_Rec(pNode->pLeft, VisitFunc);
    nRightDeep = AVLTree_CheckMagic_Rec(pNode->pRight, VisitFunc);

    if ( nLeftDeep == -1 || nRightDeep == -1 )
    {
        return -1;
    }

    if (nLeftDeep > nRightDeep)
    {
        nDepth = nLeftDeep + 1;
    }
    else
    {
        nDepth = nRightDeep + 1;
    }

    if ( pNode->nMagic == nLeftDeep - nRightDeep )
    {
        return nDepth;
    }
    else
    {
        if ( VisitFunc != NULL )
        {
            (*VisitFunc)(pNode->pData);
        }
        return -1;
    }

    return nDepth;
}

int AVLTree_CheckMagic(AVLTREE *pTree, VISITFUNC VisitFunc)
{
    int nRet = AVLTree_CheckMagic_Rec(pTree->pRoot, VisitFunc);
    return nRet != -1;
}


#if 0
AVLTree_Delete()
{
    查找待删除节点;
    //根据待删除节点的左右子节点是否存在情况来确定A节点;
    if ( 待删除节点的两个子节点均不存在 )
    {
        A节点 = 待删除节点的父节点;
        实际删除的节点 = 待删除节点;
        调整A节点的平衡因子;
    }
    else if ( 待删除节点的两个子节点均存在)
    {
        查找待删除节点左子树中的最大节点
        if ( 待删除节点左子树中最大节点 == 待删除节点的左节点 )
        {
            将待删除节点从树中移出;
            A节点 ＝ 左节点;
            实际删除节点 = 待删除节点;
        }
        else
        {
            将左子树中最大节点的数据和待删除节点的数据进行交换;
            A节点 = 待删除节点左子树中最大节点的父节点;
            实际删除节点 = 左子树最大节点;
        }
        调整A节点的平衡因子;
    }
    else // 待删除节点只有一个子节点存在的情况
    {
        使用存在的子节点替换待删除节点;
        A节点 = 取代待删除节点的子节点;
        实际删除节点 = 待删除节点;
    }

    释放实际删除节点的内存;

    while ( A节点 != NULL )
    {
        switch ( A节点的平衡因子 )
        {
        case -1:
        case 1:
            // A节点原来的平衡因子为0，删除操作发生后，高度未改变，因此不需要
            // 再调整，退出即可 
            break;
        case 0:
            if ( A节点 == 根节点 ) 
            {
                // pANode为根节点，高度减少了1，但左右子树高度相等，无需调整
                break;
            }
            else 
            {
                修改A节点的父节点的平衡因子;
                A节点 ＝ A节点的父节点;
                continue;  //继续while循环
            }
        case -2: // L型不平衡情况 
            B节点 = A节点的右节点;
            if ( B节点平衡因子 == 0 )
            {
                // L0型不平衡情况 
                对A节点进行左旋操作;
                A节点平衡因子 = -1;
                B节点平衡因子 = 1;
                break;
            }
            else if ( B节点平衡因子 == -1 )
            {
                // L-1型不平衡情况 
                调整A节点的父节点的平衡因子;
                对A节点进行左旋操作;
                A节点平衡因子 = 0;
                B节点平衡因子 = 0;

                A节点 ＝ A节点的父节点; //继续while循环
            }
            else /* B节点平衡因子 == 1的情况 */
            {
                /* L1型的情况 */
                调整A节点的父节点的平衡因子;
                对B节点进行右旋;
                对A节点进行左旋;

                A节点 ＝ A节点的父节点; //继续while循环
            }
            continue; /* 继续while() 循环 */
        case 2: // R型不平衡情况
            //和L型不平衡情况是对称的，此处略去...
            continue;
        default:
            break;
        } // switch
    } //while
}
#endif

/**	AVL树的枚举开始函数

	@param	AVLTREE *pTree - AVL树指针	
	@return	void - 无	
*/
void AVLTree_EnumBegin(AVLTREE *pTree)
{
    BINTREEBASENODE *pCursor;

    pCursor = pTree->pRoot;
    if ( pCursor != NULL )
    {
        while ( pCursor->pLeft != NULL )
        {
            pCursor = pCursor->pLeft;
        }
    }
    pTree->pCursor = pCursor;
}


/**	AVL树的枚举下一个元素函数
    按照中序遍历顺序依次获取下一个元素

	@param	AVLTREE *pTree - AVL树指针	
	@return	void * - 返回获取的下一个元素，如果下一个元素不存在则返回空	
*/
void * AVLTree_EnumNext(AVLTREE *pTree)
{
    void *pData;
    AVLTREENODE *pCursor;

    if ( pTree->pCursor == NULL )
    {
        return NULL;
    }

    pCursor = pTree->pCursor;
    pData = pCursor->pData;

    if ( pCursor->pRight != NULL )
    {
        pCursor = pCursor->pRight;
        while ( pCursor->pLeft != NULL )
        {
            pCursor = pCursor->pLeft;
        }
    }
    else
    {
        AVLTREENODE *pParent = pCursor->pParent;
        while ( pParent != NULL && pParent->pRight == pCursor )
        {
            pCursor = pParent;
            pParent = pParent->pParent;
        }
        pCursor = pParent;
    }
    pTree->pCursor = pCursor;

    return pData;
}


AVLTREENODE *AVLTreeNode_GetNext(AVLTREENODE *pCurNode)
{
    AVLTREENODE *pCursor;

    if ( pCurNode == NULL )
    {
        return NULL;
    }

    pCursor = pCurNode;

    if ( pCursor->pRight != NULL )
    {
        pCursor = pCursor->pRight;
        while ( pCursor->pLeft != NULL )
        {
            pCursor = pCursor->pLeft;
        }
    }
    else
    {
        AVLTREENODE *pParent = pCursor->pParent;
        while ( pParent != NULL && pParent->pRight == pCursor )
        {
            pCursor = pParent;
            pParent = pParent->pParent;
        }
        pCursor = pParent;
    }

    return pCursor;
}

