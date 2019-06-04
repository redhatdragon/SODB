#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#define SODBBool char
#define SODB_TRUE  1
#define SODB_FALSE 0

/*
SODB (Simple Object Data Base)

This database software is meant to be uber simple to
use and deploy.  Being header only and compatible
with older c compilers means this should be uber
easy to port to nearly any platform.

LAYOUT:
The layout is simple, it stores flat struct objects of
any desired size and number. It assumes there's only
one object type.  And that we are searching based on
index.  Can only use uint32_t number of objects. File
sizes are expected to be uint32_t number of bytes big.
The header takes up 8 of these bytes.  File size grows
with the data and there's no pre allocation.

HEADER:
The header layout is really basic and simple.  The
first 32 bits are reserved for telling the object count.
The second 32 bits describe how big every object can be.

WARNING:
This is not designed or intended to be multithreaded.
There is no compression or encryption of any kind.
Endiness must be considered when moving the data from
one platform to another.

NOTE:
This is meant to be compatible with the psxe compiler
which is why variables are declared before code within
any scope.  Also why there's a weird set of defines for
bools.
*/

uint32_t fileGetSize(FILE *fp){
    uint32_t prev;
    uint32_t size;
    prev=ftell(fp);
    fseek(fp, 0, SEEK_END);
    size=ftell(fp);
    fseek(fp,prev,SEEK_SET);
    return size;
}

struct SODBHeader {
    uint32_t count;
    uint32_t sizeOfObject;
};



SODBBool SODBInit(char *fileName, uint32_t objectSize){
    FILE *file = fopen(fileName, "w");
    struct SODBHeader header = {0, objectSize};
    if(!file){
        printf("SODB Error: Couldn't open file ", fileName, "\n");
        return SODB_FALSE;
    }
    fwrite(&header, sizeof(struct SODBHeader), 1, file);
    fclose(file);
    return SODB_TRUE;
}
SODBBool SODBIsValid(char *fileName){        
    FILE *file = fopen(fileName, "r+");
    uint32_t fileSize;
    struct SODBHeader header;
    if(!file){
        printf("SODB Error: Couldn't open file ", fileName, "\n");
        return SODB_FALSE;
    }
    fileSize = fileGetSize(file);
    if(fileSize < 8){
        printf("SODB Error: File ", *fileName, " corrupt, not properly initialized\n");
        fclose(file);
        return SODB_FALSE;
    }
    fread(&header, 8, 1, file) ;
    if(fileSize < header.count*header.sizeOfObject+8){
        printf("SODB Error: index out of range\n");
        fclose(file);
        return SODB_FALSE;
    }
    fclose(file);
    return SODB_TRUE;
}

SODBBool SODBGet(char *fileName, uint32_t index, void *outObjectBuffer){
    {if(SODBIsValid(fileName) == SODB_FALSE)  return SODB_TRUE;}

    FILE *file = fopen(fileName, "r+");
    struct SODBHeader header;

    if(!file){
        printf("SODB Error: Couldn't open file ", fileName, "\n");
        return SODB_FALSE;
    }
    fread(&header, 8, 1, file);
    if(index > header.count){
        printf("SODB Error: index out of range\n");
        fclose(file);
        return SODB_FALSE;
    }
    fseek(file, 8+index*header.sizeOfObject, SEEK_SET);
    fread(outObjectBuffer, header.sizeOfObject, 1, file);
    fclose(file);
    return SODB_TRUE;
}

SODBBool SODBReplace(char *fileName, uint32_t index, void *object){
    {if(SODBIsValid(fileName) == SODB_FALSE)  return SODB_TRUE;}

    FILE *file = fopen(fileName, "r+");
    struct SODBHeader header;

    if(!file){
        printf("SODB Error: Couldn't open file ", fileName, "\n");
        return SODB_FALSE;
    }
    fread(&header, 8, 1, file);
    if(index > header.count){
        printf("SODB Error: index out of range\n");
        fclose(file);
        return SODB_FALSE;
    }
    fseek(file, 8+index*header.sizeOfObject, SEEK_SET);
    fwrite(object, header.sizeOfObject, 1, file);
    fclose(file);
    return SODB_TRUE;
}

SODBBool SODBAppend(char *fileName, void *object){
    {if(SODBIsValid(fileName) == SODB_FALSE)  return SODB_FALSE;}

    FILE *file = fopen(fileName, "r+");
    struct SODBHeader header;

    if(!file){
        printf("SODB Error: Couldn't open file ", fileName, "\n");
        return SODB_FALSE;
    }
    fread(&header, 8, 1, file);
    fseek(file, 0, SEEK_SET);
    header.count++;
    fwrite(&header, sizeof(struct SODBHeader), 1, file);

    fseek(file, 8+(header.count*header.sizeOfObject-header.sizeOfObject), SEEK_SET);
    fwrite(object, header.sizeOfObject, 1, file);
    fclose(file);
    return SODB_TRUE;
}