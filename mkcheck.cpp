#include <stdio.h>

#define NUM_ALL_ITEMS (16)
#define TO_UINT(_val) (__uint64_t)(_val)

typedef struct AllocItem
{
    void *ptr;
    struct AllocItem *pNext;
} AllocItems;

static AllocItems freeFirst;
static AllocItems useFirst;

static AllocItems pAllItems[NUM_ALL_ITEMS];

void memcheckInit()
{
    printf("memcheckInit\n");
    for (int i = 0; i < NUM_ALL_ITEMS; i++)
    {
        pAllItems[i].ptr = NULL;
        pAllItems[i].pNext = (i == NUM_ALL_ITEMS - 1) ? NULL : &pAllItems[i + 1];
    }
    freeFirst.pNext = &pAllItems[0];
    useFirst.pNext = NULL;
}

void memcheckAlloc(void *ptr, size_t sz)
{
    if (freeFirst.pNext == NULL)
    {
        printf("memcheck: too many alloc items\n");
        return;
    }
    AllocItems *pTmp = freeFirst.pNext;
    freeFirst.pNext = pTmp->pNext;
    pTmp->pNext = useFirst.pNext;
    pTmp->ptr = ptr;
    useFirst.pNext = pTmp;
}

void memcheckFree(void *ptr)
{
    AllocItems *pFound = NULL;
    AllocItems *pFoundPrev = &useFirst;
    if (ptr == NULL)
        return;

    for (AllocItems *pa = useFirst.pNext; pa; pa = pa->pNext)
    {
        if (TO_UINT(ptr) == TO_UINT(pa->ptr))
        {
            pFound = pa;
            break;
        }
        pFoundPrev = pa;
    }
    if (pFound != NULL)
    {
        pFoundPrev->pNext = pFound->pNext;
        pFound->pNext = freeFirst.pNext;
        freeFirst.pNext = pFound;
    }
    else
    {
        printf("Can not find ptr %ld\n", TO_UINT(ptr));
    }
}

void memcheckPrint()
{
    int i = 0;
    printf("----------- memcheck ----------\n");
    for (AllocItems *pa = useFirst.pNext; pa; pa = pa->pNext)
    {
        printf(" [%d] ptr=%ld\n", i, TO_UINT(pa->ptr));
        i++;
    }
}
