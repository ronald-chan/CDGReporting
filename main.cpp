#ifndef CDG_MAIN_CPP
#define CDG_MAIN_CPP

#include "main.h"
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
#include <iomanip>

int ReportGenerator::numProdAdj;
int ReportGenerator::numColAdj;
int ReportGenerator::numPaymentTypes;
std::vector<std::string> ReportGenerator::adjTypes;

bool providerCompare(const Provider * a, const Provider * b) { 
    return (a->name).compare(b->name) < 0; 
};

std::vector<int> ReportGenerator::getDate(std::string date) {
    std::vector<int> res = std::vector<int>(3, 0);
    sscanf(date.c_str(), "%d/%d/%d", &res[0], &res[1], &res[2]);
    return res;
}

void ReportGenerator::addToRole(std::vector<double>& toAdd) {
    for(int i = 0; i < toAdd.size(); i++)
        roleTotal[i] += toAdd[i];
}

void ReportGenerator::addRoleToClinic() {
    for(int i = 0; i < clinicTotal.size(); i++) 
        clinicTotal[i] += roleTotal[i];
    roleTotal = std::vector<double>(37, 0.0);
}

void ReportGenerator::addClinicToGrand() {
    for(int i = 0; i < clinicTotal.size(); i++) 
        grandTotal[i] += clinicTotal[i];
    clinicTotal = std::vector<double>(37, 0.0);
}

int ReportGenerator::GetAdjType(std::string& desc) {
    for(int i = 1; i < adjTypes.size(); i++) {
        if(desc.find(adjTypes[i]) != desc.npos)
            return i;
        
    }
    return 0;
}

void ReportGenerator::loadHygienists() {

    std::ifstream reader;
    reader.open(HygienistFile);
    if (!reader.is_open()) {
        std::cout << "cannot open Hygienist file in " << HygienistFile << std::endl;
        failed = true;
        return;
    }
    std::string curr;
    std::getline(reader, curr);
    while(!reader.eof()) {
        auto it = hyg.find(curr);
        if(it != hyg.end()) {
            std::cout << "duplicate name " << curr << " found in Hygienist file, treating as identical" << std::endl;
        }
        hyg.insert(curr);
        std::getline(reader, curr);
    }
    // for(auto it = hyg.begin(); it != hyg.end(); it++) {
    //     std::cout << *it << std::endl;
    // }
    reader.close();
}

void ReportGenerator::loadProdAdj() {
    std::ifstream reader;
    reader.open(ProdAdjFile);
    if (!reader.is_open()) {
        std::cout << "cannot open Production Adjustment file in " << HygienistFile << std::endl;
        failed = true;
        return;
    }
    std::string curr;
    std::getline(reader, curr);
    int i = 0;
    while(!reader.eof()) {
        auto it = hyg.find(curr);
        if(it != hyg.end()) {
            std::cout << "duplicate ProdAdj " << curr << " found in file, treating as identical" << std::endl;
        }
        adjTypes.push_back(curr);
        std::getline(reader, curr);
        i++;
    }
    numProdAdj = i;
    reader.close();
}

void ReportGenerator::loadColAdj() {
    std::ifstream reader;
    reader.open(ColAdjFile);
    if (!reader.is_open()) {
        std::cout << "cannot open Collection Adjustment file in " << HygienistFile << std::endl;
        failed = true;
        return;
    }
    std::string curr;
    std::getline(reader, curr);
    int i = 0;
    while(!reader.eof()) {
        auto it = hyg.find(curr);
        if(it != hyg.end()) {
            std::cout << "duplicate ColAdj " << curr << " found in file, treating as identical" << std::endl;
        }
        adjTypes.push_back(curr);
        std::getline(reader, curr);
        i++;
    }
    numColAdj = i;
    reader.close();
}

void ReportGenerator::loadPaymentTypes() {
    std::ifstream reader;
    reader.open(PaymentTypesFile);
    if (!reader.is_open()) {
        std::cout << "cannot open Payment Types file in " << HygienistFile << std::endl;
        failed = true;
        return;
    }
    std::string curr;
    std::getline(reader, curr);
    int i = 0;
    while(!reader.eof()) {
        auto it = hyg.find(curr);
        if(it != hyg.end()) {
            std::cout << "duplicate Payment Type " << curr << " found in file, treating as identical" << std::endl;
        }
        adjTypes.push_back(curr);
        std::getline(reader, curr);
        i++;
    }
    numPaymentTypes = i;
    reader.close();
}

void ReportGenerator::loadColumns() {
    loadProdAdj();
    loadColAdj();
    loadPaymentTypes();
}

bool ReportGenerator::inRange(std::vector<int> date) {
    if(date[0] >= startDate[0] && date[0] <= endDate[0]) {
        if(date[1] >= startDate[1] && date[1] <= endDate[1]) {
            if(date[2] >= startDate[2] && date[2] <= endDate[2]) {
                return true;
            }
        }
    }
    return false;
}

bool ReportGenerator::isHygienist(std::string name) {
    auto it = hyg.find(name);
    return it != hyg.end() && name.find("PRV") == name.npos && name.find("SUSPENDED") == name.npos;
}

bool ReportGenerator::isDoctor(std::string name) {
    auto it = hyg.find(name);
    return it == hyg.end() && name.find("PRV") == name.npos && name.find("SUSPENDED") == name.npos;
}

