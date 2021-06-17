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
    std::vector<std::string> adjTypes;
    std::vector<double> clinicTotal;
    std::vector<double> grandTotal;
    std::vector<double> roleTotal;
    
    std::string HygienistFile = "files/Hygienists.csv";
    bool failed;

    bool isHygienist(std::string name);
    bool isDoctor(std::string name);

    void sumProcs(Provider* p);
    void sumRoles(Clinic *c);

    void addToRole(std::vector<double>& toAdd);
    void addRoleToClinic();
    void addClinicToGrand();

    void indexDaySheet(std::string daySheet);
    void indexProcedures();
    void loadHygienists();
    
    void printHeader(std::fstream& out, std::string& provName);
    void printGrandSummary();
public:
    static std::vector<int> printOrder;
    ReportGenerator(std::string daySheet): failed(false) {
        adjTypes.push_back("Gross Production");
        adjTypes.push_back("-Ins Credit Adj");
        adjTypes.push_back("+Ins Debit Adj");
        adjTypes.push_back("-Professional Allowance");
        adjTypes.push_back("+Other Dr Production Adjustment");
        adjTypes.push_back("-Other Dr Production Adjustment");
        adjTypes.push_back("-Hygiene Adjustment - Hygienist");
        adjTypes.push_back("-Hygiene Adjustment - Dr.");
        adjTypes.push_back("-Pre-Conversion Credit Balance"); //8
        adjTypes.push_back("-Senior Citizen Courtesy");
        adjTypes.push_back("-Staff Courtesy");
        adjTypes.push_back("-Reverse Finance Charges");
        adjTypes.push_back("+Pre-Conversion Debit Balance"); //12
        adjTypes.push_back("-Pre-Conversion Credit"); //13
        adjTypes.push_back("-Reversal of Bank Fee");
        adjTypes.push_back("+Bank Fee");
        adjTypes.push_back("-Write Off");
        adjTypes.push_back("+Payment Transfer From");
        adjTypes.push_back("-Payment Transfer To");
        adjTypes.push_back("+Patient Refund - Check");
        adjTypes.push_back("+Patient Refund Credit Card");
        adjTypes.push_back("+Insurance Refund");
        adjTypes.push_back("-Void Refund");
        adjTypes.push_back("+Returned Check");
        adjTypes.push_back("+Pt Initiated Credit Card Charge Back");
        adjTypes.push_back("-Transfer Balance From");
        adjTypes.push_back("+Transfer Balance To");
        adjTypes.push_back("-Charge Reduction");
        adjTypes.push_back("-Care Credit Discount");
        adjTypes.push_back("+Pre-Conversion Debit"); //29
        adjTypes.push_back("Care Credit Payment");
        adjTypes.push_back("Cash Payment");
        adjTypes.push_back("Check Payment");
        adjTypes.push_back("Credit Card Payment");
        adjTypes.push_back("INS Credit Card Payment");
        adjTypes.push_back("Insurance ACH Payment");
        adjTypes.push_back("Insurance Payment");
        
        printOrder.push_back(0);
        printOrder.push_back(3);
        printOrder.push_back(4);
        printOrder.push_back(5);
        printOrder.push_back(6);
        printOrder.push_back(7);
        printOrder.push_back(9);
        printOrder.push_back(10);
        printOrder.push_back(27);
        printOrder.push_back(1);
        printOrder.push_back(2);
        printOrder.push_back(13);
        printOrder.push_back(11);
        printOrder.push_back(29);
        printOrder.push_back(8);
        printOrder.push_back(14);
        printOrder.push_back(15);
        printOrder.push_back(16);
        printOrder.push_back(17);
        printOrder.push_back(18);
        printOrder.push_back(19);
        printOrder.push_back(20);
        printOrder.push_back(21);
        printOrder.push_back(22);
        printOrder.push_back(23);
        printOrder.push_back(24);
        printOrder.push_back(25);
        printOrder.push_back(26);
        printOrder.push_back(28);
        printOrder.push_back(12);
        printOrder.push_back(30);
        printOrder.push_back(31);
        printOrder.push_back(32);
        printOrder.push_back(33);
        printOrder.push_back(34);
        printOrder.push_back(35);
        printOrder.push_back(36);

        clinicTotal = std::vector<double>(37, 0.0);
        grandTotal = std::vector<double>(37, 0.0);
        roleTotal = std::vector<double>(37, 0.0);
        loadHygienists();
        this->indexDaySheet(daySheet);
        this->indexProcedures();
    }
    void ProdAdjReport();
    void ColAdjReport();
    void PaymentReport();
    void SummaryReport();
    void AllReports();
};

#endif