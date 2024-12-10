#include "Action.h"
extern Simulation* backup;

BaseAction::BaseAction(const string errorMsg, const ActionStatus status)
    : inputSyntax() 
    , errorMsg(errorMsg)
    , status(status) 
    {}

ActionStatus BaseAction::getStatus() const
{
    return status;
}

string BaseAction::actionStatusToStr(const ActionStatus& status) const
{
    if (status == ActionStatus::COMPLETED)
        return "COMPLETED";
    
    if (status == ActionStatus::ERROR)
        return "ERROR";
    
    return "UNKNOWN"; //NOT FOUND    
}

// Sets the inputSyntax member variable to the command entered by the user.
void BaseAction::setInputSyntax(const string inputSyntax)
{
    this->inputSyntax = inputSyntax;
}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    std::cout << errorMsg << std::endl;
}

const string &BaseAction::getErrorMsg() const
{
    return errorMsg;
}

//----------------------------------------------------------------------

SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation)
{
    for (int i = 0; i < numOfSteps; i++)
    {
        simulation.step();
    }
    complete();
}

const string SimulateStep::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus()); 
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}

//----------------------------------------------------------------------

AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : BaseAction("Cannot create this plan", ActionStatus::ERROR) 
    , settlementName(settlementName)
    , selectionPolicy(selectionPolicy) 
    {}

void AddPlan::act(Simulation &simulation)
{
    SelectionPolicy* policy = simulation.stringToSelectionPoliciy(selectionPolicy);
    if(!(simulation.isSettlementExists(this->settlementName)) || policy == nullptr) {
        delete policy;
        error(getErrorMsg());
        return;    
    }

    Settlement &settlement = simulation.getSettlement(settlementName);
    simulation.addPlan(settlement, policy);
    complete();
}

const string AddPlan::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

AddPlan *AddPlan::clone() const
{
    return new AddPlan(*this);
}

//----------------------------------------------------------------------

AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
: BaseAction("Settlement already exists", ActionStatus::ERROR)
, settlementName(settlementName)
, settlementType(settlementType) 
{}

void AddSettlement::act(Simulation &simulation)
{
    Settlement* newSettlement = new Settlement(settlementName, settlementType);
    if(simulation.addSettlement(newSettlement)) 
    {
        complete();
        return;
    }
    
    delete newSettlement;
    error(getErrorMsg());
}

AddSettlement *AddSettlement::clone() const
{
    return new AddSettlement(*this);
}

const string AddSettlement::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory, const int price, const int lifeQualityScore, const int economyScore, const int environmentScore)
: BaseAction("Facility already exists", ActionStatus::ERROR)
, facilityName(facilityName)
, facilityCategory(facilityCategory)
, price(price)
, lifeQualityScore(lifeQualityScore)
, economyScore(economyScore)
, environmentScore(environmentScore)
{}

void AddFacility::act(Simulation &simulation)
{
    FacilityType newFacility(facilityName, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
    if(simulation.addFacility(newFacility))
    {
        complete();
        return;
    }
    error(getErrorMsg());
}

AddFacility *AddFacility::clone() const
{
    return new AddFacility(*this);
}

const string AddFacility::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

PrintPlanStatus::PrintPlanStatus(int planId)
: BaseAction("Plan doesn't exist", ActionStatus::ERROR)
, planId(planId)
{}

void PrintPlanStatus::act(Simulation &simulation)
{
    if (simulation.printPlanStatus(planId))
    {
        complete();
        return;
    }
    error(getErrorMsg());
}

PrintPlanStatus *PrintPlanStatus::clone() const
{
    return new PrintPlanStatus(*this);
}

const string PrintPlanStatus::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy)
: BaseAction("Cannot change selection policy", ActionStatus::ERROR)
, planId(planId)
, newPolicy(newPolicy)
{}

void ChangePlanPolicy::act(Simulation &simulation)
{
    if(simulation.changePlanPolicy(planId, newPolicy))
    {
        complete();
        return;
    }
    error(getErrorMsg());
}

ChangePlanPolicy *ChangePlanPolicy::clone() const
{
    return new ChangePlanPolicy(*this);
}

const string ChangePlanPolicy::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

PrintActionsLog::PrintActionsLog()
{
}

void PrintActionsLog::act(Simulation &simulation)
{
    simulation.printActionsLog();
    complete();
}

PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}

const string PrintActionsLog::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

Close::Close()
{}

void Close::act(Simulation &simulation)
{
    simulation.close();
    complete();
}

Close *Close::clone() const
{
    return new Close(*this);
}

const string Close::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

BackupSimulation::BackupSimulation()
{}

void BackupSimulation::act(Simulation &simulation)
{
    if(backup != nullptr)
        *backup = simulation;    
    else
        backup = new Simulation(simulation);

    complete();
}

BackupSimulation *BackupSimulation::clone() const
{
    return new BackupSimulation(*this);
}

const string BackupSimulation::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}

//----------------------------------------------------------------------

RestoreSimulation::RestoreSimulation()
: BaseAction("No backup available", ActionStatus::ERROR)
{}

void RestoreSimulation::act(Simulation &simulation)
{
    if(backup == nullptr) 
        error(getErrorMsg());
    else
        simulation = *backup;
    complete();
}

RestoreSimulation *RestoreSimulation::clone() const
{
    return new RestoreSimulation(*this);
}

const string RestoreSimulation::toString() const
{
    return inputSyntax + " " + actionStatusToStr(this->getStatus());
}