void ReportGenerator::printHeader(std::fstream& out, std::string& provName) {
    out << "," << provName << ",,,Production Adjustments,";
    for(int k = 1; k < adjTypes.size(); k++) {
        out << ',';
        if(k == numProdAdj)
            out << "Collection Adjustments,";
        else if (k == numProdAdj + numColAdj)
            out << "Payment Types,";
    }
    
    out << "\n,,,";
    for(int k = 0; k < adjTypes.size(); k++) {
        out << ' ' << adjTypes[k] << ',';
        if(k == numProdAdj || k == numProdAdj + numColAdj)
            out << ',';
    }
    out << "\n";
}

void ReportGenerator::indexDaySheet(std::string daySheet) {
    priorPeriod = false;
    //set up file reader
    std::ifstream reader;
    //std::ofstream check;
    reader.open(daySheet);
    //check.open("outputtest.csv");
    if (!reader.is_open()) {
        //file existence error
        std::cout << "cannot open file " << daySheet << std::endl;
        failed = true;
        return;
    }
    std::string temp;
    std::string line;
    std::getline(reader, line);
    int linecount = 1;
    bool first = true;
    while(!reader.eof()) { //loop while hasn't hit eof
        Procedure *current = GenerateProcedure();
        //std::cout << linecount++ << std::endl;
        std::stringstream s(line);
        std::string data;
        //skip to head of data
        for(int i = 0; i < 3; i++)
            std::getline(s, data, ',');

        //get date
        if(first) {
            data = data.substr(1, data.length() - 2);
            startDate = getDate(data.substr(0, data.find(' ')));
            data = data.substr(data.find(' ') + 3);
            endDate = getDate(data.substr(0, data.find(' ')));

            first = false;
        }

        for(int i = 0; i < 19; i++)
            std::getline(s, data, ',');

        //entry date
        std::getline(s, data, ',');
        //std::cout << data << std::endl;
        current->entryDate = getDate(data);

        //proc date
        std::getline(s, data, ',');
        //std::cout << data << std::endl;
        current->procDate = getDate(data);

        //name
        s.ignore(1); //ignore starting quote
        data.clear();
        std::getline(s, data, ',');
        current->patName = data;
        while (current->patName.at(current->patName.length() - 1) != '\"') {
            current->patName += ",";
            std::getline(s, data, ',');
            current->patName += data;
        }
        current->patName = current->patName.substr(0, current->patName.length() - 1);

        //desc
        s.ignore(1); //ignore starting quote
        std::getline(s, data, '\"');
        current->procDesc = data;
        s.ignore(1); //ignore comma
        current->adjType = GetAdjType(data);

        //charges/credits
        std::getline(s, data, ','); //read to first comma
        //std::cout << "data: " << data << "end data" << std::endl;
        while (data[0] == '\"' && data[data.length() - 1] != '\"') {
            std::string temp = data;
            std::getline(s, data, ',');
            data = temp + data;
        }

        bool charge = data.length() != 0;
        if(!charge) { //if first is empty
            std::getline(s, data, ',');
            while (data[0] == '\"' && data[data.length() - 1] != '\"') {
                std::string temp = data;
                std::getline(s, data, ',');
                data = temp + data;
            }
        }
        if(data[0] == '\"') //remove quotes
            data = data.substr(1, data.length() - 2);

        if(data[0] == '{') //remove {} which can occur
            data = data.substr(5, data.length()-5);

        

        data.erase(std::remove(data.begin(), data.end(), ','), data.end());
        //std::cout << linecount << "\t" << data << std::endl;
        current->amt = std::stod(data);
        //check << current->amt << ",\n";
        if(charge)
            std::getline(s, data, ',');

        //std::cout << "amt: " << current->amt << "endamt" << std::endl;

        //BT
        std::getline(s, data, ',');
        current->BT = std::stoi(data);

        //sec provider
        std::getline(s, data, ',');
        if(data.length() > 2)
            current->secProvider = data.substr(1, data.length() - 2);
        else
            current->secProvider = "";

        //clinic
        std::getline(s, data, ',');
        current->clinic = data.substr(1, data.length() - 2);

        //chart
        std::getline(s, data, ',');
        current->chartNum = data.length() == 0 ? "-1" : data.substr(1, data.length() - 2);

        //pri provider
        std::getline(s, data, ',');
        data = data.substr(1, data.length()-2);
        if(data[0] == 'P')
            current->priProvider = data.substr(6, data.length() - 6);
        else 
            current->priProvider = current->clinic + " " + data;

        /*std::cout << current->procDate << '\t' << current->procDesc << '\n' 
            << current->adjType << '\t' << current->amt << '\n'
            << current->patName << '\n' 
            << current->priProvider << '\t' << current->secProvider << '\t' << current->clinic << std::endl << std::endl;*/
        procs.push_back(current);
        std::getline(reader, line);
        linecount++;
    }
    reader.close();
    this->indexProcedures();
}

