// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PorfCharacterMovementComponent.generated.h"

UCLASS()
class PORFROYALE_API UPorfCharacterMovementComponent : public UCharacterMovementComponent
{

	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:
// Aaron Peneueta
// Functions I have created or augmented
// For specific diffs I have done see word doc PorfDiffs

	void SetPushSpeed(float pushSpeed) { m_pushSpeed = pushSpeed; }
	float GetPushSpeed() { return m_pushSpeed; }
	void SetMovementSpeed(float movementSpeed) { m_movementSpeed = movementSpeed; }
	void SetRotationSpeedAndLerpRatio(float rotSpeed, float lerpRatio);
	void Snap(FVector direction);

// End Section Aaron Peneueta


    /** Returns true if the character is walking.  False, otherwise. */
	UFUNCTION(BlueprintCallable)
		bool IsWalking() { return m_isWalking; }

	void AddToMovementThisFrame(const FVector& movement) { m_movementThisFrame += movement; }
    void Push(FVector direction, float distance, float speed);
	
    void Stop();
	

protected:
// Aaron Peneueta
// Functions I have created or augmented
// For specific diffs I have done see word doc PorfDiffs

	/** The maximum speed value that a character can move. */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Tunable Values")
		float m_maxMovementSpeed = 200.f;
	/** The rate at which movement speed takes to reach max speed. */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Tunable Values")
		float m_rampingSpeed = 1.f;
	/** The ratio used to set the actor's rotation needs to be between 0 and 1*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float m_lerpRatio = 1.f;

	/** The ratio used to set at what distance push speed will begin to decrease*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float m_pushSpeedDropRatio = 0.75f;

// End Section Aaron Peneueta

	/** The movement speed of the character. */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Tunable Values")
		float m_movementSpeed = 200.f;

    /** The speed the character moves when he's pushed by an outside force. */
    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Tunable Values")
        float m_pushSpeed = 400.f;

	/** The speed at which the character will rotate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_rotationSpeed = 5.f;

	/** The leeway given to the direction of the character when calculating if he's facing the same direction he's moving.
	The larger the number the less accurate he's facing needs to be. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_directionPadding = 1.f;

	

private:

	void CalculateRotation(float DeltaTime);
	const FVector CalculateForces(float DeltaTime);
	void CloseToZero(float& toCheck, float howClose);

private:
// Aaron Peneueta
// Functions I have created or augmented
// For specific diffs I have done see word doc PorfDiffs
	float m_currentPushSpeed = 0.f;
	float m_pointToLowerSpeed = 0.f;
	float m_quadraticA = 0.f;
	bool m_calculatePush = false;
// End Section Aaron Peneueta
	
    FVector m_movementThisFrame = FVector(0.f, 0.f, 0.f);
    FVector m_pushDirection = FVector(0.f, 0.f, 0.f);
    float m_distanceTraveled = 0.f;
    float m_distanceToTravel;
	bool m_isWalking = false;
};
