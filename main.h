#ifndef CDG_MAIN_H
#define CDG_MAIN_H

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include "row.h"
#include "provider.h"
#include "clinic.h"
#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdio.h>
#include <algorithm>

class ReportGenerator {
private:
    std::unordered_map<std::string, Clinic*> clinics;
    std::unordered_map<std::string, Provider*> prov;
    std::unordered_set<std::string> hyg;
    std::vector<Procedure*> procs;
    std::vector<double> clinicTotal;
    std::vector<double> grandTotal;
    std::vector<double> roleTotal;
    
    std::string HygienistFile = "files/Hygienists.csv";
    std::string ProdAdjFile = "files/ProdAdj.csv";
    std::string ColAdjFile = "files/ColAdj.csv";
    std::string PaymentTypesFile = "files/PaymentTypes.csv";
    std::vector<int> startDate;
    std::vector<int> endDate;

    bool failed;
    bool priorPeriod;

    int GetAdjType(std::string& desc);

    std::string dateToString(std::vector<int> date) {
        return std::to_string(date[0]) + "." + std::to_string(date[1]) + "." + std::to_string(date[2]);
    }
    std::vector<int> getDate(std::string date);
    std::string getPrefix() {
        std::string res = dateToString(startDate)+ '-' + dateToString(endDate);
        if (priorPeriod)
            res += "Suspended";
        
        return res;
    }
    bool inRange(std::vector<int> date);
    bool isHygienist(std::string name);
    bool isDoctor(std::string name);

    void sumProcs(Provider* p);
    void sumRoles(Clinic *c);

    void addToRole(std::vector<double>& toAdd);
    void addRoleToClinic();
    void addClinicToGrand();

    void indexDaySheet(std::string daySheet);
    
    void loadHygienists();
    void loadProdAdj();

    void loadColAdj();

    void loadPaymentTypes();

    void loadColumns();
    void filterByDateRange();
    
    void printHeader(std::fstream& out, std::string& provName);
    void printGrandSummary();

public:
    static int numProdAdj;
    static int numColAdj;
    static int numPaymentTypes;
    static std::vector<std::string> adjTypes;
  
    ReportGenerator(std::string daySheet): failed(false), priorPeriod(false) {
        adjTypes.push_back("Gross Production");
        loadColumns();
        startDate = std::vector<int>(3, 0);
        endDate = std::vector<int>(3, 0);

        clinicTotal = std::vector<double>(adjTypes.size(), 0.0);
        grandTotal = std::vector<double>(adjTypes.size(), 0.0);
        roleTotal = std::vector<double>(adjTypes.size(), 0.0);
        loadHygienists();
        this->indexDaySheet(daySheet);
    }
    void ProdAdjReport();
    void ColAdjReport();
    void PaymentReport();
    void SummaryReport();
    void AllReports();
    void indexAppPMT(std::string appPMT);
    void indexAppAdj(std::string appAdj);

    void indexProcedures();

    void clearAll();

};

#endif