void ReportGenerator::indexAppPMT(std::string appPMT) {
    priorPeriod = true;
    //set up file reader
    std::ifstream reader;
    //std::ofstream check;
    reader.open(appPMT);
    //check.open("outputtest.csv");
    if (!reader.is_open()) {
        //file existence error
        std::cout << "cannot open file " << appPMT << std::endl;
        failed = true;
        return;
    }
    std::string temp;
    std::string line;
    std::getline(reader, line);
    int linecount = 1;
    bool first = true;
    while(!reader.eof()) { //loop while hasn't hit eof
        Procedure *current = GenerateProcedure();
        //std::cout << linecount++ << std::endl;
        std::stringstream s(line);
        std::string data;
        //skip to head of data
        for(int i = 0; i < 3; i++)
            std::getline(s, data, ',');

        //get date
        if(first) {
            data = data.substr(1, data.length() - 2);
            startDate = getDate(data.substr(0, data.find(' ')));
            data = data.substr(data.find(' ') + 3);
            endDate = getDate(data.substr(0, data.find(' ')));
            //std::cout << startDate[0] << startDate[1] << startDate[2] 
            //    << endDate[0] << endDate[1] << endDate[2] << std::endl;
            first = false;
        }

        for(int i = 0; i < 19; i++)
            std::getline(s, data, ',');

        //pri provider
        std::getline(s, data, ',');
        data = data.substr(1, data.length()-2);
        if(data[0] == 'P')
            current->priProvider = data.substr(10, data.length() - 10);
        else 
            current->priProvider = current->clinic + " " + data;

        //desc
        s.ignore(1); //ignore starting quote
        std::getline(s, data, '\"');
        current->procDesc = data;
        s.ignore(1); //ignore comma
        current->adjType = GetAdjType(data);
        std::getline(s, data, ',');

        //entry date
        std::getline(s, data, ',');
        current->entryDate = getDate(data);
        if(inRange(current->entryDate)) {
            std::getline(reader, line);
            continue;
        }

        //proc date
        std::getline(s, data, ',');
        current->procDate = getDate(data);

        //name
        s.ignore(1); //ignore starting quote
        data.clear();
        std::getline(s, data, '\"');
        current->patName = data;
        s.ignore(1); //ignore comma

        //chart
        std::getline(s, data, ',');
        current->chartNum = data.length() == 0 ? "-1" : data.substr(1, data.length() - 2);

        //BT
        std::getline(s, data, ',');
        current->BT = std::stoi(data);

        std::getline(s, data, ',');

        //clinic
        std::getline(s, data, ',');
        current->clinic = data.substr(1, data.length() - 2);

        //skip op
        std::getline(s, data, ',');

        //charges
        std::getline(s, data, ','); //read to first comma
        while (data[0] == '\"' && data[data.length() - 1] != '\"') {
            std::string temp = data;
            std::getline(s, data, ',');
            data = temp + data;
        }

        if(data[0] == '\"') //remove quotes
            data = data.substr(1, data.length() - 2);

        if(data[0] == '{') //remove {} which can occur
            data = data.substr(5, data.length()-5);

        data.erase(std::remove(data.begin(), data.end(), ','), data.end());
        current->amt = std::stod(data);


        /*std::cout <<  current->procDesc << '\n' 
            << current->adjType << '\t' << current->amt << '\n'
            << current->patName << '\n' 
            << current->priProvider << '\t' << current->clinic << std::endl << std::endl;*/
        procs.push_back(current);
        std::getline(reader, line);
    }
    reader.close();
}

void ReportGenerator::indexAppAdj(std::string appAdj) {
    priorPeriod = true;
    //set up file reader
    std::ifstream reader;
    //std::ofstream check;
    reader.open(appAdj);
    //check.open("outputtest.csv");
    if (!reader.is_open()) {
        //file existence error
        std::cout << "cannot open file " << appAdj << std::endl;
        failed = true;
        return;
    }
    std::string temp;
    std::string line;
    std::getline(reader, line);
    int linecount = 1;
    bool first = true;
    while(!reader.eof()) { //loop while hasn't hit eof
        Procedure *current = GenerateProcedure();
        //std::cout << linecount++ << std::endl;
        std::stringstream s(line);
        std::string data;
        //skip to head of data
        for(int i = 0; i < 3; i++)
            std::getline(s, data, ',');

        //get date
        if(first) {
            data = data.substr(1, data.length() - 2);
            startDate = getDate(data.substr(0, data.find(' ')));
            data = data.substr(data.find(' ') + 3);
            endDate = getDate(data.substr(0, data.find(' ')));
            //std::cout << startDate[0] << startDate[1] << startDate[2] 
            //    << endDate[0] << endDate[1] << endDate[2] << std::endl;
            first = false;
        }

        for(int i = 0; i < 18; i++)
            std::getline(s, data, ',');

        //pri provider
        std::getline(s, data, ',');
        data = data.substr(1, data.length()-2);
        if(data[0] == 'P')
            current->priProvider = data.substr(10, data.length() - 10);
        else 
            current->priProvider = current->clinic + " " + data;

        //desc
        s.ignore(1); //ignore starting quote
        std::getline(s, data, '\"');
        current->procDesc = data;
        s.ignore(1); //ignore comma
        current->adjType = GetAdjType(data);

        //entry date
        std::getline(s, data, ',');
        current->entryDate = getDate(data);
        if(inRange(current->entryDate)) {
            std::getline(reader, line);
            continue;
        }

        //proc date
        std::getline(s, data, ',');
        current->procDate = getDate(data);

        //name
        s.ignore(1); //ignore starting quote
        data.clear();
        std::getline(s, data, '\"');
        current->patName = data;
        s.ignore(1); //ignore comma

        //chart
        std::getline(s, data, ',');
        current->chartNum = data.length() == 0 ? "-1" : data.substr(1, data.length() - 2);

        //BT
        std::getline(s, data, ',');
        current->BT = std::stoi(data);

        std::getline(s, data, ',');

        //clinic
        std::getline(s, data, ',');
        current->clinic = data.substr(1, data.length() - 2);

        //skip op
        std::getline(s, data, ',');

        //charges
        std::getline(s, data, ','); //read to first comma
        while (data[0] == '\"' && data[data.length() - 1] != '\"') {
            std::string temp = data;
            std::getline(s, data, ',');
            data = temp + data;
        }

        if(data[0] == '\"') //remove quotes
            data = data.substr(1, data.length() - 2);

        if(data[0] == '{') //remove {} which can occur
            data = data.substr(5, data.length()-5);

        data.erase(std::remove(data.begin(), data.end(), ','), data.end());
        current->amt = std::stod(data);
        //std::cout << current->amt << std::endl;


        /*std::cout <<  current->procDesc << '\n' 
            << current->adjType << '\t' << current->amt << '\n'
            << current->patName << '\n' 
            << current->priProvider << '\t' << current->clinic << std::endl << std::endl;*/
        procs.push_back(current);
        std::getline(reader, line);
    }
    reader.close();
}

