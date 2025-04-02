/*
 * ComputerSystem.h
 *
 *  Created on: Apr 2, 2025
 *      Author: p_uwamuk
 */

#ifndef SRC_COMPUTERSYSTEM_H_
#define SRC_COMPUTERSYSTEM_H_


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



#endif /* SRC_COMPUTERSYSTEM_H_ */
