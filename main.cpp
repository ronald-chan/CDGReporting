#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "row.h"
#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdio.h>
#include <algorithm>

class ReportGenerator {
private:
    std::unordered_map<std::string, std::vector<std::string> > clinics;
    std::unordered_map<std::string, std::vector<int> > prov;
    std::unordered_set<std::string> hyg;
    std::vector<Procedure*> procs;
    std::vector<std::string> adjTypes;
    std::string hygenistFile = "files/hygenists.csv";
    bool failed;

    void indexDaySheet(std::string daySheet);
    void loadHygenists();
public:
    ReportGenerator(std::string daySheet): failed(false) {
        adjTypes.push_back("Gross Production");
        adjTypes.push_back("-Ins Credit Adj");
        adjTypes.push_back("+Ins Debit Adj");
        adjTypes.push_back("-Professional Allowance");
        adjTypes.push_back("+Other Dr Production Adjustment");
        adjTypes.push_back("-Other Dr Production Adjustment");
        adjTypes.push_back("-Hygiene Adjustment - Hygienist");
        adjTypes.push_back("-Hygiene Adjustment - Dr.");
        adjTypes.push_back("-Pre-Conversion Credit");
        adjTypes.push_back("-Senior Citizen Courtesy");
        adjTypes.push_back("-Staff Courtesy");
        adjTypes.push_back("-Reverse Finance Charges");
        adjTypes.push_back("+Pre-Conversion Debit");
        adjTypes.push_back("-Pre-Conversion Credit Balance");
        adjTypes.push_back("-Reversal of Bank Fee");
        adjTypes.push_back("+Bank Fee");
        adjTypes.push_back("-Write Off");
        adjTypes.push_back("+Payment Transfer From");
        adjTypes.push_back("-Payment Transfer To");
        adjTypes.push_back("+Patient Refund - Check");
        adjTypes.push_back("+Patient Refund - Credit Card");
        adjTypes.push_back("+Insurance Refund");
        adjTypes.push_back("+Void Refund");
        adjTypes.push_back("+Returned Check");
        adjTypes.push_back("+Pt Initiated Credit Card Charge Back");
        adjTypes.push_back("-Transfer Balance From");
        adjTypes.push_back("+Transfer Balance To");
        adjTypes.push_back("-Charge Reduction");
        adjTypes.push_back("-Care Credit Discount");
        adjTypes.push_back("+Pre-Conversion Debit Balance");
        loadHygenists();
        this->indexDaySheet(daySheet);
    }
    void ProdAdjReport();
};

int GetAdjType(const std::string& desc, const std::vector<std::string>& adjTypes) {
    for(int i = 1; i < adjTypes.size(); i++) {
        if(std::strcmp(desc.c_str(), adjTypes[i].c_str()) == 0) {
            //std::cout << "adj matched" << std::endl;
            return i;
        }
    }
    return 0;
}

void ReportGenerator::loadHygenists() {
    std::ifstream reader;
    reader.open(hygenistFile);
    if (!reader.is_open()) {
        std::cout << "cannot open hygenist file in " << hygenistFile << std::endl;
        failed = true;
        return;
    }
    std::string curr;
    std::getline(reader, curr);
    while(!reader.eof()) {
        auto it = hyg.find(curr);
        if(it != hyg.end()) {
            std::cout << "duplicate name " << curr << " found in hygenist file, treating as identical" << std::endl;
        }
        hyg.insert(curr);
        std::getline(reader, curr);
    }
    // for(auto it = hyg.begin(); it != hyg.end(); it++) {
    //     std::cout << *it << std::endl;
    // }
    reader.close();
}