void ReportGenerator::indexProcedures() {
    //add procedures
    for (int i = 0; i < procs.size(); i++) {
        auto it = clinics.find(procs[i]->clinic);
        if(it == clinics.end()) {
            clinics[procs[i]->clinic] = CreateClinic(procs[i]->clinic);
        }
        auto it2 = prov.find(procs[i]->priProvider);
        // if(priorPeriod && strcmp(procs[i]->priProvider.c_str(), "CDGPRV") == 0) {
        //     std::cout << "found" << std::endl;
        // }
        if(it2 == prov.end()) {
            prov[procs[i]->priProvider] = CreateProvider(procs[i]->priProvider);
            AddProvider(clinics[procs[i]->clinic], prov[procs[i]->priProvider]);
        }
        AddProc(prov[procs[i]->priProvider], i);
        
    }
    for (auto it = clinics.begin(); it != clinics.end(); ++it) {
        Clinic* c = it->second;
        std::sort(c->providers.begin(), c->providers.end(), providerCompare);
        for (int i = 0; i < c->providers.size(); i++) {
            sumProcs(c->providers[i]);
        }
        sumRoles(c);
    }
}

void ReportGenerator::printGrandSummary() {
    std::fstream output;
    output.open(getPrefix() + "clinicsummary.csv");
    std::string prov = "ALL CLINIC SUMMARY";
    printHeader(output, prov);
    double prodAdj = 0.0;
    double prodTotal = 0.0;
    double payTotal = 0.0;
    double collAdj = 0.0;
    output << std::setprecision(15) << ",,,";
    for(int k = 0; k < grandTotal.size(); k++) {
        output << std::setprecision(15) << grandTotal[k] << ',';
        if(k < numProdAdj + 1)
            prodTotal += grandTotal[k];
        else if (k < numProdAdj + numColAdj + 1) 
            collAdj += grandTotal[k];
        else
            payTotal += grandTotal[k];
        if(k == numProdAdj || k == numProdAdj + numColAdj)
            output << std::setprecision(15) << ',';
    }

    prodAdj = prodTotal - grandTotal[0];

    output << std::setprecision(15) << "\n\n\nProduction Adjustments Grand Total," << prodAdj << "\n";
    output << std::setprecision(15) << "Production Grand Total," << prodTotal << ",\n";
    output << std::setprecision(15) << "Payment Grand Total," << payTotal << ",\n";
    output << std::setprecision(15) << "Collections Adjustment Grand Total," << collAdj << ",";
    output.close();
}

void ReportGenerator::sumProcs(Provider *p) {
    for (int j = 0; j < p->procKeys.size(); j++){
        int index = GetProc(p, j);
        Procedure* proc = procs[index];
        //if ((proc->amt >= 0 && proc->adjType == 0) || proc->adjType != 0)
            AddToCol(p, proc->adjType, proc->amt);
    }
}

void ReportGenerator::sumRoles(Clinic *c) {
    for (int i = 0; i < c->providers.size(); i++) {
        std::string name = c->providers[i]->name;
        std::vector<double>* addInto;
        if(isHygienist(name)) {
            addInto = &c->HygienistTotals; 
        }
        else if (isDoctor(name)) {
            addInto = &c->doctorTotals; 
            //std::cout << "doctor" << name << c->name << "\t\n";
        }
        else {
            addInto = &c->otherTotals; 
        }
        for(int j = 0; j < adjTypes.size(); j++) {
            
            (*addInto)[j] += c->providers[i]->totals[j];
        }
    }

    //std::cout << "total" << c->name << c->doctorTotals[0] << std::endl;
}

