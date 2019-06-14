// Fill out your copyright notice in the Description page of Project Settings.

#include "PorfCharacterMovementComponent.h"
#include "PorfRoyale.h"


//-------------------------------------------------------------------------------------------------
//  In this custom movement class we do NOT use the ControlInputVector for movement.  This is
//  because it is consumed by it's parent class CharacterMovementComponent before we get a
//  chance to use it ourselves.  We use m_movementThisFrame instead, call AddToMovementThisFrame()
//  to add to the movement vector.  Movement in this game is suppose to be abrupt.  If a character
//  gets pushed a certain direction and then is pushed in a different direction the character, should
//  stop all movement in the direction of the first push and only move in the direction of the
//  subsequent push.
//-------------------------------------------------------------------------------------------------

// Aaron Peneueta
// Functions I have created or augment
// For specific diffs I have done see word doc PorfDiffs


//-------------------------------------------------------------------------------------------------
//  This will calculate movement for the character this frame and move the character.
//-------------------------------------------------------------------------------------------------
void UPorfCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Ensures that movement speed is not higher than max speed
	if (m_movementSpeed > m_maxMovementSpeed)
		m_movementSpeed = m_maxMovementSpeed;

	// Calculate the amount we want to move this frame by multipling by the modifier
	m_movementThisFrame = m_movementThisFrame * DeltaTime * m_movementSpeed;

	// Handle Rotation
	if (!m_movementThisFrame.IsNearlyZero())
	{
		CalculateRotation(DeltaTime);

		m_isWalking = true;
	}
	else
	{
		m_isWalking = false;
	}
	
	FVector forceToAdd = FVector::ZeroVector;;

	// Get additional forces on actor
	if (m_calculatePush)
	{
		forceToAdd = CalculateForces(DeltaTime);
	}

	// Set actor to new location based on movement and other forces
	GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + m_movementThisFrame + forceToAdd);

	// ResetMovementThisFrame
	m_movementThisFrame *= 0.f;
}

//-------------------------------------------------------------------------------------------------
//  This handles the rotation of character.
//-------------------------------------------------------------------------------------------------
void UPorfCharacterMovementComponent::CalculateRotation(float DeltaTime)
{
	// TODO: This appears unecessary Remove
	// Calculate our destination vector
	FVector dest = (GetOwner()->GetActorLocation() + m_movementThisFrame) - GetOwner()->GetActorLocation();

	// Get the direction (rotation) of the vector
	FRotator destRot = m_movementThisFrame.Rotation();

	// Get the actor's current direction (rotation)
	FRotator actorRot = GetOwner()->GetActorRotation();

	// Get current rotation
	FRotator newRotation = actorRot;

	// Rotation Speed when character speed is at 0;
	float baseRotSpeed = m_rotationSpeed - 6.0f;

	// What is added to base rotation speed as movement speed changes
	float rotSpeedOffset = 0.1f * FMath::Pow(m_movementSpeed, 2.0f) / 1000.0f;

	float currentRotSpeed = rotSpeedOffset + baseRotSpeed;

	// Makes sure we do not exceed characters max rotation speed
	if (currentRotSpeed > m_rotationSpeed)
		currentRotSpeed = m_rotationSpeed;

	//UE_LOG(Character, Warning, TEXT("Rot Speed: %f"), currentRotSpeed);


	// TODO: Remove if breaks game
	// Trying to catch an edge case in unreal where checking against 0.0f seems to be ignored
	if (destRot.Yaw <= 0.1f && destRot.Yaw >= -0.1f)
	{
		//UE_LOG(Character, Warning, TEXT("RotationTest: Yaw near 0"));
		destRot.Yaw = 0.1f;
	}


	// Handle rotation or stop rotating if we are going in the desired direction
	if (actorRot.Yaw >= destRot.Yaw + m_directionPadding || actorRot.Yaw <= destRot.Yaw - m_directionPadding)
	{
		/* Unreal rotation
		          0
		          |
		-90_______|_______90
		          |
		          |
		          180
        */

		//** TO DO: need to think of better names for these
		// This calculates the degrees we need to rotate to face the desired direction.  Calculates
		// clockwise and counter clockwise.  Depending on our current rotation and the desired rotation
		// the equations will either calculate clockwise or counter clockwise
		float locationMinusGoalRot = FMath::Abs(actorRot.Yaw - destRot.Yaw);
		float otherRot = (180.f - FMath::Abs(actorRot.Yaw)) + (180.f - FMath::Abs(destRot.Yaw));

		bool rotClockwise = true;

		// If our rotation is greater than desired rotation
		// locationMinusGoalRot = clockwise rotation
		// otherRot = counter clockwise rotation 
		if (actorRot.Yaw > destRot.Yaw)
			rotClockwise = locationMinusGoalRot >= otherRot;

		// Current rotation is less than the desired rotation
		// locationMinusGoalRot = counter clockwise rotation
		// otherRot = clockwise rotation
		else
			rotClockwise = locationMinusGoalRot <= otherRot;

		// Determine the direction we rotate
		float toAdd = 0;
		if (rotClockwise)
			toAdd = currentRotSpeed;
		else
			toAdd = -currentRotSpeed;

		newRotation.Yaw += toAdd;

		// When rotation speed is greater than the location minus the goal rotation
		// sets the rotation yaw to the destination yaw
		if (locationMinusGoalRot <= currentRotSpeed)
		{
			newRotation.Yaw = destRot.Yaw;
		}
	}


	// TODO: This may change
	// Set the the actor's position
	//GetOwner()->SetActorRotation(newRotation);
	//GetOwner()->SetActorRotation(FMath::Lerp(actorRot, newRotation, m_lerpRatio));
	GetOwner()->SetActorRotation(FMath::RInterpTo(actorRot, newRotation, DeltaTime, currentRotSpeed));
}

