#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>

class Communication {
public:
    Communication();
    void init();
    void handleRequests();
private:
    void processCommand(const String& command);
};

#endif // COMMUNICATION_H