void ReportGenerator::ProdAdjReport() {
    
    std::ofstream output(getPrefix() + "ProductionAdjustmentSummary.csv");

    //production adjustments
    
    //header
    output << std::setprecision(15) << "Clinic,Group,Provider,";
    for(int i = 0; i < numProdAdj + 1; i++) {
        output << std::setprecision(15) << "\" " << adjTypes[i] << '\"' << ',';
    }
    output << std::setprecision(15) << "Total Production Adjustments, Net Production,\n\n";
    std::vector<double> grandTotal = std::vector<double>(adjTypes.size(), 0.0);
    double grandTotalProdAdj = 0.0;
    double grandTotalNetProd = 0.0;
    for (auto it = clinics.begin(); it != clinics.end(); ++it) {

        output << std::setprecision(15) << it->first << ',';
        Clinic* currentClinic = it->second;

        //print doctors
        bool first = true;
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isDoctor(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << "Doctors,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            for(int i = 0; i < numProdAdj + 1; i++) {
                output << std::setprecision(15) << currentProv->totals[i] << ',';
            }
            output << std::setprecision(15) << GetProdAdjSubtotal(currentProv) << ',';
            output << std::setprecision(15) << GetNetProduction(currentProv) << ',';
            output << std::setprecision(15) << '\n';
        }
        output << std::setprecision(15) << ",,Doctors Total" << ',';
        for(int i = 0; i < numProdAdj + 1; i++) {
            output << std::setprecision(15) << currentClinic->doctorTotals[i] << ',';
        }
        output << std::setprecision(15) << GetRoleTotalProdAdj(currentClinic, 0) << ',';
        output << std::setprecision(15) << GetRoleNetProd(currentClinic, 0) << ',';

        output << std::setprecision(15) << "\n\n";
        first = true;


        //print Hygienists
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isHygienist(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << ",Hygienists,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            for(int i = 0; i < numProdAdj + 1; i++) {
                output << std::setprecision(15) << currentProv->totals[i] << ',';
            }
            output << std::setprecision(15) << GetProdAdjSubtotal(currentProv) << ',';
            output << std::setprecision(15) << GetNetProduction(currentProv) << ',';
            output << std::setprecision(15) << '\n';
        }
        output << std::setprecision(15) << ",,Hygienists Total" << ',';
        for(int i = 0; i < numProdAdj + 1; i++) {
            output << std::setprecision(15) << currentClinic->HygienistTotals[i] << ',';
        }
        output << std::setprecision(15) << GetRoleTotalProdAdj(currentClinic, 1) << ',';
        output << std::setprecision(15) << GetRoleNetProd(currentClinic, 1) << ',';
        output << std::setprecision(15) << "\n";

        output << std::setprecision(15) << ",,Doctors + Hygienists Total,";
        for(int i = 0; i < numProdAdj + 1; i++) {
            int index = i;
            output << std::setprecision(15) << (currentClinic->HygienistTotals[index] + currentClinic->doctorTotals[index]) << ',';
        }
        output << std::setprecision(15) << (GetRoleTotalProdAdj(currentClinic, 1) + GetRoleTotalProdAdj(currentClinic, 0)) << ',';
        output << std::setprecision(15) << (GetRoleNetProd(currentClinic, 1) + GetRoleNetProd(currentClinic, 0)) << ',';
        output << std::setprecision(15) << "\n\n";


        //print others
        output << std::setprecision(15) << ",Others,";
        Provider* currentProv;

        //suspended
        std::string name = it->first + " SUSPENDED CREDITS";
        output << std::setprecision(15) << name << ',';
        auto it2 = prov.find(name);
        if (it2 != prov.end()) 
            currentProv = it2->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        for(int i = 0; i < numProdAdj + 1; i++) {
            output << std::setprecision(15) << currentProv->totals[i] << ',';
        }
        output << std::setprecision(15) << GetProdAdjSubtotal(currentProv) << ',';
        output << std::setprecision(15) << GetNetProduction(currentProv) << ',';
        output << std::setprecision(15) << '\n';
        //PRV
        name = it->first + "PRV";
        if(strncmp("CPD", it->first.c_str(), 3) == 0)
            name = "PEDOPRV";
        output << std::setprecision(15) << ",," << name << ',';
        auto it3 = prov.find(name);
        if (it3 != prov.end()) 
            currentProv = it3->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        for(int i = 0; i < numProdAdj + 1; i++) {
            output << std::setprecision(15) << currentProv->totals[i] << ',';
        }
        output << std::setprecision(15) << GetProdAdjSubtotal(currentProv) << ',';
        output << std::setprecision(15) << GetNetProduction(currentProv) << ',';
        output << std::setprecision(15) << "\n";

        output << std::setprecision(15) << ",,Clinic Total,";
        double amt;
        for(int i = 0; i < numProdAdj + 1; i++) {
            int index = i;
            
            amt = (currentClinic->otherTotals[index] + currentClinic->HygienistTotals[index] + 
                currentClinic->doctorTotals[index]);
            output << std::setprecision(15) << amt << ',';
            grandTotal[index] += amt;
        }
        amt = (GetRoleTotalProdAdj(currentClinic, 2) + GetRoleTotalProdAdj(currentClinic, 1) + 
            GetRoleTotalProdAdj(currentClinic, 0));
        grandTotalProdAdj += amt;
        output << std::setprecision(15) << amt << ',';

        amt = (GetRoleNetProd(currentClinic, 2) + GetRoleNetProd(currentClinic, 1) + 
            GetRoleNetProd(currentClinic, 0));
        grandTotalNetProd += amt;
        output << std::setprecision(15) << amt << ',';
        output << std::setprecision(15) << "\n\n\n";
    }

    output << std::setprecision(15) << "ALL,,Grand Total,";
    for(int i = 0; i < numProdAdj + 1; i++) {
        int index = i;
        output << std::setprecision(15) << grandTotal[index] << ',';
    }
    output << std::setprecision(15) << grandTotalProdAdj << ',';
    output << std::setprecision(15) << grandTotalNetProd << ',';
    output << std::setprecision(15) << '\n';

    output.close();
}

