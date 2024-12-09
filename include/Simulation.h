#pragma once
#include <string>
#include <vector>
#include "Facility.h"
#include "Plan.h"
#include "Settlement.h"
#include <iostream>
#include <stdexcept>
using std::string;
using std::vector;

class BaseAction;
class SelectionPolicy;
enum class ActionStatus;

class Simulation {
    public:
        Simulation(const string &configFilePath);
        void start();
        void addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy);
        void addAction(BaseAction *action);
        bool addSettlement(Settlement *settlement);
        bool addFacility(FacilityType facility);
        bool isSettlementExists(const string& settlementName);
        Settlement &getSettlement(const string &settlementName);
        Plan &getPlan(const int planID);
        void step();
        void close();
        void open();
        
        //NEW
        SelectionPolicy* stringToSelectionPoliciy(const string &name);
        FacilityCategory numStringToCategory(const string &num);
        SettlementType numStringToSettlementType(const string &num);
        int findCommand(const string& command);
        const vector<string> commands;
        bool printPlanStatus(const int i);
        void printActionsLog();
        bool changePlanPolicy(const int planId, const string &newPolicy);
        bool isInteger(const string& str);

        Simulation(const Simulation& other); 
        Simulation(Simulation&& other) noexcept;
        ~Simulation();
        Simulation& operator=(const Simulation& other);
        Simulation& operator=(Simulation&& other) noexcept;

    private:
        bool isRunning;
        int planCounter;
        vector<BaseAction*> actionsLog;
        vector<Plan> plans;
        vector<Settlement*> settlements;
        vector<FacilityType> facilitiesOptions;
};