void ReportGenerator::indexDaySheet(std::string daySheet) {
    //set up file reader
    std::ifstream reader;
    reader.open(daySheet);
    if (!reader.is_open()) {
        //file existence error
        std::cout << "cannot open file " << daySheet << std::endl;
        failed = true;
        return;
    }
    std::string temp;
    std::string line;
    std::getline(reader, line);
    while(!reader.eof()) { //loop while hasn't hit eof
        Procedure *current = GenerateProcedure();
        
        std::stringstream s(line);
        std::string data;
        //skip to head of data
        for(int i = 0; i < 22; i++)
            std::getline(s, data, ',');

        //entry date
        std::getline(s, data, ',');
        current->entryDate = data;

        //proc date
        std::getline(s, data, ',');
        current->procDate = data;

        //name
        s.ignore(1); //ignore starting quote
        data.clear();
        std::getline(s, data, '\"');
        current->patName = data;
        s.ignore(1); //ignore comma

        //desc
        s.ignore(1); //ignore starting quote
        std::getline(s, data, '\"');
        current->procDesc = data;
        s.ignore(1); //ignore comma
        current->adjType = GetAdjType(data, adjTypes);

        //charges/credits
        std::getline(s, data, '\"'); //skip to quote
        bool charge = data.empty();
        std::getline(s, data, '\"'); //get numbers between quotes
        if(data[0] == '{')
            data = data.substr(5, data.length()-5);
        data.erase(std::remove(data.begin(), data.end(), ','), data.end());
        current->amt = std::stod(data);
        if(charge)
            s.ignore(2);
        else 
            s.ignore(1);

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
        current->chartNum = data.substr(1, data.length() - 2);

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
    }
    reader.close();
}

void ReportGenerator::ProdAdjReport() {
    
    for (int i = 0; i < procs.size(); i++) {
        
        auto it = prov.find(procs[i]->priProvider);
        if(it == prov.end()) {
            clinics[procs[i]->clinic].push_back(procs[i]->priProvider);
        }
        prov[procs[i]->priProvider].push_back(i);
    }

    std::vector<double> totals = std::vector<double>(30, 0.0);
    for(std::unordered_map<std::string, std::vector<std::string> >::iterator it = clinics.begin(); 
        it != clinics.end(); ++it) {

        std::cout << it->first << " Doctor Production" << std::endl;
        for(int i = 0; i < it->second.size(); i++) {
            std::string currProv = it->second[i];
            //skip hygenists && suspended
            if(hyg.find(currProv) != hyg.end() || currProv.find("SUSPENDED CREDITS") != currProv.npos)
                continue;
            std::cout << '\t' << currProv << std::endl;
            for(int j = 0; j < prov[currProv].size(); j++) {
                int index = prov[currProv][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }

            for(int k = 0; k < adjTypes.size(); k++) {
                std::printf("\t\t|\t%-50s:%10.2f", adjTypes[k].c_str(), totals[k]);
                totals[k] = 0.0;
                std::cout << std::endl;
            }
        } //doctor for

        std::cout << it->first << " Hygiene Production" << std::endl;
        for(int i = 0; i < it->second.size(); i++) {
            std::string currProv = it->second[i];
            //skip doctors && suspended
            if(hyg.find(currProv) == hyg.end() || currProv.find("SUSPENDED CREDITS") != currProv.npos) 
                continue;
            std::cout << '\t' << currProv << std::endl;
            for(int j = 0; j < prov[currProv].size(); j++) {
                int index = prov[currProv][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }

            for(int k = 0; k < adjTypes.size(); k++) {
                std::printf("\t\t|\t%-50s:%10.2f", adjTypes[k].c_str(), totals[k]);
                totals[k] = 0.0;
                std::cout << std::endl;
            }
        } //hygiene for

        std::cout << it->first << " Suspended Credits" << std::endl;
        std::string provName = it->first + " SUSPENDED CREDITS";
        auto iter = prov.find(provName);
        if(iter != prov.end()) {
            for(int j = 0; j < prov[provName].size(); j++) {
                int index = prov[provName][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }
        }
        for(int k = 0; k < adjTypes.size(); k++) {
            std::printf("\t\t|\t%-50s:%10.2f", adjTypes[k].c_str(), totals[k]);
            totals[k] = 0.0;
            std::cout << std::endl;
        }

    } // clinic for
}

void AdjReport();


int main(int argc, char **argv) {
    
    if(argc != 2) {
        //input error, print usage
        std::cout << "wrong num args" << std::endl;
        return 1;
    }
    
    

    ReportGenerator rep = ReportGenerator(std::string(argv[1]));
    rep.ProdAdjReport();
    return 0;
}