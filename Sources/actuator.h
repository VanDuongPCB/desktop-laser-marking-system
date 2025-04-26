
#ifndef ACTUATOR_H
#define ACTUATOR_H




class Actuator{
public:
    static bool setEnable(bool en);
    static bool setMarkResult(bool status);
    static bool setCvWidth(double width);
    static bool setTransferMode(bool on);
    static bool setStopper(int stopper);
    static bool hasTrigger();
    static bool confirmTrigger();
    static bool setCompleteBit();
};

#endif // ACTUATOR_H