void ReportGenerator::ColAdjReport() {
    std::ofstream output(getPrefix() + "CollectionAdjustmentSummary.csv");

    //production adjustments
    
    //header
    output << std::setprecision(15) << "Clinic,Group,Provider,";
    for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
        output << std::setprecision(15) << "\" " << adjTypes[i] << '\"' << ',';
    }
    output << std::setprecision(15) << "Total Collection & Group Adjustments,\n\n";
    std::vector<double> grandTotal = std::vector<double>(adjTypes.size(), 0.0);
    double grandTotalColAdj = 0.0;
    for (auto it = clinics.begin(); it != clinics.end(); ++it) {

        output << std::setprecision(15) << it->first << ',';
        Clinic* currentClinic = it->second;

        //print doctors
        bool first = true;
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isDoctor(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << "Doctors,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
                output << std::setprecision(15) << currentProv->totals[i] << ',';
            }
            output << std::setprecision(15) << GetCollecAdjSubtotal(currentProv) << ',';
            output << std::setprecision(15) << '\n';
        }
        output << std::setprecision(15) << ",,Doctors Total" << ',';
        for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
            output << std::setprecision(15) << currentClinic->doctorTotals[i] << ',';
        }
        output << std::setprecision(15) << GetRoleColAdj(currentClinic, 0) << ',';

        output << std::setprecision(15) << "\n\n";
        first = true;


        //print Hygienists
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isHygienist(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << ",Hygienists,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
                output << std::setprecision(15) << currentProv->totals[i] << ',';
            }
            output << std::setprecision(15) << GetCollecAdjSubtotal(currentProv) << ',';
            output << std::setprecision(15) << '\n';
        }
        output << std::setprecision(15) << ",,Hygienists Total" << ',';
        for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
            output << std::setprecision(15) << currentClinic->HygienistTotals[i] << ',';
        }
        output << std::setprecision(15) << GetRoleColAdj(currentClinic, 1) << ',';
        output << std::setprecision(15) << "\n";

        output << std::setprecision(15) << ",,Doctors + Hygienists Total,";
        for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
            int index = i;
            output << std::setprecision(15) << (currentClinic->HygienistTotals[index] + currentClinic->doctorTotals[index]) << ',';
        }
        output << std::setprecision(15) << (GetRoleColAdj(currentClinic, 1) + GetRoleColAdj(currentClinic, 0)) << ',';
        output << std::setprecision(15) << "\n\n";


        //print others
        output << std::setprecision(15) << ",Others,";
        Provider* currentProv;

        //suspended
        std::string name = it->first + " SUSPENDED CREDITS";
        output << std::setprecision(15) << name << ',';
        auto it2 = prov.find(name);
        if (it2 != prov.end()) 
            currentProv = it2->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
            output << std::setprecision(15) << currentProv->totals[i] << ',';
        }
        output << std::setprecision(15) << GetCollecAdjSubtotal(currentProv) << ',';
        output << std::setprecision(15) << '\n';
        //PRV
        name = it->first + "PRV";
        if(strncmp("CPD", it->first.c_str(), 3) == 0)
            name = "PEDOPRV";
        output << std::setprecision(15) << ",," << name << ',';
        auto it3 = prov.find(name);
        if (it3 != prov.end()) 
            currentProv = it3->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
            output << std::setprecision(15) << currentProv->totals[i] << ',';
        }
        output << std::setprecision(15) << GetCollecAdjSubtotal(currentProv) << ',';
        output << std::setprecision(15) << "\n";

        output << std::setprecision(15) << ",,Clinic Total,";
        double amt;
        for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
            int index = i;
            amt = (currentClinic->otherTotals[index] + currentClinic->HygienistTotals[index] + 
                currentClinic->doctorTotals[index]);
            output << std::setprecision(15) << amt << ',';
            grandTotal[index] += amt;
        }
        

        amt = (GetRoleColAdj(currentClinic, 2) + GetRoleColAdj(currentClinic, 1) + 
            GetRoleColAdj(currentClinic, 0));
        grandTotalColAdj += amt;
        output << std::setprecision(15) << amt << ',';
        output << std::setprecision(15) << "\n\n\n";
    }

    output << std::setprecision(15) << "ALL,,Grand Total,";
    for(int i = numProdAdj + 1; i < numProdAdj + numColAdj + 1; i++) {
        int index = i;
        output << std::setprecision(15) << grandTotal[index] << ',';
    }
    output << std::setprecision(15) << grandTotalColAdj << ',';
    output << std::setprecision(15) << '\n';

    output.close();
}

