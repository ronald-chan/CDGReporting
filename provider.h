#ifndef CDG_PROVIDER_H
#define CDG_PROVIDER_H
#include <string>
#include <vector>
struct Provider {
    std::string name;
    std::vector<int> procKeys;
    std::vector<double> totals;
    double totalProdAdj;
    double netProd;
    double totalPayments;
    double totalColAndGroupAdj;
};

Provider* CreateProvider(std::string name);

void AddProc(Provider* p, int index);
int GetProc(Provider* p, int i);

void AddToCol(Provider* p, int i, double val);

double GetNetProduction(Provider* p);
double GetProdAdjSubtotal(Provider* p);
double GetPaySubtotal(Provider* p);
double GetCollecAdjSubtotal(Provider* p);
#endif