// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StatusEffectsWidget.h"
#include "ClassicGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PORFROYALE_API AClassicGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    AClassicGameModeBase();

    UStatusEffectsWidget* GetStatusEffectsWidget() { return m_pStatusEffectsWidget; }

protected:
    UPROPERTY(BlueprintReadWrite)
    UStatusEffectsWidget* m_pStatusEffectsWidget;
	
};
