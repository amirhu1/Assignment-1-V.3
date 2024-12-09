#include "Plan.h"

Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy* selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId), settlement(settlement), selectionPolicy(selectionPolicy), status(PlanStatus::AVALIABLE), facilities(), underConstruction(), facilityOptions(facilityOptions),
      life_quality_score(0), economy_score(0), environment_score(0) {}

const int Plan::getlifeQualityScore() const
{
    return life_quality_score;
}

const int Plan::getEconomyScore() const
{
    return economy_score;
}

const int Plan::getEnvironmentScore() const
{
    return environment_score;
}

void Plan::setSelectionPolicy(SelectionPolicy* selectionPolicy)
{
    if (this->selectionPolicy != nullptr)  
    {                                       
        delete this->selectionPolicy;         
    }

    this->selectionPolicy = selectionPolicy;
}

void Plan::step()
{
    while(status == PlanStatus::AVALIABLE) 
    {
        Facility* newFacility = new Facility(selectionPolicy->selectFacility(facilityOptions), settlement.getName());
        addFacility(newFacility);

        if(reachedConstructionLimit())
            status = PlanStatus::BUSY;
    }

    for(size_t i = 0; i < underConstruction.size(); i++) 
    {
        if(underConstruction[i]->step() == FacilityStatus::OPERATIONAL) 
        {
            addFacility(underConstruction[i]);
            underConstruction.erase(underConstruction.begin() + i);
            i--;
        }
    }

    if(reachedConstructionLimit()) 
    {
        status = PlanStatus::BUSY;
    }
    else
        status = PlanStatus::AVALIABLE;
}

bool Plan::reachedConstructionLimit() const {
    return static_cast<int>(underConstruction.size()) == static_cast<int>(settlement.getType()) + 1;   
}

const int Plan::getId() const
{
    return plan_id;
}

const SelectionPolicy* Plan::getSelectionPolicy() const
{
    return selectionPolicy;
}

Plan::Plan(const Plan &other, const Settlement &thisSettlement, const vector<FacilityType>& thisFacilityOptions)
: plan_id(other.plan_id)
, settlement(thisSettlement)
, selectionPolicy(other.selectionPolicy->clone())
, status(other.status)
, facilities()
, underConstruction()
, facilityOptions(thisFacilityOptions)
, life_quality_score(other.life_quality_score)
, economy_score(other.economy_score)
, environment_score(other.environment_score)
{
    for(size_t i = 0; i < other.facilities.size(); i++) 
    {
        facilities.push_back(new Facility(*other.facilities[i]));
    }

    for(size_t i = 0; i < other.underConstruction.size(); i++) 
    {
        underConstruction.push_back(new Facility(*other.underConstruction[i]));
    }    
}

const Settlement &Plan::getPlanSettlement() const
{
    return settlement;
}

Plan::Plan(const Plan &other)
: plan_id(other.plan_id)
, settlement(other.settlement)
, selectionPolicy(other.selectionPolicy->clone())
, status(other.status)
, facilities()
, underConstruction()
, facilityOptions(other.facilityOptions)
, life_quality_score(other.life_quality_score)
, economy_score(other.economy_score)
, environment_score(other.environment_score)
{
    for(size_t i = 0; i < other.facilities.size(); i++) 
    {
        facilities.push_back(new Facility(*other.facilities[i]));
    }

    for(size_t i = 0; i < other.underConstruction.size(); i++) 
    {
        underConstruction.push_back(new Facility(*other.underConstruction[i]));
    }
}

Plan::Plan(Plan &&other) noexcept
: plan_id(other.plan_id)
, settlement(other.settlement)
, selectionPolicy(other.selectionPolicy)
, status(other.status)
, facilities(std::move(other.facilities))
, underConstruction(std::move(other.underConstruction))
, facilityOptions(other.facilityOptions)
, life_quality_score(other.life_quality_score)
, economy_score(other.economy_score)
, environment_score(other.environment_score) 
{
    other.selectionPolicy = nullptr; 
}

Plan::~Plan()
{
    for(size_t i = 0; i < facilities.size(); i++) 
    {
        delete facilities[i];
    }
    facilities.clear();

    for(size_t i = 0; i < underConstruction.size(); i++) 
    {
        delete underConstruction[i];
    }
    underConstruction.clear();

    delete selectionPolicy;
}

void Plan::printStatus()
{
    string str = "PlanID: " + std::to_string(plan_id) + "\n" + settlement.toString() + "\n";
    string statusStr;
    
    if (status == PlanStatus::AVALIABLE)
        statusStr = "AVAILABLE";
    else
        statusStr = "BUSY";
    
    str += "PlanStatus: " + statusStr + "\n" + 
           "SelectionPolicy: " + selectionPolicy->toString() + "\n" + 
           "LifeQualityScore: " + std::to_string(life_quality_score) + "\n" + 
           "EconomyScore: " + std::to_string(economy_score) + "\n" + 
           "EnvironmentScore: " + std::to_string(environment_score);
    
    std::cout << str << std::endl;

    for (size_t i = 0; i < facilities.size(); i++)
    {
        std::cout << facilities[i]->toString() << std::endl;; 
    }

    for (size_t i = 0; i < underConstruction.size(); i++)
    {
        std::cout << underConstruction[i]->toString() << std::endl;;
    }
}

const vector<Facility*>& Plan:: getFacilities() const
{
    return facilities;
}

void Plan::addFacility(Facility* facility)
{ 
    if (facility->getStatus() == FacilityStatus::OPERATIONAL)
    {
        facilities.push_back(facility);
        life_quality_score += facility->getLifeQualityScore();
        economy_score += facility->getEconomyScore();
        environment_score += facility->getEnvironmentScore();
    }
    else
    {
        underConstruction.push_back(facility);
    }
}

const string Plan::toString() const
{
    string str;
    str += "PlanID: " + std::to_string(plan_id) + "\n";
    str += settlement.toString() + "\n";
    str += "LifeQualityScore: " + std::to_string(life_quality_score) + "\n";
    str += "EconomyScore: " + std::to_string(economy_score) + "\n";
    str += "EnvironmentScore: " + std::to_string(environment_score) + "\n";
    return str;
}