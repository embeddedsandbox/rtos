//==============================================================================
// Copyright 2020 Daniel Boals & Michael Boals
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
//==============================================================================

#include "types.h"

typedef struct _listNode_t listNode_t;

typedef struct _listNode_t
{
    listNode_t *    next;
    listNode_t *    prev;
    uint32_t        count;
} listNode_t;


typedef struct 
{
    listNode_t *    head;
    listNode_t *    tail;
    uint32_t        count;    
} dllist_t;


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void dllistPut(dllist_t * theList, listNode_t * theNode)
{  
    theNode->prev = theList->tail ;
    theNode->next = NULL;

    if (theList->tail != NULL)
    {
        theList->tail->next = theNode;
        theList->tail = theNode;
    }
    else
    {
        // if tail is NULL we are adding to an empty list
        theList->head = theNode;
        theList->tail = theNode;
    }

    theList->count++;    
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
listNode_t * dllistGetFromHead(dllist_t * theList)
{
    listNode_t * theNode = NULL;

    if (theList->head  != NULL)
    {
        theNode = theList->head;        
        theList->head = theList->head->next;

        if (theList->head != NULL)
        {
            theList->head->prev = NULL;
        }
        else
        {
            theList->tail = NULL;
        }
        
        theList->count--;
    }

    return theNode;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
listNode_t * dllistGetFromTail(dllist_t * theList)
{
    listNode_t * theNode = NULL;

    if (theList->tail  != NULL)
    {
        theNode = theList->tail;
        theList->tail = theList->tail->prev;

        if (theList->tail != NULL)
        {
            theList->tail->next = NULL;
        }
        else
        {
            theList->head = NULL;
        }
        
        theList->count--;
    }

    return theNode;
}

//-----------------------------------------------------------------------------
// Initialize a list structure 
//-----------------------------------------------------------------------------
void dllistInit(dllist_t * theList)
{
    theList->head   = NULL;
    theList->tail   = NULL;
    theList->count  = 0;
}

