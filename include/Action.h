#pragma once
#include <string>
#include <vector>
#include "Simulation.h"
#include <iostream>
enum class SettlementType;
enum class FacilityCategory;
using std::string;
using std::vector;

enum class ActionStatus{
    COMPLETED, ERROR
};

class BaseAction{
    public:
        BaseAction() = default;
        BaseAction(const string errorMsg, const ActionStatus status);
        ActionStatus getStatus() const;
        virtual void act(Simulation& simulation)=0;
        virtual const string toString() const=0;
        virtual BaseAction* clone() const = 0;
        virtual ~BaseAction() = default;
        
        string actionStatusToStr(const ActionStatus& status) const;
        void setInputSyntax(const string inputSyntax);

    protected:
        void complete();
        void error(string errorMsg);
        const string &getErrorMsg() const;
        
        string inputSyntax = ""; // Stores the command entered by the user for use in the toString method.

    private:
        string errorMsg = "";
        ActionStatus status = ActionStatus::ERROR;
};

class SimulateStep : public BaseAction {

    public:
        SimulateStep(const int numOfSteps);
        void act(Simulation &simulation) override;
        const string toString() const override;
        SimulateStep *clone() const override;
    private:
        const int numOfSteps;
};

class AddPlan : public BaseAction {
    public:
        AddPlan(const string &settlementName, const string &selectionPolicy);
        void act(Simulation &simulation) override;
        const string toString() const override;
        AddPlan *clone() const override;
    private:
        const string settlementName;
        const string selectionPolicy;
};


class AddSettlement : public BaseAction {
    public:
        AddSettlement(const string &settlementName,SettlementType settlementType);
        void act(Simulation &simulation) override; 
        AddSettlement *clone() const override;
        const string toString() const override;
    private:
        const string settlementName;
        const SettlementType settlementType;
};

class AddFacility : public BaseAction {
    public:
        AddFacility(const string &facilityName, const FacilityCategory facilityCategory, const int price, const int lifeQualityScore, const int economyScore, const int environmentScore);
        void act(Simulation &simulation) override;
        AddFacility *clone() const override; 
        const string toString() const override; 
    private:
        const string facilityName;
        const FacilityCategory facilityCategory;
        const int price;
        const int lifeQualityScore;
        const int economyScore;
        const int environmentScore;

};

class PrintPlanStatus: public BaseAction {
    public:
        PrintPlanStatus(int planId);
        void act(Simulation &simulation) override;
        PrintPlanStatus *clone() const override;
        const string toString() const override;
    private:
        const int planId;
};


class ChangePlanPolicy : public BaseAction {
    public:
        ChangePlanPolicy(const int planId, const string &newPolicy);
        void act(Simulation &simulation) override;
        ChangePlanPolicy *clone() const override;
        const string toString() const override;
    private:
        const int planId;
        const string newPolicy;
};


class PrintActionsLog : public BaseAction {
    public:
        PrintActionsLog();
        void act(Simulation &simulation) override;
        PrintActionsLog *clone() const override;
        const string toString() const override;
    private:
};

class Close : public BaseAction {
    public:
        Close();
        void act(Simulation &simulation) override;
        Close *clone() const override;
        const string toString() const override;
    private:
};

class BackupSimulation : public BaseAction {
    public:
        BackupSimulation();
        void act(Simulation &simulation) override;
        BackupSimulation *clone() const override;
        const string toString() const override;
    private:
};


class RestoreSimulation : public BaseAction {
    public:
        RestoreSimulation();
        void act(Simulation &simulation) override;
        RestoreSimulation *clone() const override;
        const string toString() const override;
    private:
};