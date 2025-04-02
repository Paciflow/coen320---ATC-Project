// ComputerSystem.h

#ifndef COMPUTERSYSTEM_H
#define COMPUTERSYSTEM_H

#include <sys/dispatch.h>

struct ClientMessage {
    int messageType;
    int messageData;
};

union Message {
    ClientMessage client_msg;
    struct _pulse pulse;
};

class ComputerSystem {
public:
    ComputerSystem();
    ~ComputerSystem();
    void run();

private:
    int chid; // Channel ID
};

#endif
