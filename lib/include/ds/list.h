/*
 * MollenOS - Generic List
 * Insertion, Deletion runs at O(1) 
 * Lookup by key, or search runs at O(n)
 * Pop, push runs at O(1)
 */

#ifndef _GENERIC_LIST_H_
#define _GENERIC_LIST_H_

/* Includes 
 * - Library */
#include <defs.h>
#include <ds/ds.h>
#include <os/spinlock.h>

/* The list node structure 
 * this is a generic list item
 * that holds an ident (key) and data */
typedef struct _ListNode {
	DataKey_t			Key;
	DataKey_t			SortKey;
	void				*Data;

	struct _ListNode		*Link;
	struct _ListNode		*Prev;
} ListNode_t;

/* This is the list structure
 * it holds basic information about the list */
typedef struct _ListNode ListIterator_t;
typedef struct _List List_t;

/* List Definitions 
 * Used to implement list with basic locking mechanisms */
#define LIST_NORMAL				0x0
#define LIST_SAFE				0x1

/* Sorted list? Normally 
 * the list is unsorted but supports different sorts */
#define LIST_SORT_ONINSERT		0x2
#define LIST_SORT_ONCALL		(0x4 | LIST_SORT_ONINSERT)

/* Foreach Macro(s)
 * They help keeping the code clean and readable when coding loops */
#define foreach(i, List) ListNode_t *i; for (i = ListBegin(List); i != NULL; i = ListNext(i))
#define foreach_nolink(i, List) ListNode_t *i; for (i = ListBegin(List); i != NULL; )
#define _foreach(i, List) for (i = ListBegin(List); i != NULL; i = ListNext(i))
#define _foreach_nolink(i, List) for (i = ListBegin(List); i != NULL; )

/* Protect against c++ files */
//_CODE_BEGIN

/* ListCreate
 * Instantiates a new list with the given attribs and keytype */
List_t* ListCreate(KeyType_t KeyType, Flags_t Attributes);

/* ListDestroy
 * Destroys the list and frees all resources associated
 * does also free all list elements and keys */
OsStatus_t ListDestroy(List_t *List);

/* ListLength
 * Returns the length of the given list */
size_t ListLength(List_t *List);

/* ListBegin
 * Retrieves the starting element of the list */
ListIterator_t* ListBegin(List_t *List);

/* ListNext
 * Iterates to the next element in the list and returns
 * NULL when the end has been reached */
ListIterator_t*ListNext(ListIterator_t *It);

/* ListCreateNode
 * Instantiates a new list node that can be appended to the list 
 * by ListAppend. If using an unsorted list set the sortkey == key */
ListNode_t* ListCreateNode(DataKey_t Key, DataKey_t SortKey, void *Data);

/* ListDestroyNode
 * Cleans up a list node and frees all resources it had */
OsStatus_t ListDestroyNode(List_t *List, ListNode_t *Node);

/* ListInsertAt
 * Insert the node into a specific position in the list, if position is invalid it is
 * inserted at the back. This function is not available for sorted lists, it will simply 
 * call ListInsert instead */
OsStatus_t ListInsertAt(List_t *List, ListNode_t *Node, int Position);

/* ListInsert 
 * Inserts the node into the front of the list. This should be used for sorted
 * lists, but is available for unsorted lists aswell */
OsStatus_t ListInsert(List_t *List, ListNode_t *Node);

/* ListAppend
 * Inserts the node into the the back of the list. This function is not
 * available for sorted lists, it will simply redirect to ListInsert */
OsStatus_t ListAppend(List_t *List,ListNode_t *Node);

/* List pop functions, the either 
 * remove an element from the back or 
 * the front of the given list and return the node */
ListNode_t* ListPopFront(List_t *List);

ListNode_t* ListPopBack(List_t *List);

/* ListGetNodeByKey
 * These are the node-retriever functions 
 * they return the list-node by either key data or index */
ListNode_t* ListGetNodeByKey(List_t *List, DataKey_t Key, int n);

/* These are the data-retriever functions 
 * they return the list-node by either key
 * node or index */
void* ListGetDataByKey(List_t *List, DataKey_t Key, int n);

/* ListExecute(s)
 * These functions execute a given function on all relevant nodes (see names) */
void ListExecuteOnKey(List_t *List, void(*Function)(void*, int, void*), DataKey_t Key, void *UserData);

/* ListExecute(s)
 * These functions execute a given function on all relevant nodes (see names) */
void ListExecuteAll(List_t *List, void(*Function)(void*, int, void*), void *UserData);

/* ListUnlinkNode
 * This functions unlinks a node and returns the next node for usage */
ListNode_t* ListUnlinkNode(List_t *List, ListNode_t *Node);

/* ListRemove
 * These are the deletion functions 
 * and remove based on either node index or key */
OsStatus_t ListRemoveByNode(List_t *List, ListNode_t* Node);

/* ListRemove
 * These are the deletion functions 
 * and remove based on either node index or key */
OsStatus_t ListRemoveByIndex(List_t *List, int Index);

/* ListRemove
 * These are the deletion functions 
 * and remove based on either node index or key */
OsStatus_t ListRemoveByKey(List_t *List, DataKey_t Key);

#endif //!_GENERIC_LIST_H_
