#ifndef CDG_CLINIC_CPP
#define CDG_CLINIC_CPP

#include "clinic.h"
#include "main.h"

Clinic* CreateClinic(std::string name) {
    Clinic* c = new Clinic;
    c->name = name;
    c->HygienistTotals = std::vector<double>(ReportGenerator::adjTypes.size(), 0);
    c->doctorTotals = std::vector<double>(ReportGenerator::adjTypes.size(), 0);
    c->otherTotals = std::vector<double>(ReportGenerator::adjTypes.size(), 0);
    return c;
}

void AddProvider(Clinic* c, Provider* p) {
    c->providers.push_back(p);
}

double GetRoleTotalProdAdj(Clinic* c, int role) {
    std::vector<double> toSum;
    if(role == 0) 
        toSum = c->doctorTotals;
    else if (role == 1)
        toSum = c->HygienistTotals;
    else
        toSum = c->otherTotals;
    
    double total = 0.0;
    for(int i = 1; i < ReportGenerator::numProdAdj + 1; i++)
        total += toSum[i];

    return total;
}

double GetRoleNetProd(Clinic* c, int role) {
    std::vector<double> toSum;
    if(role == 0) 
        toSum = c->doctorTotals;
    else if (role == 1)
        toSum = c->HygienistTotals;
    else
        toSum = c->otherTotals;
    
    double total = 0.0;
    for(int i = 0; i < ReportGenerator::numProdAdj + 1; i++)
        total += toSum[i];

    return total;
}

double GetRoleColAdj(Clinic* c, int role) {
    std::vector<double> toSum;
    if(role == 0) 
        toSum = c->doctorTotals;
    else if (role == 1)
        toSum = c->HygienistTotals;
    else
        toSum = c->otherTotals;
    
    double total = 0.0;
    for(int i = ReportGenerator::numProdAdj + 1; 
        i < ReportGenerator::numProdAdj + ReportGenerator::numColAdj + 1; 
        i++)
        total += toSum[i];

    return total;
}

double GetRoleTotalPayments(Clinic* c, int role) {
    std::vector<double> toSum;
    if(role == 0) 
        toSum = c->doctorTotals;
    else if (role == 1)
        toSum = c->HygienistTotals;
    else
        toSum = c->otherTotals;
    
    double total = 0.0;
    for(int i = ReportGenerator::numProdAdj + ReportGenerator::numColAdj + 1; 
    i < ReportGenerator::adjTypes.size(); 
    i++)
        total += toSum[i];

    return total;
}

#endif