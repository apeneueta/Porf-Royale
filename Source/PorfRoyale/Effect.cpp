// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect.h"

#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"

#include "PorfRoyale.h"
#include "ClassicGameModeBase.h"
#include "StatusEffectsWidget.h"

//-------------------------------------------------------------------------------------------------
// Status effects can be added to the character for a specified duration or indefinitely.  Status
// effects can effect the character in different ways (stun, slow, etc.).  Status effects should
// only be created and added to a character through APorfCharacterBase::AddStatusEffect().  The
// duration of the status effect is specified at this time.  If no time, is specified it defaults
// to -1.  -1 means that the status effect will remain on the character until explicitly removed by
// calling APorfCharacterBase::RemoveStatusEffect().  Status effects are marked as finished when
// their duration is over.
//-------------------------------------------------------------------------------------------------

// Sets default values
AEffect::AEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEffect::BeginPlay()
{
	Super::BeginPlay();
}

//-------------------------------------------------------------------------------------------------
// This initializes are effect.  This should always be called immediately after the class is
// constructed.  This will start the duration timer for the effect.
//      type: the type of effect this is=
//      duration: how long this effect will last.  If duration == -1, effect stays until explicitly removed
//-------------------------------------------------------------------------------------------------
void AEffect::Init(const int playerIndex, StatusEffects type, float duration)
{
    m_type = type;

    if (duration != -1.f)
    {
        GetWorldTimerManager().SetTimer(m_durationHandle, this, &AEffect::EndEffect, duration);
    }

    m_duration = duration;
    m_curr = m_duration;
    DisplayStatusEffectAsWidget(playerIndex);
}

//-------------------------------------------------------------------------------------------------
//  This decrements the remaining time of this status effect.
//-------------------------------------------------------------------------------------------------
void AEffect::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (m_curr > 0.f)
        m_curr -= DeltaTime;
}

//-------------------------------------------------------------------------------------------------
// This is currently only being used for debug purposes.  It displays the duration of the status
// effect on the screen using StatusEffectsWidget.
//      playerIndex: the index of the player to add the status effect
//-------------------------------------------------------------------------------------------------
void AEffect::DisplayStatusEffectAsWidget(const int playerIndex)
{
    AClassicGameModeBase* pGameMode = Cast<AClassicGameModeBase>(GetWorld()->GetAuthGameMode());

    if (pGameMode)
    {
        UStatusEffectsWidget* pWidget = pGameMode->GetStatusEffectsWidget();

        if (pWidget)
        {
            pWidget->AddStatusEffectToBox(playerIndex, m_duration, &m_curr);
        }
    }
    else
    {
        UE_LOG(StatusEffect, Warning, TEXT("Unable to get classic game mode."));
    }
}

//-------------------------------------------------------------------------------------------------
// This sets the effect to finished, which will notify other objects that this effect's duration
// is over.
//-------------------------------------------------------------------------------------------------
void AEffect::EndEffect()
{
    m_type = Finished;
}