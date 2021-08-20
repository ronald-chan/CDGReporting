#ifndef CDG_PROVIDER_CPP
#define CDG_PROVIDER_CPP

#include "provider.h"
#include "main.h"

Provider* CreateProvider(std::string name) {
    Provider* p = new Provider;
    p->name = name;
    p->totals = std::vector<double>(ReportGenerator::adjTypes.size(), 0.0);
    p->totalProdAdj = 0;
    p->totalPayments = 0;
    p->totalColAndGroupAdj = 0;
    p->netProd = 0;
    return p;
}

void AddProc(Provider* p, int index) {
    p->procKeys.push_back(index);
}

int GetProc(Provider* p, int i) {
    return p->procKeys[i];
}

void AddToCol(Provider* p, int i, double val) {
    p->totals[i] += val;
}

double GetNetProduction(Provider* p) {
    if(p->netProd != 0) 
        return p->netProd;
    p->netProd = p->totals[0] + GetProdAdjSubtotal(p);
    return p->netProd;
}

double GetProdAdjSubtotal(Provider* p) {
    if(p->totalProdAdj != 0)
        return p->totalProdAdj;
    for (int i = 1; i < ReportGenerator::numProdAdj + 1; i++) {
        p->totalProdAdj += p->totals[i];
    }
    return p->totalProdAdj;
}

double GetPaySubtotal(Provider* p) {
    if (p->totalPayments != 0)
        return p->totalPayments;
    for(int i = ReportGenerator::numProdAdj + ReportGenerator::numColAdj + 1; i < p->totals.size(); i++) {
        p->totalPayments += p->totals[i];
    }
    return p->totalPayments;
}

double GetCollecAdjSubtotal(Provider* p) {
    if(p->totalColAndGroupAdj != 0) 
        return p->totalColAndGroupAdj;
    for(int i = ReportGenerator::numProdAdj + 1; i < ReportGenerator::numProdAdj + ReportGenerator::numColAdj + 1; i++) {
        p->totalColAndGroupAdj += p->totals[i];
    }
    return p->totalColAndGroupAdj;
}


#endif