void ReportGenerator::PaymentReport() {
    std::ofstream output(getPrefix() + "PaymentSummary.csv");
    
    //header
    output << std::setprecision(15) << "Clinic,Group,Provider,";
    for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
        output << std::setprecision(15) << "\" " << adjTypes[i] << '\"' << ',';
    }
    output << std::setprecision(15) << "Total Payments,\n\n";
    std::vector<double> grandTotal = std::vector<double>(adjTypes.size(), 0.0);
    double grandTotalPayments = 0.0;
    for (auto it = clinics.begin(); it != clinics.end(); ++it) {

        output << std::setprecision(15) << it->first << ',';
        Clinic* currentClinic = it->second;

        //print doctors
        bool first = true;
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isDoctor(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << "Doctors,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
                output << std::setprecision(15) << currentProv->totals[i] << ',';
            }
            output << std::setprecision(15) << GetPaySubtotal(currentProv) << ',';
            output << std::setprecision(15) << '\n';
        }
        output << std::setprecision(15) << ",,Doctors Total" << ',';
        for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
            output << std::setprecision(15) << currentClinic->doctorTotals[i] << ',';
        }
        output << std::setprecision(15) << GetRoleTotalPayments(currentClinic, 0) << ',';

        output << std::setprecision(15) << "\n\n";
        first = true;


        //print Hygienists
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isHygienist(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << ",Hygienists,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
                output << std::setprecision(15) << currentProv->totals[i] << ',';
            }
            output << std::setprecision(15) << GetPaySubtotal(currentProv) << ',';
            output << std::setprecision(15) << '\n';
        }
        output << std::setprecision(15) << ",,Hygienists Total" << ',';
        for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
            output << std::setprecision(15) << currentClinic->HygienistTotals[i] << ',';
        }
        output << std::setprecision(15) << GetRoleTotalPayments(currentClinic, 1) << ',';
        output << std::setprecision(15) << "\n";

        output << std::setprecision(15) << ",,Doctors + Hygienists Total,";
        for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
            int index = i;
            output << std::setprecision(15) << (currentClinic->HygienistTotals[index] + currentClinic->doctorTotals[index]) << ',';
        }
        output << std::setprecision(15) << (GetRoleTotalPayments(currentClinic, 1) + GetRoleTotalPayments(currentClinic, 0)) << ',';
        output << std::setprecision(15) << "\n\n";


        //print others
        output << std::setprecision(15) << ",Others,";
        Provider* currentProv;

        //suspended
        std::string name = it->first + " SUSPENDED CREDITS";
        output << std::setprecision(15) << name << ',';
        auto it2 = prov.find(name);
        if (it2 != prov.end()) 
            currentProv = it2->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
            output << std::setprecision(15) << currentProv->totals[i] << ',';
        }
        output << std::setprecision(15) << GetPaySubtotal(currentProv) << ',';
        output << std::setprecision(15) << '\n';
        //PRV
        name = it->first + "PRV";
        if(strncmp("CPD", it->first.c_str(), 3) == 0)
            name = "PEDOPRV";
        output << std::setprecision(15) << ",," << name << ',';
        auto it3 = prov.find(name);
        if (it3 != prov.end()) 
            currentProv = it3->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
            output << std::setprecision(15) << currentProv->totals[i] << ',';
        }
        output << std::setprecision(15) << GetPaySubtotal(currentProv) << ',';
        output << std::setprecision(15) << "\n";

        output << std::setprecision(15) << ",,Clinic Total,";
        double amt;
        for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
            int index = i;
            amt = (currentClinic->otherTotals[index] + currentClinic->HygienistTotals[index] + 
                currentClinic->doctorTotals[index]);
            output << std::setprecision(15) << amt << ',';
            grandTotal[index] += amt;
        }
        
        amt = (GetRoleTotalPayments(currentClinic, 2) + GetRoleTotalPayments(currentClinic, 1) + 
            GetRoleTotalPayments(currentClinic, 0));
        grandTotalPayments += amt;
        output << std::setprecision(15) << amt << ',';
        output << std::setprecision(15) << "\n\n\n";
    }

    output << std::setprecision(15) << "ALL,,Grand Total,";
    for(int i = numProdAdj + numColAdj + 1; i < adjTypes.size(); i++) {
        int index = i;
        output << std::setprecision(15) << grandTotal[index] << ',';
    }
    output << std::setprecision(15) << grandTotalPayments << ',';
    output << std::setprecision(15) << '\n';

    output.close();
}

