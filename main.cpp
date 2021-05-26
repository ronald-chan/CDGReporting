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
    std::vector<int> printOrder;
    std::string hygenistFile = "files/hygenists.csv";
    bool failed;

    void indexDaySheet(std::string daySheet);
    void loadHygenists();
    void printHeader(std::fstream& out, std::string& provName);
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
        printOrder.push_back(8);
        printOrder.push_back(11);
        printOrder.push_back(12);
        printOrder.push_back(13);
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
        printOrder.push_back(29);
        printOrder.push_back(30);
        printOrder.push_back(31);
        printOrder.push_back(32);
        printOrder.push_back(33);
        printOrder.push_back(34);
        printOrder.push_back(35);
        printOrder.push_back(36);

        loadHygenists();
        this->indexDaySheet(daySheet);
    }
    void ProdAdjReport();
};

int GetAdjType(const std::string& desc, const std::vector<std::string>& adjTypes) {
    for(int i = 1; i < adjTypes.size(); i++) {
        if(std::strncmp(desc.c_str(), adjTypes[i].c_str(), adjTypes[i].length()) == 0) {
            //if (i >= 30)
                //std::cout << "payment matched" << std::endl;
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

void ReportGenerator::printHeader(std::fstream& out, std::string& provName) {
    out << "," << provName << ",,,Production Adjustments,";
    for(int k = 1; k < printOrder.size(); k++) {
        //out << '\'' << adjTypes[k] << '\'' <<  ',';
        out << ',';
        if(k == 8)
            out << "Collection Adjustments,";
        else if (k == 29)
            out << "Payment Types,";
    }
    
    out << "\n,,,";
    for(int k = 0; k < printOrder.size(); k++) {
        //out << '\'' << adjTypes[k] << '\'' <<  ',';
        out << ' ' << adjTypes[printOrder[k]] << ',';
        if(k == 8 || k == 29)
            out << ',';
    }
    out << "\n";
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
    std::fstream output;
    std::vector<double> totals = std::vector<double>(37, 0.0);
    int counter = 0;
    for(std::unordered_map<std::string, std::vector<std::string> >::iterator it = clinics.begin(); 
        it != clinics.end(); ++it) {
        output.open(it->first + "summary.csv");
        output << it->first << " Doctor Production,\n\n";
        
        double roleProdSubtotal = 0.0;
        double rolePaySubtotal = 0.0;

        double clinicProdSubtotal = 0.0;
        double clinicPaySubtotal = 0.0;

        for(int i = 0; i < it->second.size(); i++) {
            std::string currProv = it->second[i];
            //skip hygenists && suspended
            if(hyg.find(currProv) != hyg.end() || currProv.find("SUSPENDED CREDITS") != currProv.npos 
                || currProv.find("PRV") != currProv.npos)
                continue;

            //Add each procedure to category total
            for(int j = 0; j < prov[currProv].size(); j++) {
                int index = prov[currProv][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }

            //print production
            printHeader(output, currProv);
            double subtotal = 0.0;
            double prod = 0.0;
            output << ",,,";
            for(int k = 0; k < 30; k++) {
                output << totals[printOrder[k]] << ',';
                subtotal += totals[printOrder[k]];
                totals[printOrder[k]] = 0.0;
                if(k == 8 || k == 29)
                    output << ',';
            }

            prod = subtotal;
            subtotal = 0.0;

            for(int k = 30; k < adjTypes.size(); k++) {
                output << totals[printOrder[k]] << ',';
                subtotal += totals[printOrder[k]];
                totals[printOrder[k]] = 0.0;
            }
            output << "\n\n,,Production subtotal:," << prod << ",\n";
            output << ",,Payments subtotal:," << subtotal << ",\n\n";

            roleProdSubtotal += prod;
            rolePaySubtotal += subtotal;

        } //doctor for

        output << ",Doctor Production Subtotal," << roleProdSubtotal << ",\n";
        output << ",Doctor Payments Subtotal," << rolePaySubtotal << ",\n";

        clinicPaySubtotal += rolePaySubtotal;
        clinicProdSubtotal += roleProdSubtotal;

        rolePaySubtotal = 0.0;
        roleProdSubtotal = 0.0;
        

        output << it->first << " Hygiene Production,\n\n";
        for(int i = 0; i < it->second.size(); i++) {
            std::string currProv = it->second[i];
            //skip doctors && suspended
            if(hyg.find(currProv) == hyg.end() || currProv.find("SUSPENDED CREDITS") != currProv.npos
                || currProv.find("PRV") != currProv.npos) 
                continue;
            output << ',' << currProv << ",\n";
            //Add each procedure to category total
            for(int j = 0; j < prov[currProv].size(); j++) {
                int index = prov[currProv][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }

            //print production
            printHeader(output, currProv);
            double subtotal = 0.0;
            double prod = 0.0;
            output << ",,,";
            for(int k = 0; k < 30; k++) {
                output << totals[printOrder[k]] << ',';
                subtotal += totals[printOrder[k]];
                totals[printOrder[k]] = 0.0;
                if(k == 8 || k == 29)
                    output << ',';
            }

            prod = subtotal;
            subtotal = 0.0;

            for(int k = 30; k < adjTypes.size(); k++) {
                output << totals[printOrder[k]] << ',';
                subtotal += totals[printOrder[k]];
                totals[printOrder[k]] = 0.0;
            }
            output << "\n\n,,Production subtotal:," << prod << ",\n";
            output << ",,Payments subtotal:," << subtotal << ",\n\n";

            roleProdSubtotal += prod;
            rolePaySubtotal += subtotal;

        } //hygiene for

        output << ",Hygiene Production Subtotal," << roleProdSubtotal << ",\n";
        output << ",Hygiene Payments Subtotal," << rolePaySubtotal << ",\n";

        clinicPaySubtotal += rolePaySubtotal;
        clinicProdSubtotal += roleProdSubtotal;

        output << "\n\nSpecial\n\n";
        std::string provName = it->first + " SUSPENDED CREDITS";
        printHeader(output, provName);
        auto iter = prov.find(provName);
        if(iter != prov.end()) {
            for(int j = 0; j < prov[provName].size(); j++) {
                int index = prov[provName][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }
        }
        output << ",,,";
        for(int k = 0; k < adjTypes.size(); k++) {
            output << totals[printOrder[k]] << ',';
            if (k < 30)
                clinicProdSubtotal += totals[printOrder[k]];
            else
                clinicPaySubtotal += totals[printOrder[k]];
            totals[printOrder[k]] = 0.0;
        }

        output << "\n\n";
        provName = it->first + "PRV";
        printHeader(output, provName);
        iter = prov.find(provName);
        if(iter != prov.end()) {
            for(int j = 0; j < prov[provName].size(); j++) {
                int index = prov[provName][j];
                if((procs[index]->amt >= 0 && procs[index]->adjType == 0) || procs[index]->adjType != 0)
                    totals[procs[index]->adjType] += procs[index]->amt;
            }
        }
        output << ",,,";
        for(int k = 0; k < adjTypes.size(); k++) {
            output << totals[printOrder[k]] << ',';
            if (k < 30)
                clinicProdSubtotal += totals[printOrder[k]];
            else
                clinicPaySubtotal += totals[printOrder[k]];
            totals[printOrder[k]] = 0.0;
                if(k == 8 || k == 29)
                    output << ',';
        }

        //Print grand total for clinic

        output << "\n\n\nProduction Grand Total for " << it->first << "," << clinicProdSubtotal << ",\n";
        output << "Payment Grand Total for " << it->first << "," << clinicPaySubtotal << ",";

        output.close();

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