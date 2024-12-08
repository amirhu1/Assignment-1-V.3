#include "Simulation.h"
#include "Auxiliary.h"
#include "Action.h"
#include "sstream"
#include "fstream"

Simulation::Simulation(const string &configFilePath) //CHECK
: commands({"step", "plan", "settlement", "facility", "planStatus", "changePolicy", "log", "close", "backup", "restore"}) 
, isRunning(false)
, planCounter(0) 
, actionsLog()
, plans()
, settlements()
, facilitiesOptions()

{
    std::ifstream file(configFilePath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file: " << configFilePath << std::endl;
        return;
    }
    
    // if (file.peek() == std::ifstream::traits_type::eof())  CHEACK CASE OF EMPTY FILE
    // {
    //     std::cerr << "Error: Config file is empty!" << std::endl;
    //     file.close();
    //     return;
    // }
    
    std::string lineN;
    while (getline(file, lineN))
    {

        vector<string> con = Auxiliary::parseArguments(lineN);
        if (con.size() == 0)
        {
            continue;
            ;
        }

        if (con[0] == "settlement" && con.size() == 3)
        {
            string name1 = con[1];
            string num = con[2];
            settlements.push_back(new Settlement(name1, numStringToSettlementType(num)));
        }
        else if (con[0] == "facility" && con.size() == 7)
        {
            string name1 = con[1];
            string cat = con[2];
            int price = stoi(con[3]);
            int numLifeScore = stoi(con[4]);
            int numEcoScore = stoi(con[5]);
            int numEnvScore = stoi(con[6]);
            addFacility(FacilityType(name1, numStringToCategory(cat), price, numLifeScore, numEcoScore, numEnvScore));
        }
        else if (con[0] == "plan" && con.size() == 3)
        {
            string name1 = con[1];
            string policyString = con[2];
            addPlan(getSettlement(name1), stringToSelectionPoliciy(policyString));
        }
    }
}

void Simulation::start()
{
    open();
    std::cout << "The simulation has started" << std::endl;
    string command;

    while (isRunning)
    {
        std::getline(std::cin, command);
        vector<string> a = Auxiliary::parseArguments(command);
        string &actionToDo = a[0];

        switch (findCommand(actionToDo)) {
            case 0: // "SimulateStep"
                {
                int numOfSteps = std::stoi(a[1]);
                SimulateStep *stepAction = new SimulateStep(numOfSteps);
                stepAction->setInputSyntax(command); 
                stepAction->act(*this);
                addAction(stepAction);
                }
                break;
            case 1: // "AddPlan"
                {
                AddPlan *addPlanAction = new AddPlan(a[1], a[2]);
                addPlanAction->setInputSyntax(command); 
                addPlanAction->act(*this);
                addAction(addPlanAction);
                }
                break;
            case 2: // "AddSettlement"
                {
                SettlementType settlementType = numStringToSettlementType(a[2]);
                AddSettlement *addSettlementAction = new AddSettlement(a[1], settlementType);
                addSettlementAction->setInputSyntax(command); 
                addSettlementAction->act(*this);
                addAction(addSettlementAction);
                }
                break;
            case 3: // "AddFacility"
                {
                FacilityCategory facilityCategory = numStringToCategory(a[2]);
                int price = std::stoi(a[3]);
                int lifeQualityScore = std::stoi(a[4]);
                int economyScore = std::stoi(a[5]);
                int environmentScore = std::stoi(a[6]);
                AddFacility *addFacilityAction = new AddFacility(a[1],facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
                addFacilityAction->setInputSyntax(command); 
                addFacilityAction->act(*this);
                addAction(addFacilityAction);
                }
                break;
            case 4: // "PrintPlanStatus"
                {
                PrintPlanStatus *printPlanStatusAction = new PrintPlanStatus(std::stoi(a[1]));
                printPlanStatusAction->setInputSyntax(command); 
                printPlanStatusAction->act(*this);
                addAction(printPlanStatusAction);
                }
                break;
            case 5: // "ChangePlanPolicy"
                {
                ChangePlanPolicy *changePlanPolicyAction = new ChangePlanPolicy(std::stoi(a[1]), a[2]);
                changePlanPolicyAction->setInputSyntax(command); 
                changePlanPolicyAction->act(*this);
                addAction(changePlanPolicyAction);
                }
                break;
            case 6: // "PrintActionsLog"
                {
                PrintActionsLog *printActionsLogAction = new PrintActionsLog();
                printActionsLogAction->setInputSyntax(command); 
                printActionsLogAction->act(*this);
                addAction(printActionsLogAction);
                }
                break;
            case 7: // "Close"
                {
                Close *CloseAction = new Close();
                CloseAction->setInputSyntax(command); 
                CloseAction->act(*this);
                addAction(CloseAction);
                }
                break;
            case 8: // "BackupSimulation"
                {
                BackupSimulation *backupSimulationAction = new BackupSimulation();
                backupSimulationAction->setInputSyntax(command); 
                backupSimulationAction->act(*this);
                addAction(backupSimulationAction);
                }
                break;
            case 9: // "RestoreSimulation"
                {
                RestoreSimulation *restoreSimulationAction = new RestoreSimulation();
                restoreSimulationAction->setInputSyntax(command); 
                restoreSimulationAction->act(*this);
                addAction(restoreSimulationAction);
                }
                break;    
            default:
                break;
        }    
    }    
}    

void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy)
{
    if(selectionPolicy == nullptr)
        return;
    plans.emplace_back(planCounter, settlement, selectionPolicy, facilitiesOptions);
    planCounter++;
}