void UPorfCharacterMovementComponent::Snap(FVector direction)
{
	FRotator newRotation = GetOwner()->GetActorRotation();
	FRotator destRot = direction.Rotation();

	newRotation.Yaw = destRot.Yaw;
	GetOwner()->SetActorRotation(newRotation);
}

//-------------------------------------------------------------------------------------------------
//  This pushes the character in a specified direction a specified distance.  This will
//-------------------------------------------------------------------------------------------------
void UPorfCharacterMovementComponent::Push(FVector direction, float distance, float speed)
{
	UE_LOG(Character, Error, TEXT("PushCalled"));
	m_calculatePush = true;
    m_pushDirection = direction;
    m_distanceToTravel = distance;
    m_distanceTraveled = 0.f;
	m_pushSpeed = speed;
	m_currentPushSpeed = m_pushSpeed;

	if (m_distanceToTravel > 0)
	{
		// Calculates at what point of distanced traveled does speed begin to decrease
		m_pointToLowerSpeed = m_distanceToTravel * m_pushSpeedDropRatio;


		float parabolaWidth = FMath::Square((m_distanceToTravel - m_pointToLowerSpeed));

		// Find the value of a in the quadratic function y = a(x-h)^2 + k
		m_quadraticA = -(m_pushSpeed / parabolaWidth);
	}
	else
	{
		m_quadraticA = 0.f;
	}
}

//-------------------------------------------------------------------------------------------------
//  This calculates any forces that our still being applied to the actor.  Forces, happen
//  over a period of time.  They are not reset at the end of each frame.  This does not
//  apply the force to the actor directly, it just calculates what should be applied and
//  returns it.
//      return: the force on the character this frame.
//-------------------------------------------------------------------------------------------------
const FVector UPorfCharacterMovementComponent::CalculateForces(float DeltaTime)
{
    FVector forceToAdd;

	
	// Checks for case when the character is supposed to decrease speed as soon as push has been called
	if (m_pointToLowerSpeed == 0)
	{
		// Finds the speed that goes with the distance along the parabolic curve
		m_currentPushSpeed = m_quadraticA * FMath::Square((m_distanceTraveled)) + m_pushSpeed;
	}
	else if (m_distanceTraveled >= m_pointToLowerSpeed)
	{
		// Once distance traveled matches the point where speed begins to lower
		// calculates what the speed should be along the parabolic curve
		// quadratic function y = a(x-h)^2 + k
		m_currentPushSpeed = m_quadraticA * FMath::Square((m_distanceToTravel - m_pointToLowerSpeed)-(m_distanceToTravel - m_distanceTraveled)) + m_pushSpeed;
	}

	// Checks for when character has reached the distance goal then resets push variables to default
	// Also checks if distance traveled is -1 handles an edge case cause by super punch
    if ((m_distanceToTravel > 0 && m_distanceTraveled >= (m_distanceToTravel - 0.5f)) || m_distanceTraveled <= -1)
    {
		UE_LOG(Character, Warning, TEXT("PushTest: Made Distance"));
        m_distanceToTravel = 0.f;
        m_distanceTraveled = 0.f;
		m_currentPushSpeed = 0.f;
		m_calculatePush = false;
    }

    // Distance To Travel
    float distanceThisFrame = m_distanceToTravel * DeltaTime * m_currentPushSpeed;
    m_distanceTraveled += distanceThisFrame;

    forceToAdd = m_pushDirection * distanceThisFrame;

	return forceToAdd;
}


//-------------------------------------------------------------------------------------------------
//  Sets the speed of character rotation and the lerp ratio for rotation
//-------------------------------------------------------------------------------------------------
void UPorfCharacterMovementComponent::SetRotationSpeedAndLerpRatio(float rotSpeed, float lerpRatio)
{
	m_rotationSpeed = rotSpeed;

	FMath::Clamp(lerpRatio, 0.0f, 1.0f);

	m_lerpRatio = lerpRatio;
}

// Collapsed Code
//-------------------------------------------------------------------------------------------------
//  This is our custom check to see if a value is close to zero.  This will set the value
//  to zero if it's within the tunable range to zero (negative or positive).
//      toCheck: the number to check, this number will be changed to zero if it's within the range
//      howClose: THIS MUST BE POSITIVE.  How close to zero does the number have to be to change
//                  it to zero.
//-------------------------------------------------------------------------------------------------
void UPorfCharacterMovementComponent::CloseToZero(float& toCheck, float howClose)
{
	if (toCheck > howClose && toCheck < howClose ||
		toCheck < howClose  && toCheck > -howClose)
	{
		toCheck = 0.f;
	}
}
//-------------------------------------------------------------------------------------------------
//  This will shop the character's movement
//-------------------------------------------------------------------------------------------------
void UPorfCharacterMovementComponent::Stop()
{
    m_movementThisFrame *= 0.f;
    m_pushDirection *= 0.f;
}