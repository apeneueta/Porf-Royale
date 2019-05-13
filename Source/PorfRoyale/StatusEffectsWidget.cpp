// Fill out your copyright notice in the Description page of Project Settings.

#include "StatusEffectsWidget.h"

#include "PorfRoyale.h"

//-------------------------------------------------------------------------------------------------
// Creates two 2D arrays, first index is the player index, the second index is the corresponding
// slot in the vertical box widget.  One of the 2D arrays is for the current time remaining of a 
// status effect.  The other 2D array is the entire duration of the specific status effect.
//-------------------------------------------------------------------------------------------------
UStatusEffectsWidget::UStatusEffectsWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // TODO: Need to create a variable for number of players
    // Create arrays for player 1 and player 2
    m_max.Add(TArray<float>());
    m_max.Add(TArray<float>());

    // Create arrays for player 1 and player 2
    m_curr.Add(TArray<float*>());
    m_curr.Add(TArray<float*>());
}

//-------------------------------------------------------------------------------------------------
// This adds a status effect to the widget to be displayed on screen.
//      playerIndex: The index of the player
//      max: The starting duration the status effect will effect the player
//      curr: A pointer to the time remaining of the status effect
//-------------------------------------------------------------------------------------------------
void UStatusEffectsWidget::AddStatusEffectToBox(int playerIndex, float max, float* curr)
{
    // If we haven't reached max status effects add this status effect to the end of the array
	if (m_curr[playerIndex].Num() < m_maxEffects)
	{
		m_max[playerIndex].Add(max);
		m_curr[playerIndex].Add(curr);
	}

    // The array is full
	else
	{
        // Check if the array has a status effect has ended and replace it with the new one.
		for (int i{ 0 }; i < m_maxEffects; ++i)
		{
			if (*m_curr[playerIndex][i] <= 0.f)
			{
				m_max[playerIndex][i] = max;
				m_curr[playerIndex][i] = curr;
                return;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
//  This gets the remaining time of a status effect as a ratio at a given location.
//      playerIndex: The index of the player
//      slotLocation: The location of the slot to get status effect.
//      return: the ratio of the current time and the entire duration of a status effect. -1 means
//              index is null.
//-------------------------------------------------------------------------------------------------
float UStatusEffectsWidget::GetRemainingTimeAsPercent(int playerIndex, int slotLocation)
{
    if (slotLocation <= m_max[playerIndex].Num() - 1 && slotLocation <= m_curr[playerIndex].Num() - 1)
    {
        if (m_max[playerIndex][slotLocation] && m_curr[playerIndex][slotLocation])
        {
            return (*m_curr[playerIndex][slotLocation]) / (m_max[playerIndex][slotLocation]);
        }
    }

    return -1;
}