void Simulation::addAction(BaseAction *action)
{
    if(action == nullptr)
        return;
    actionsLog.push_back(action);
}

bool Simulation::addSettlement(Settlement *settlement)
{
    if(isSettlementExists(settlement->getName()))
        return false;
    
    settlements.push_back(settlement);
    return true;    
}

bool Simulation::addFacility(FacilityType facility)
{
    for (size_t i = 0; i < facilitiesOptions.size(); i++)
    {
        if (facilitiesOptions[i].getName() == facility.getName())
            return false;
    }

    facilitiesOptions.push_back(facility);
    return true;
}

bool Simulation::isSettlementExists(const string &settlementName)
{
    for (size_t i = 0; i < settlements.size(); i++)
    {
        if (settlements[i]->getName() == settlementName)
        {
            return true;
        }
    }
    return false;
}

Settlement &Simulation::getSettlement(const string &settlementName)
{
    for (size_t i = 0; i < settlements.size(); i++)
    {
        if (settlements[i]->getName() == settlementName)
        {
            return *settlements[i];     
        }
    }
    
    Settlement *invalidInput = nullptr; //INCASE OF INVALID INPUT, UB:Dereferencing a null pointer-PROGRAM WILL LIKELY CRASH
    return *invalidInput; 
}

Plan &Simulation::getPlan(const int planID) 
{
    if (0 <= planID && planID <= planCounter)
        return plans[planID];
    
    Plan *invalidInput = nullptr;
    return *invalidInput; //INCASE OF INVAILD INPUT, UB:Dereferencing a null pointer-PROGRAM WILL LIKELY CRASH
}

void Simulation::step()
{
    for (size_t i = 0; i < plans.size(); i++)
    {
        plans[i].step();
    }
}

void Simulation::close()
{
    isRunning = false;
    for(size_t i = 0; i < plans.size(); i++) 
    {
        std::cout << plans[i].toString() << std::endl;
    }

}

void Simulation::open()
{
    isRunning = true;
}

SelectionPolicy* Simulation::stringToSelectionPoliciy(const string &name)
{
    SelectionPolicy *policy = nullptr;
    
    if (name == "nve")   
        policy = new NaiveSelection();
    else if (name == "bal")
             policy = new BalancedSelection(0, 0, 0);
        else if (name == "eco")
                policy = new EconomySelection();
            else if (name == "env")
                    policy = new SustainabilitySelection();
    return policy;
}

FacilityCategory Simulation::numStringToCategory(const string &num)
{
    if (num == "0")   
        return FacilityCategory::LIFE_QUALITY;
    else if (num == "1")
            return FacilityCategory::ECONOMY;
        else if (num == "2")
                return FacilityCategory::ENVIRONMENT;
            else
                return FacilityCategory::ERROR_INVAILD_INPUT;; //INCASE OF INVAILD INPUT
}

SettlementType Simulation::numStringToSettlementType(const string &num)
{  
    if (num == "0")   
        return SettlementType::VILLAGE;
    else if (num == "1")
            return SettlementType::CITY;
        else if (num == "2")
                return SettlementType::METROPOLIS;
            else
                return SettlementType::ERROR_INVAILD_INPUT;; //INCASE OF INVAILD INPUT
}

int Simulation::findCommand(const string &command)
{
    for(size_t i = 0; i < commands.size(); i++) {
        if(command == commands[i])
            return i;
    }

    return -1; //COMMAND NOT FOUND
}

// const int Simulation::getPlanCounter() const
// {
//     return planCounter;
// }

// const vector<BaseAction*>& Simulation::getActionsLog() const
// {
//     return actionsLog;
// }

// vector<Plan> &Simulation::getPlansList() 
// {
//     return plans;
//}

bool Simulation::printPlanStatus(const int i)
{
    if (0 <= i && i <= planCounter)
    {
        plans[i].printStatus();
        return true;
    }
    return false;
}

void Simulation::printActionsLog()
{
    for(size_t i = 0; i < actionsLog.size(); i++)
    {
        std::cout << actionsLog[i]->toString() << std::endl;    
    }
}

