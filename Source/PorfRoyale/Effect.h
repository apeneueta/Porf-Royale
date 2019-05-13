// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Effect.generated.h"

UENUM(BlueprintType)
enum StatusEffects
{
    Stun,
    Incapacitated,
	Invulnerable,
    None,
    Finished // This is used to notify that this effect is over and can be deleted 
};

UCLASS()
class PORFROYALE_API AEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	AEffect();
    void Init(const int playerIndex, StatusEffects type, float duration);

	virtual void Tick(float DeltaTime) override;
    const StatusEffects GetType() { return m_type; }
    bool IsFinished() { return m_type == StatusEffects::Finished; }

protected:
	virtual void BeginPlay() override;

private:
    void EndEffect();
    void DisplayStatusEffectAsWidget(const int playerIndex);

private:
    FTimerHandle m_durationHandle;
    StatusEffects m_type;
    float m_duration;
    float m_curr;
};
