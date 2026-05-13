#ifndef FSE_H
#define FSE_H

#include "User.h"
#include "Permission.h"
#include <iostream>
using namespace std;

class FSE {
protected:
    char name[20];
    User* owner;
    Permission perm;

public:
    FSE() {
     name[0] = '\0';
        owner = NULL;
    }

    FSE(const char* n, User* u, Permission p) {
        int i = 0;
        while (n[i] != '\0' && i < 19) {
            name[i] = n[i];
            i++;
        }
        name[i]='\0';

        owner = u;
        perm = p;
    }

    virtual ~FSE() {}
    virtual bool isDirectory() const { return false; }
    const char* getName() {
    return name;
}
   virtual FSE* copy() = 0;
    virtual int getSize() = 0;
    virtual void show() = 0;
};

#endif
