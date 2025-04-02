#ifndef MESSAGES_H
#define MESSAGES_H

// Message Type IDs
#define MSG_TYPE_ALERT 1
#define MSG_TYPE_SET_N 2

// Alert Message (Computer → Operator)
struct AlertMessage {
    unsigned int id1;
    unsigned int id2;
    int time_to_violation;
    char alert[100];
};

// Control Message (Operator → Computer)
struct ControlMessage {
    int msg_type;
    int new_n;
};

// Aircraft Command (Computer → Aircraft)
struct AircraftCommand {
    char command[100];
};

#endif
