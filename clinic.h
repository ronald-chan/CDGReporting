#ifndef CDG_CLINIC_H
#define CDG_CLINIC_H

#include <vector>
#include "provider.h"
struct Clinic {
    std::string name;
    std::vector<double> doctorTotals; //0
    std::vector<double> HygienistTotals; //1
    std::vector<double> otherTotals; //2
    std::vector<Provider*> providers;
};

Clinic* CreateClinic(std::string name);
void AddProvider(Clinic* c, Provider* p);

double GetRoleTotalProdAdj(Clinic* c, int role);
double GetRoleNetProd(Clinic* c, int role);

double GetRoleColAdj(Clinic* c, int role);

double GetRoleTotalPayments(Clinic* c, int role);

#endif