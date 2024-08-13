#include "OpenRelTable.h"
#include<iostream>
#include <cstring>


void clearList(AttrCacheEntry* head) {
    for (AttrCacheEntry* it = head, *next; it != nullptr; it = next) {
        next = it->next;
        free(it);
    }
}

AttrCacheEntry* createAttrCacheEntryList (int size) {
    AttrCacheEntry *head = nullptr, *curr = nullptr;
    head = curr = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
    size--;
    while (size--) {
        curr->next = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
        curr = curr->next;
    }
    curr->next = nullptr;

    return head;
}

OpenRelTable::OpenRelTable()
{

    // initialize relCache and attrCache with nullptr
    for (int i = 0; i < MAX_OPEN; ++i)
    {
        RelCacheTable::relCache[i] = nullptr;
        AttrCacheTable::attrCache[i] = nullptr;
    }

    /************ Setting up Relation Cache entries ************/
    // (we need to populate relation cache with entries for the relation catalog
    //  and attribute catalog.)

    // setting up the variables
    RecBuffer relCatBlock (RELCAT_BLOCK);
    Attribute relCatRecord [RELCAT_NO_ATTRS];
    RelCacheEntry *relCacheEntry = nullptr;

    for (int relId = RELCAT_RELID; relId <= ATTRCAT_RELID+1; relId++) {
        relCatBlock.getRecord(relCatRecord, relId);

        relCacheEntry = (RelCacheEntry *) malloc (sizeof(RelCacheEntry));
        RelCacheTable::recordToRelCatEntry(relCatRecord, &(relCacheEntry->relCatEntry));
        relCacheEntry->recId.block = RELCAT_BLOCK;
        relCacheEntry->recId.slot = relId;

        RelCacheTable::relCache[relId] = relCacheEntry;
    }

    
    /************ Setting up Attribute cache entries ************/
    // (we need to populate attribute cache with entries for the relation catalog
    //  and attribute catalog.)

    // setting up the variables
    RecBuffer attrCatBlock (ATTRCAT_BLOCK);
    Attribute attrCatRecord [ATTRCAT_NO_ATTRS];
    AttrCacheEntry *attrCacheEntry = nullptr, *head = nullptr;

    for (int relId = RELCAT_RELID, recordId = 0; relId <= ATTRCAT_RELID+1; relId++) {
        int numberOfAttributes = RelCacheTable::relCache[relId]->relCatEntry.numAttrs;
        head = createAttrCacheEntryList (numberOfAttributes);
        attrCacheEntry = head;
        
        while (numberOfAttributes--) {
            attrCatBlock.getRecord(attrCatRecord, recordId);

            AttrCacheTable::recordToAttrCatEntry(
                attrCatRecord, 
                &(attrCacheEntry->attrCatEntry)
            );
            attrCacheEntry->recId.slot = recordId++;
            attrCacheEntry->recId.block = ATTRCAT_BLOCK;

            attrCacheEntry = attrCacheEntry->next;
        }

        AttrCacheTable::attrCache[relId] = head;
    }

    
    RecBuffer relCatBuffer (RELCAT_BLOCK);
    Attribute relCatRecord1 [RELCAT_NO_ATTRS];

    HeadInfo relCatHeader;
    relCatBuffer.getHeader(&relCatHeader);

    int relationIndex = -1;
    // char* relationName = "Students";

    for (int index = 2; index < relCatHeader.numEntries+4; index++) {
        relCatBuffer.getRecord(relCatRecord, index);

        if (strcmp(relCatRecord1[RELCAT_REL_NAME_INDEX].sVal, 
                "Students") == 0) { // matching the name of the record we want
            relationIndex = index;
        }
    }

}


OpenRelTable::~OpenRelTable() {
    for (int i = 0; i < MAX_OPEN; i++)
        free(RelCacheTable::relCache[i]);
    for (int i = 0; i < MAX_OPEN; i++)
        clearList(AttrCacheTable::attrCache[i]);
}

/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  // if relname is RELCAT_RELNAME, return RELCAT_RELID
    if (strcmp(relName, RELCAT_RELNAME) == 0)
        return RELCAT_RELID;
  // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
    if (strcmp(relName, ATTRCAT_RELNAME) == 0)
        return ATTRCAT_RELID;

    if (strcmp(relName, "Students") == 0)
        return 2;

  return E_RELNOTOPEN;
}