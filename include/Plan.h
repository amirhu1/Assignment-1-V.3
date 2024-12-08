#pragma once
#include <vector>
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
#include <string>
#include <iostream>
using std::vector;
using std::string;

enum class PlanStatus {
    AVALIABLE,
    BUSY,
};

class Plan {
    public:
        Plan(const int planId, const Settlement& settlement, SelectionPolicy* selectionPolicy, const vector<FacilityType> &facilityOptions);
        const int getlifeQualityScore() const;
        const int getEconomyScore() const;
        const int getEnvironmentScore() const;
        void setSelectionPolicy(SelectionPolicy* selectionPolicy);
        void step();
        void printStatus();
        const vector<Facility*>& getFacilities() const;
        void addFacility(Facility* facility);
        const string toString() const;
        
        //NEW
        bool reachedConstructionLimit() const;
        const int getId() const;
        const SelectionPolicy* getSelectionPolicy() const;
        Plan(const Plan &other, const Settlement &thisSettlement, const vector<FacilityType>& thisFacilityOptions); //NEW
        const Settlement& getPlanSettlement() const;

        Plan(const Plan& other); 
        Plan(Plan&& other) noexcept;
        ~Plan();
        Plan& operator=(const Plan& other) = delete;
        Plan& operator=(Plan&& other) = delete;
        
    private:
        int plan_id;
        const Settlement& settlement;
        SelectionPolicy* selectionPolicy;
        PlanStatus status;
        vector<Facility*> facilities;
        vector<Facility*> underConstruction;
        const vector<FacilityType>& facilityOptions;
        int life_quality_score, economy_score, environment_score;
};