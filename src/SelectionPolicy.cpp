#include "SelectionPolicy.h"

NaiveSelection:: NaiveSelection() :
    lastSelectedIndex(-1){}

const FacilityType& NaiveSelection:: selectFacility(const vector<FacilityType> &facilitiesOptions) 
{
    lastSelectedIndex = (lastSelectedIndex + 1) % static_cast<int>(facilitiesOptions.size());
    return facilitiesOptions[lastSelectedIndex];
}

const string NaiveSelection:: toString() const
{
       return "nve";
}


NaiveSelection* NaiveSelection:: clone() const
{
    return new NaiveSelection(*this);
}

EconomySelection:: EconomySelection() 
: lastSelectedIndex(-1){}

const FacilityType& EconomySelection:: selectFacility(const vector<FacilityType> &facilitiesOptions) 
{
    lastSelectedIndex = (lastSelectedIndex + 1) % static_cast<int>(facilitiesOptions.size());
    while (facilitiesOptions[lastSelectedIndex].getCategory() != FacilityCategory:: ECONOMY)
    {
        lastSelectedIndex = (lastSelectedIndex + 1) % static_cast<int>(facilitiesOptions.size());
    }
    return facilitiesOptions[lastSelectedIndex];
}
    
const string EconomySelection:: toString() const 
{
    return "eco";
}

EconomySelection* EconomySelection:: clone() const
{ 
    return new EconomySelection(*this);
}

SustainabilitySelection:: SustainabilitySelection() 
: lastSelectedIndex(-1){}

const FacilityType& SustainabilitySelection:: selectFacility(const vector<FacilityType> &facilitiesOptions) 
{
    lastSelectedIndex = (lastSelectedIndex + 1) % static_cast<int>(facilitiesOptions.size());
    while (facilitiesOptions[lastSelectedIndex].getCategory() != FacilityCategory:: ENVIRONMENT)
    {
        lastSelectedIndex = (lastSelectedIndex + 1) % static_cast<int>(facilitiesOptions.size());
    }
    return facilitiesOptions[lastSelectedIndex];
}

const string SustainabilitySelection:: toString() const 
{
    return "env";
}

SustainabilitySelection* SustainabilitySelection:: clone() const  
{ 
    return new SustainabilitySelection(*this);
}

BalancedSelection::BalancedSelection(int LifeQualityScore, int EconomyScore, int EnvironmentScore)
: LifeQualityScore(LifeQualityScore)
, EconomyScore(EconomyScore)
, EnvironmentScore(EnvironmentScore) 
{
}

const FacilityType& BalancedSelection::selectFacility(const vector<FacilityType> &facilitiesOptions)
{
    int a, b, c, max, min, currDist, minIndex;
    int prevDist = std::numeric_limits<int>::max();
    
    for(size_t i = 0; i < facilitiesOptions.size(); i++) 
    {
        a = LifeQualityScore + facilitiesOptions[i].getLifeQualityScore();
        b = EconomyScore + facilitiesOptions[i].getEconomyScore();
        c = EnvironmentScore + facilitiesOptions[i].getEnvironmentScore();

        max = std::max({a, b, c});
        min = std::min({a, b, c});
        currDist = max - min;
        
        if(currDist < prevDist) 
        {
            prevDist = currDist;
            minIndex = i;       
        }
    }

    LifeQualityScore += facilitiesOptions[minIndex].getLifeQualityScore();
    EconomyScore += facilitiesOptions[minIndex].getEconomyScore();
    EnvironmentScore += facilitiesOptions[minIndex].getEnvironmentScore();

    return facilitiesOptions[minIndex];
}

const string BalancedSelection::toString() const
{
    return "bal";
}

BalancedSelection *BalancedSelection::clone() const
{
    return new BalancedSelection(*this);;
}

// Sets scores when switching to a balanced policy.
void BalancedSelection::setBalancedPolicyScores(int newLifeScore, int newEcoScore, int newEnvScore)
{
    LifeQualityScore = newLifeScore;
    EconomyScore = newEcoScore;
    EnvironmentScore = newEnvScore;
}