bool Simulation::changePlanPolicy(const int planId, const string &newPolicy)
{
    SelectionPolicy *newPolicyPtr = nullptr;
    if(newPolicy == "bal")
        newPolicyPtr = new BalancedSelection(getPlan(planId).getlifeQualityScore(), getPlan(planId).getEconomyScore(), getPlan(planId).getEnvironmentScore());        
    
    newPolicyPtr = stringToSelectionPoliciy(newPolicy);
    string prevPolicy = getPlan(planId).getSelectionPolicy()->toString();
    
    if(0 <= planId && planId <= planCounter && typeid(*newPolicyPtr) != typeid( *(getPlan(planId).getSelectionPolicy()) )) 
    {
        getPlan(planId).setSelectionPolicy(newPolicyPtr);
        std::cout << "PlanID: " << std::to_string(planId) << "\n"
                  << "previousPolicy: " << prevPolicy << "\n"
                  << "newPolicy: " << newPolicyPtr->toString() << std::endl;
        return true;          
    }
    delete newPolicyPtr;
    return false;
}

// string Simulation::actionStatusToStr(const ActionStatus &status) const
// {
//     if (status == ActionStatus::COMPLETED)
//         return "COMPLETED";
    
//     if (status == ActionStatus::ERROR)
//         return "ERROR";
    
//     return "UNKNOWN"; //NOT FOUND
// }

Simulation::Simulation(const Simulation &other)
: commands(other.commands)
, isRunning(other.isRunning)
, planCounter(other.planCounter)
, actionsLog()
, plans()
, settlements()
, facilitiesOptions()
{
    for(size_t i = 0; i < other.actionsLog.size(); i++)
    {
        actionsLog.push_back(other.actionsLog[i]->clone());
    }

    for(size_t i = 0; i < other.settlements.size(); i++)
    {
        settlements.push_back(new Settlement(*other.settlements[i]));
    }

    for(size_t i = 0; i < other.facilitiesOptions.size(); i++)
    {
        facilitiesOptions.emplace_back(other.facilitiesOptions[i]);
    }

    for(size_t i = 0; i < other.plans.size(); i++)
        {
        plans.emplace_back(other.plans[i], getSettlement(other.plans[i].getPlanSettlement().getName()), facilitiesOptions);
        }   
}

Simulation::Simulation(Simulation &&other) noexcept
: commands(other.commands)
, isRunning(other.isRunning)
, planCounter(other.planCounter)
, actionsLog(std::move(other.actionsLog))
, plans(std::move(other.plans)) 
, settlements(std::move(other.settlements))
, facilitiesOptions(std::move(other.facilitiesOptions))
{
    other.actionsLog.clear();
    other.plans.clear();
    other.settlements.clear();
    other.facilitiesOptions.clear();
}

Simulation::~Simulation()
{
    for(size_t i = 0; i < actionsLog.size(); i++) 
    {
        delete actionsLog[i];
    }
    actionsLog.clear();

    for(size_t i = 0; i < settlements.size(); i++) 
    {
        delete settlements[i];
    }
    settlements.clear();
}

Simulation &Simulation::operator=(const Simulation &other)
{
    if(&other != this) 
    {
        isRunning = other.isRunning;
        planCounter = other.planCounter;

        for(size_t i = 0; i < actionsLog.size(); i++) 
        {
            delete actionsLog[i];
        }
        actionsLog.clear();

        for(size_t i = 0; i < other.actionsLog.size(); i++) 
        {
            actionsLog.push_back(other.actionsLog[i]->clone());    
        }

        for(size_t i = 0; i < settlements.size(); i++) 
        {
            delete settlements[i];
        }
        settlements.clear();

        for(size_t i = 0; i < other.settlements.size(); i++) 
        {
            settlements.push_back(new Settlement(*other.settlements[i]));    
        }

        facilitiesOptions.clear();
        for(size_t i = 0; i < other.facilitiesOptions.size(); i++)
        {
            facilitiesOptions.emplace_back(other.facilitiesOptions[i]);
        }

        plans.clear();
        for(size_t i = 0; i < other.plans.size(); i++)
        {
        plans.emplace_back(other.plans[i], getSettlement(other.plans[i].getPlanSettlement().getName()), facilitiesOptions);
        }
    }
    
    return *this;
}

Simulation &Simulation::operator=(Simulation &&other) noexcept
{
    if(&other != this)
    {
        for(size_t i = 0; i < actionsLog.size(); i++) 
        {
            delete actionsLog[i];
        }
        actionsLog.clear();    
        actionsLog = std::move(other.actionsLog);
        other.actionsLog.clear();

        plans.clear();
        plans = std::move(other.plans);
        other.plans.clear();

        for(size_t i = 0; i < settlements.size(); i++) 
        {
            delete settlements[i];
        }
        settlements.clear();    
        settlements = std::move(other.settlements);
        other.settlements.clear();

        facilitiesOptions.clear();
        facilitiesOptions = std::move(other.facilitiesOptions);
        other.facilitiesOptions.clear();
    }
    return *this;    
}
