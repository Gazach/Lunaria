#ifndef TYPE_H
#define TYPE_H

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_FLOATING_POINT,
    TYPE_CHARACTER,
    TYPE_VOID,
} Type;

typedef struct {
    Type type;
} TypeInfo;


#endif // TYPE_H