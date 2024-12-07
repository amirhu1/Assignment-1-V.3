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
        Simulation(const string &configFilePath); //CHEACK
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
        const int getPlanCounter() const;
        //const vector<BaseAction*>& getActionsLog() const;
        //vector<Plan>& getPlansList();
        void printPlanStatus(const int i);
        void printActionsLog();
        string actionStatusToStr(const ActionStatus& status) const;

        Simulation(const Simulation& other); 
        Simulation(Simulation&& other) noexcept;
        ~Simulation();
        Simulation& operator=(const Simulation& other);
        Simulation& operator=(Simulation&& other) noexcept;

    private:
        bool isRunning;
        int planCounter; //For assigning unique plan IDs
        vector<BaseAction*> actionsLog;
        vector<Plan> plans;
        vector<Settlement*> settlements;
        vector<FacilityType> facilitiesOptions;
};