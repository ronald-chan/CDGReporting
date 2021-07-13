#ifndef CDG_ROW_CPP
#define CDG_ROW_CPP

#include "row.h"

Procedure *GenerateProcedure() {
    Procedure *proc = new Procedure;
    proc->procDate = std::vector<int>(3, 0);
    proc->entryDate = std::vector<int>(3, 0);
    return proc;
}

#endif //CDG_ROW_CPP