void ReportGenerator::SummaryReport() {
    std::ofstream output(getPrefix() + "TotalSummary.csv");
    double grandGross = 0.0;
    double grandTotalProdAdj = 0.0;
    double grandTotalNetProd = 0.0;
    double grandTotalPayments = 0.0;
    double grandTotalColGroupAdj = 0.0;
    output << "Clinic,Group,Provider,Gross Production,Total Production Adjustments,Net Production,Total Payments,Total Collection & Group Adjustments,\n";
    for (auto it = clinics.begin(); it != clinics.end(); ++it) {

        output << std::setprecision(15) << it->first << ',';
        Clinic* currentClinic = it->second;
        double clinicGross = 0.0;
        double clinicTotalProdAdj = 0.0;
        double clinicTotalNetProd = 0.0;
        double clinicTotalPayments = 0.0;
        double clinicTotalColGroupAdj = 0.0;
        //print doctors
        bool first = true;
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isDoctor(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << "Doctors,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";

            clinicGross += currentProv->totals[0];
            clinicTotalProdAdj += currentProv->totalProdAdj;
            clinicTotalNetProd += currentProv->netProd;
            clinicTotalColGroupAdj += currentProv->totalColAndGroupAdj;
            clinicTotalPayments += currentProv->totalPayments;
            output << std::setprecision(15) << currentProv->name << ',';
            //print
            output << std::setprecision(15) << currentProv->totals[0] << ',' << currentProv->totalProdAdj << ',' << 
                currentProv->netProd << ',' << currentProv->totalPayments << ',' << currentProv->totalColAndGroupAdj << ',';
            output << std::setprecision(15) << '\n';
        }
        // output << std::setprecision(15) << ",,Doctors Total" << ',';
        // for(int i = 30; i < adjTypes.size(); i++) {
        //     output << std::setprecision(15) << currentClinic->doctorTotals[i] << ','; //change
        // }
        // output << std::setprecision(15) << GetRoleTotalPayments(currentClinic, 0) << ',';
        output << ",,Doctors Total," << currentClinic->doctorTotals[0] << ',' <<  GetRoleTotalProdAdj(currentClinic, 0) <<
            ',' << GetRoleNetProd(currentClinic, 0) << ',' << GetRoleTotalPayments(currentClinic, 0) << ',' << 
            GetRoleColAdj(currentClinic, 0);
        output << std::setprecision(15) << "\n\n";
        first = true;


        //print Hygienists
        for(int i = 0; i < currentClinic->providers.size(); i++) {
            Provider* currentProv = currentClinic->providers[i];
            if (!isHygienist(currentProv->name))
                continue;
            if(first) {
                output << std::setprecision(15) << ",Hygienists,";
                first = false;
            }
            else
                output << std::setprecision(15) << ",,";
            output << std::setprecision(15) << currentProv->name << ',';
            clinicGross += currentProv->totals[0];
            clinicTotalProdAdj += currentProv->totalProdAdj;
            clinicTotalNetProd += currentProv->netProd;
            clinicTotalColGroupAdj += currentProv->totalColAndGroupAdj;
            clinicTotalPayments += currentProv->totalPayments;
            //print
            //std::cout << currentProv->name << currentProv->totalProdAdj << "\t" << std::endl;
            output << std::setprecision(15) << currentProv->totals[0] << ',' << currentProv->totalProdAdj << ',' << 
                currentProv->netProd << ',' << currentProv->totalPayments << ',' << currentProv->totalColAndGroupAdj << ',';
            output << std::setprecision(15) << '\n';
        }
        output << ",,Hygienists Total," << currentClinic->HygienistTotals[0] << ',' <<  GetRoleTotalProdAdj(currentClinic, 1) <<
            ',' << GetRoleNetProd(currentClinic, 1) << ',' << GetRoleTotalPayments(currentClinic, 1) << ',' << 
            GetRoleColAdj(currentClinic, 1);
        output << std::setprecision(15) << "\n\n";

        output << std::setprecision(15) << ",,Doctors + Hygienists Total,";
        output << clinicGross << ',' <<  clinicTotalProdAdj << ',' << clinicTotalNetProd << ',' << clinicTotalPayments << ',' << 
            clinicTotalColGroupAdj;
        output << std::setprecision(15) << "\n\n";


        //print others
        output << std::setprecision(15) << ",Others,";
        Provider* currentProv;

        //suspended
        std::string name = it->first + " SUSPENDED CREDITS";
        output << std::setprecision(15) << name << ',';
        auto it2 = prov.find(name);
        if (it2 != prov.end()) 
            currentProv = it2->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        clinicGross += currentProv->totals[0];
        clinicTotalProdAdj += currentProv->totalProdAdj;
        clinicTotalNetProd += currentProv->netProd;
        clinicTotalColGroupAdj += currentProv->totalColAndGroupAdj;
        clinicTotalPayments += currentProv->totalPayments;
         output << std::setprecision(15) << currentProv->totals[0] << ',' << currentProv->totalProdAdj << ',' << 
                currentProv->netProd << ',' << currentProv->totalPayments << ',' << currentProv->totalColAndGroupAdj << ',';
            output << std::setprecision(15) << '\n';
        //PRV
        name = it->first + "PRV";
        if(strncmp("CPD", it->first.c_str(), 3) == 0)
            name = "PEDOPRV";
        output << std::setprecision(15) << ",," << name << ',';
        auto it3 = prov.find(name);
        if (it3 != prov.end()) 
            currentProv = it3->second;
        else {
            currentProv = CreateProvider(name);
            prov[name] = currentProv;
            AddProvider(currentClinic, currentProv);
        }
        clinicGross += currentProv->totals[0];
        clinicTotalProdAdj += currentProv->totalProdAdj;
        clinicTotalNetProd += currentProv->netProd;
        clinicTotalColGroupAdj += currentProv->totalColAndGroupAdj;
        clinicTotalPayments += currentProv->totalPayments;
         output << std::setprecision(15) << currentProv->totals[0] << ',' << currentProv->totalProdAdj << ',' << 
                currentProv->netProd << ',' << currentProv->totalPayments << ',' << currentProv->totalColAndGroupAdj << ',';
            output << std::setprecision(15) << '\n';

        output << std::setprecision(15) << ",,Clinic Total,";
        output << clinicGross << ',' <<  clinicTotalProdAdj << ',' << clinicTotalNetProd << ',' << clinicTotalPayments << ',' << 
            clinicTotalColGroupAdj;
        output << std::setprecision(15) << "\n\n";
        output << std::setprecision(15) << "\n\n\n";
        grandTotalProdAdj += clinicTotalProdAdj;
        grandTotalPayments += clinicTotalPayments;
        grandTotalNetProd += clinicTotalNetProd;
        grandTotalColGroupAdj += clinicTotalColGroupAdj;
        grandGross += clinicGross;
    }
    output << std::setprecision(15) << "All,,Grand Total," << grandGross << ',' << grandTotalProdAdj << ',' << 
        grandTotalNetProd << ',' << grandTotalPayments << ',' << grandTotalColGroupAdj << ",\n";
    output.close();
}

void ReportGenerator::AllReports() {
    ProdAdjReport();
    ColAdjReport();
    PaymentReport();
    SummaryReport();
}

void ReportGenerator::clearAll() {
    procs.clear();
    clinics.clear();
    prov.clear();
}

int main(int argc, char **argv) {
    
    if(argc != 4) {
        //input error, print usage
        std::cout << "wrong num args" << std::endl;
        return 1;
    }
    
    

    ReportGenerator rep = ReportGenerator(std::string(argv[1]));
    rep.AllReports();
    rep.clearAll();
    rep.indexAppAdj(std::string(argv[2]));
    rep.indexAppPMT(std::string(argv[3]));
    rep.indexProcedures();
    rep.AllReports();
    return 0;
}

#endif