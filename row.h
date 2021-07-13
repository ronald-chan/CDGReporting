#ifndef CDG_ROW_H
#define CDG_ROW_H
#include <string>
#include <vector>
struct Procedure {
    std::vector<int> entryDate;
    std::vector<int> procDate;
    std::string procDesc;
    std::string patName;
    std::string priProvider;
    std::string secProvider;
    std::string clinic;
    std::string operatorID;
    double amt;
    std::string chartNum;
    int BT;
    int adjType;
}; //Procedure 

Procedure *GenerateProcedure();


#endif //CDG_ROW_H