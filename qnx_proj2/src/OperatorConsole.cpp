// OperatorConsole.cpp

#include <iostream>
#include <sys/dispatch.h>
#include <cstring>
#include <unistd.h>
#include "Messages.h"

int main() {
    std::cout << "[OperatorConsole] Starting IPC...\n";

    int coid = name_open("computer_system", 0);
    if (coid == -1) {
        perror("name_open failed");
        return 1;
    }

    std::cout << "[OperatorConsole] Connected to ComputerSystem.\n";

    while (true) {
        std::cout << "Enter new prediction window n (seconds) or -1 to quit: ";
        int input_n;
        std::cin >> input_n;

        if (input_n == -1) break;

        ControlMessage msg;
        msg.msg_type = MSG_TYPE_SET_N;
        msg.new_n = input_n;

        if (MsgSend(coid, &msg, sizeof(msg), NULL, 0) == -1) {
            perror("MsgSend failed");
        }
        else {
            std::cout << "Sent new n = " << input_n << " to ComputerSystem.\n";
        }
    }

    name_close(coid);
    return 0;
}
