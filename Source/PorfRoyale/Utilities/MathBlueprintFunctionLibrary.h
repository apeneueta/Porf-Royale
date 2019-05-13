// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MathBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PORFROYALE_API UMathBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Math")
		static TArray<FVector> GetVerticesFromBound(const FVector origin, const FVector extent);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		static void WorldBoundToScreenBound(const APlayerController* pPlayerController, const FVector origin, const FVector extent, FVector2D& screenMin, FVector2D& screenMax);
};