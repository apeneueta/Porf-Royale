// Fill out your copyright notice in the Description page of Project Settings.

#include "MathBlueprintFunctionLibrary.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

TArray<FVector> UMathBlueprintFunctionLibrary::GetVerticesFromBound(const FVector origin, const FVector extent)
{
	constexpr int kVerticesCountInBound = 8;

	//This is a mapping used to determine all the bound's vertex
	const FVector boundPointMapping[] =
	{
		FVector{ 1, 1, 1 },
		FVector{ 1, 1, -1},
		FVector{ 1, -1, 1},
		FVector{ 1, -1, -1},
		FVector{ -1, 1, 1 },
		FVector{ -1, 1, -1 },
		FVector{ -1, -1, 1 },
		FVector{ -1, -1, -1 }
	};

	//Calculateall the bound's vertex
	TArray<FVector> result;
	for (int i = 0; i < 8; ++i)
	{
		result.Add(origin + extent * boundPointMapping[i]);
	}

	return result;
}

void UMathBlueprintFunctionLibrary::WorldBoundToScreenBound(const APlayerController* pCameraOwner, const FVector origin, const FVector extent, FVector2D& screenMin, FVector2D& screenMax)
{
	//This is a mapping used to determine all the bound's vertex
	constexpr int kVerticesCountInBound = 8;
	const FVector boundPointMapping[kVerticesCountInBound] =
	{
		FVector{ 1, 1, 1 },
		FVector{ 1, 1, -1 },
		FVector{ 1, -1, 1 },
		FVector{ 1, -1, -1 },
		FVector{ -1, 1, 1 },
		FVector{ -1, 1, -1 },
		FVector{ -1, -1, 1 },
		FVector{ -1, -1, -1 }
	};

	//Calculate all the bound's vertex
	TArray<FVector> vertices;
	for (int i = 0; i < kVerticesCountInBound; ++i)
	{
		vertices.Add(origin + extent * boundPointMapping[i]);
	}

	//Convert world point to view point
	FVector2D screenPoints[kVerticesCountInBound];
	for (int i = 0; i < kVerticesCountInBound; ++i)
	{
		UGameplayStatics::ProjectWorldToScreen(pCameraOwner, vertices[i], screenPoints[i]);
	}

	//Find the min and max value for boundaryViewportPoints
	screenMin = screenPoints[0];
	screenMax = screenPoints[0];

	for (int i = 1; i < kVerticesCountInBound; ++i)
	{
		if (vertices[i].X < screenMin.X)
			screenMin.X = vertices[i].X;

		if (vertices[i].Y < screenMin.Y)
			screenMin.Y = vertices[i].Y;

		if (vertices[i].X > screenMax.X)
			screenMax.X = vertices[i].X;

		if (vertices[i].Y < screenMax.Y)
			screenMax.Y = vertices[i].Y;
	}


}
