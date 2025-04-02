// OperatorConsole.cpp

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>

struct AlertMessage {
    unsigned int id1;
    unsigned int id2;
    int time_to_violation;
    char alert[100];
};

int main() {
    name_attach_t* attach = name_attach(NULL, "operator_console", 0);
    if (attach == NULL) {
        perror("name_attach");
        return 1;
    }

    std::cout << "[Operator Console] Ready to receive alerts...\n";

    while (true) {
        AlertMessage msg;
        int rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        std::cout << "!! ALERT RECEIVED from Computer System !!\n";
        std::cout << "Aircraft " << msg.id1 << " and Aircraft " << msg.id2
            << " risk collision in <= " << msg.time_to_violation << " seconds\n";
        std::cout << "Message: " << msg.alert << "\n";

        MsgReply(rcvid, 0, NULL, 0);
    }

    name_detach(attach, 0);
    return 0;
}
