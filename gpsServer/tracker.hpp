#ifndef tracker//.hpp // Защита от повторного включения
#define tracker

#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

class Tracker {
public:
    //int time;
    int ID;
    string name;
    double longitudine;
    double latitudine;
    
    Tracker() {
        //srand(time_t(0));
        //time = rand() % 24;
        ID = rand() % 100;
        name = to_string(ID);
        longitudine = rand() % 50;
        latitudine = rand() % 50;
    }

};

#endif //tracker_H
