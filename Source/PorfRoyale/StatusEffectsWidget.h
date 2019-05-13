// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/VerticalBox.h"
#include "StatusEffectsWidget.generated.h"

UCLASS()
class PORFROYALE_API UStatusEffectsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UStatusEffectsWidget(const FObjectInitializer& ObjectInitializer);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UVerticalBox* m_pP1VerticalBox;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UVerticalBox* m_pP2VerticalBox;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tunable Values")
	int m_maxEffects;
	
    TArray<TArray<float>> m_max;
    TArray<TArray<float*>> m_curr;

private:

public:
    void AddStatusEffectToBox(int playerIndex, float max, float* curr);

    UFUNCTION(BlueprintCallable)
    float GetRemainingTimeAsPercent(int playerIndex, int slotLocation);
};
