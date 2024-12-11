#include "Simulation.h"
#include "Auxiliary.h"
#include "Action.h"
#include "sstream"
#include "fstream"

Simulation::Simulation(const string &configFilePath) 
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
    
    std::string lineN;
    while (getline(file, lineN))
    {
        vector<string> config = Auxiliary::parseArguments(lineN);
        if (config.empty()) 
            continue;
            
        if (config[0] == "settlement" && config.size() == 3 && !isSettlementExists(config[0]) && isInteger(config[2]))
        {
            string name = config[1];
            string num = config[2];
            settlements.push_back(new Settlement(name, numStringToSettlementType(num)));
        }
        else if (config[0] == "facility" && config.size() == 7 && isInteger(config[2]) && isInteger(config[3]) && isInteger(config[4]) && isInteger(config[5]) && isInteger(config[6]))
        {
            string name = config[1];
            string category = config[2];
            int price = stoi(config[3]);
            int numLifeScore = stoi(config[4]);
            int numEcoScore = stoi(config[5]);
            int numEnvScore = stoi(config[6]);
            addFacility(FacilityType(name, numStringToCategory(category), price, numLifeScore, numEcoScore, numEnvScore));
        }
        else if (config[0] == "plan" && config.size() == 3 && isSettlementExists(config[1]))
        {
            string name = config[1];
            string policyString = config[2];
            SelectionPolicy* policy = stringToSelectionPoliciy(policyString);
            if(policy != nullptr)
                addPlan(getSettlement(name), policy);
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
        if(a.empty())
            continue;    
        
        string &actionToDo = a[0];
        switch (findCommand(actionToDo)) {
            case 0: // "SimulateStep"
                if(a.size() == 2  && isInteger(a[1]))
                {
                int numOfSteps = std::stoi(a[1]);
                SimulateStep *stepAction = new SimulateStep(numOfSteps);
                stepAction->setInputSyntax(command); 
                stepAction->act(*this);
                addAction(stepAction);
                }
                break;
            case 1: // "AddPlan"
                if(a.size() == 3)
                {
                AddPlan *addPlanAction = new AddPlan(a[1], a[2]);
                addPlanAction->setInputSyntax(command); 
                addPlanAction->act(*this);
                addAction(addPlanAction);
                }
                break;
            case 2: // "AddSettlement"
                if(a.size() == 3 && (numStringToSettlementType(a[2]) != SettlementType::ERROR_INVAILD_INPUT))
                {
                SettlementType settlementType = numStringToSettlementType(a[2]);
                AddSettlement *addSettlementAction = new AddSettlement(a[1], settlementType);
                addSettlementAction->setInputSyntax(command); 
                addSettlementAction->act(*this);
                addAction(addSettlementAction);
                }
                break;
            case 3: // "AddFacility"
                if(a.size() == 7 && isInteger(a[3]) && isInteger(a[4]) && isInteger(a[5]) && isInteger(a[6]) && (numStringToCategory(a[2]) != FacilityCategory::ERROR_INVAILD_INPUT))
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
                if(a.size() == 2 && isInteger(a[1]))
                {
                PrintPlanStatus *printPlanStatusAction = new PrintPlanStatus(std::stoi(a[1]));
                printPlanStatusAction->setInputSyntax(command); 
                printPlanStatusAction->act(*this);
                addAction(printPlanStatusAction);
                }
                break;
            case 5: // "ChangePlanPolicy"
                if(a.size() == 3 && isInteger(a[1]))
                {
                ChangePlanPolicy *changePlanPolicyAction = new ChangePlanPolicy(std::stoi(a[1]), a[2]);
                changePlanPolicyAction->setInputSyntax(command); 
                changePlanPolicyAction->act(*this);
                addAction(changePlanPolicyAction);
                }
                break;
            case 6: // "PrintActionsLog"
                if(a.size() == 1)
                {  
                PrintActionsLog *printActionsLogAction = new PrintActionsLog();
                printActionsLogAction->setInputSyntax(command); 
                printActionsLogAction->act(*this);
                addAction(printActionsLogAction);
                }
                break;
            case 7: // "Close"
                if(a.size() == 1)
                {
                Close *CloseAction = new Close();
                CloseAction->setInputSyntax(command); 
                CloseAction->act(*this);
                addAction(CloseAction);
                }
                break;
            case 8: // "BackupSimulation"
                if(a.size() == 1)
                {
                BackupSimulation *backupSimulationAction = new BackupSimulation();
                backupSimulationAction->setInputSyntax(command); 
                backupSimulationAction->act(*this);
                addAction(backupSimulationAction);
                }
                break;
            case 9: // "RestoreSimulation"
                if(a.size() == 1)
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
    
    return *settlements[-1]; // The return settlements[-1] line is unreachable due to prior checks, included only for completeness.
}

Plan &Simulation::getPlan(const int planID) 
{
    if (0 <= planID && planID <= planCounter)
        return plans[planID];
    
    return plans[-1]; // The return plans[-1] line is unreachable due to prior checks, included only for completeness.
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

// Used in the start method to find the corresponding action.
int Simulation::findCommand(const string &command)
{
    for(size_t i = 0; i < commands.size(); i++) {
        if(command == commands[i])
            return i;
    }

    return -1; //COMMAND NOT FOUND
}

bool Simulation::printPlanStatus(const int i)
{
    if (0 <= i && i < planCounter)
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
    if((0 <= planId && planId < planCounter) && (newPolicy != getPlan(planId).getSelectionPolicy()->toString()))
    {
        string prevPolicy = getPlan(planId).getSelectionPolicy()->toString();
        SelectionPolicy *newPolicyPtr = stringToSelectionPoliciy(newPolicy);
        if(newPolicyPtr == nullptr)
            return false;
        
        getPlan(planId).setSelectionPolicy(newPolicyPtr);
        
        std::cout << "PlanID: " << std::to_string(planId) << "\n"
                << "previousPolicy: " << prevPolicy << "\n"
                << "newPolicy: " << newPolicyPtr->toString() << std::endl;
        
        return true;          
    }
    return false;
}

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
: commands(other.commands) // Copying because `commands` is const, and const members cannot be moved
, isRunning(other.isRunning)
, planCounter(other.planCounter)
, actionsLog(std::move(other.actionsLog))
, plans(std::move(other.plans)) 
, settlements(std::move(other.settlements))
, facilitiesOptions(std::move(other.facilitiesOptions))
{
    other.isRunning = false;
    other.planCounter = 0;
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

        plans.clear();
        plans = std::move(other.plans);

        for(size_t i = 0; i < settlements.size(); i++) 
        {
            delete settlements[i];
        }
        settlements.clear();    
        settlements = std::move(other.settlements);

        facilitiesOptions.clear();
        facilitiesOptions = std::move(other.facilitiesOptions);
    }
    other.isRunning = false;
    other.planCounter = 0;
    return *this;    
}

bool Simulation::isInteger(const string& str) {
    if(str.empty())
        return false;
        
    for(size_t i = 0; i < str.size(); i++)
    {
        if(!isdigit(str[i]))
            return false;
    }
    return true;
}