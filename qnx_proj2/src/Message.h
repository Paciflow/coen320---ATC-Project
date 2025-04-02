#ifndef MESSAGES_H
#define MESSAGES_H

// Message Type IDs
#define MSG_TYPE_ALERT 1
#define MSG_TYPE_SET_N 2

// Alert Message (from ComputerSystem -> Operator)
struct AlertMessage {
    unsigned int id1;
    unsigned int id2;
    int time_to_violation;
    char alert[100];
};

// Operator command (OperatorConsole -> ComputerSystem)
struct ControlMessage {
    int msg_type;   // Always MSG_TYPE_SET_N
    int new_n;      // New prediction window value
};

#endif
