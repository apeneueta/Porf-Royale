// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityActionChain.h"

#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes//Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "PorfRoyale.h"
#include "Effect.h"

//-------------------------------------------------------------------------------------------
// This serves as an entire chain of different actions for an ability.  Every link in the chain
// is an action that either leads into another action or ends the chain.  The key methods from
// a scripter/designer standpoint are OnCast(), OnCastHit(), OnModify(), OnFollowUp(), and OnEnd()
// they are all BlueprintImplementableEvents.  They do NOT have to be implemented for every
// action chain.  Only the ones they want to use.  Each function should have it's own micro
// action chain.  
//      OnCast(): this is called when the player first cast the ability.
//      OnCastHit(): this is called if the ability hits an object.  This is what the MeleeAction()
//                  and DeactivateMeleeAction() are for.  This turns on/off the hit box.  Which
//                  every Porf is required to have.
//      OnModify(): (right now not being used)
//      OnFollowUp(): this is called if the player casts the ability again before the initial cast
//                      is finished.
//      OnEnd(): this is called when the duration of the ability ends.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// TODO: Need to think about setting bCanEverTick to false.  First need to figure out why character
// no longer moves.
//-------------------------------------------------------------------------------------------
AAbilityActionChain::AAbilityActionChain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//-------------------------------------------------------------------------------------------
// Set's this ability action chain's owner.
//-------------------------------------------------------------------------------------------
void AAbilityActionChain::SetAbilityOwner(APorfCharacterBase* pAbilityOwner)
{
    m_pAbilityOwner = pAbilityOwner;
}

//-------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------
void AAbilityActionChain::BeginPlay()
{
	Super::BeginPlay();
	
}

//-------------------------------------------------------------------------------------------
// TODO: May be removing this
//-------------------------------------------------------------------------------------------
void AAbilityActionChain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (m_remainingCooldown > 0.f)
        m_remainingCooldown -= DeltaTime;

}

//-------------------------------------------------------------------------------------------------
// Interupts this ability
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::InteruptAbility()
{
    OnAbilityEnd();
}

//-------------------------------------------------------------------------------------------------
// Ends the ability if it failed to hit an opponent
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::EndAbilityNoHit()
{
    if (m_hit == false)
    {
        OnAbilityEnd();
    }
}

//-------------------------------------------------------------------------------------------------
//  Cast the ability, this is the entry point to cast an ability.  This will call OnCast(), which
//  is the functionality of this ability defined in blueprint.
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::BeginCast()
{
    StartCooldown();
    StartAbilityTimer();
    m_casting = true;
    OnCast();
}

//-------------------------------------------------------------------------------------------------
// When the ability ends we need to deactivate melee box and state we are no longer casting.
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::OnAbilityEnd()
{
    UE_LOG(Ability, Warning, TEXT("Ending ability."));
	DeactivateMeleeAction();

    // If we have a target porf we need to release it and then clear it
    if (m_pPorfHit)
    {
        ReleaseAction();
        m_pPorfHit = nullptr;
    }

	// Work around for the bug where block is never cleared
	// TODO: Find a better way
	if (m_pAbilityOwner->CheckStatusEffect(StatusEffects::Incapacitated) && m_pAbilityOwner->CheckStatusEffect(StatusEffects::Invulnerable))
	{
		m_pAbilityOwner->StopAbility(true);
	}

    m_casting = false;
    m_pAbilityOwner->EndAbility();
    m_pAbilityOwner->SetActionState(Idle);
}

//-------------------------------------------------------------------------------------------------
// This Starts the ability timer.  This is how long before the ability ends.
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::StartAbilityTimer()
{
  //  GetWorld()->GetTimerManager().SetTimer(m_abilityDurationHandle, this, &AAbilityActionChain::OnAbilityEnd, m_abilityDuration);
}

//-------------------------------------------------------------------------------------------------
// Stops this ability.  Resets all the data
//      isHoldDown: whether or not we care if it's a hold down ability.  If true, we will not
//                  stop the ability if it hit an opponent
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::StopCast(bool isHoldDown)
{
    // If we don't care if it's a hold down ability, then end the ability.  If we do care, make sure that it is, if so, end it.
    if ((isHoldDown == false) || (isHoldDown && m_isHoldDownAbility))
    {
        OnReleaseCast();
        m_casting = false;
        OnAbilityEnd();
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
// This function is called when an ability hits an object.  This is an internal function.
// This will call OnCastHit() which is a blueprint implementable function, which calls
// the actions.
//      pPorfHit: the porf hit, nullptr, if we didn't hit a porf
//      return: true, if we hit a porf, false otherwise
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::CastHit(APorfCharacterBase* pPorfHit)
{
    if (pPorfHit)
    {
        m_pPorfHit = pPorfHit;
        OnCastHit();
        return true;
    }
    else
    {
        UE_LOG(Ability, Warning, TEXT("Passing in a nullptr to CastHit()."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
// This starts the melee box timer.  After time expires the melee box will be deactivated.
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::StartMeleeTimer(const float time)
{
 //   GetWorld()->GetTimerManager().SetTimer(m_meleeDurationHandle, this, &AAbilityActionChain::DeactivateMeleeAction, time);
}

#pragma region Actions
//-------------------------------------------------------------------------------------------------
// Ability actions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// This deactivates the porf's melee box.
// This has to return a void because of how Unreal Timers work.
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::DeactivateMeleeAction()
{
	if (m_currentHitboxTag.IsNone())
	{
		return;
	}

    UShapeComponent* pMeleeBox = m_pAbilityOwner->GetMeleeBox(m_currentHitboxTag);
	m_currentHitboxTag = FName();
    pMeleeBox->SetActive(false);

	// If we didn't grab a porf then we set state to idle
	if (!m_pPorfHit)
	{
        OnMiss();
	}
}

//-------------------------------------------------------------------------------------------------
// Stuns the porf hit for a specified duration.
//      stunDuration: how long the stun last.
//      return: whether or not we successfully stunned the opponent.
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::StunAction(const float stunDuration)
{
    // we have a target
    if (m_pPorfHit != nullptr)
    {
        // stun the target
        m_pPorfHit->Stop();
        m_pPorfHit->AddStatusEffect(Stun, stunDuration);
        return true;
    }
    else
    {
        // no target to stun
        UE_LOG(Ability, Warning, TEXT("Attempting to stun with no target."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
// Grabs the opponent and attaches him to this actor.
//      return: whether or not we successfully grabbed the opponent.
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::GrabAction()
{
    // we have a target
    if (m_pPorfHit != nullptr)
    {
		//m_pAbilityOwner->SetActionState(Holding);

        // attach the target to this actor
        const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);

        m_pPorfHit->AttachToActor(m_pAbilityOwner, rules);

        // lift the porf above this porf's head
        // get this porfs position
        // TODO improve this a lot.
        FVector pos = m_pAbilityOwner->GetActorLocation();
        pos.Z += 250.f;

        m_pPorfHit->SetActorLocation(pos);

        m_pPorfHit->TurnOffGravity();

        return true;
    }
    else
    {
        // no target to grab
        UE_LOG(Ability, Warning, TEXT("Attempting to grab with no target."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
//  If we have a grabbed/attached opponent we release it.
//      return: whether or not we successfully released a target
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::ReleaseAction()
{
    // we have a target
    if (m_pPorfHit != nullptr)
    {
        // release/detach target
        const FDetachmentTransformRules rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
        m_pPorfHit->DetachFromActor(rules);
        m_pPorfHit->TurnOnGravity();

        return true;
    }
    else
    {
        // we don't have a target
        UE_LOG(Ability, Warning, TEXT("Attempting to release with not target."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
//		Pushes the opponent away from the ability owner.
//      distance: the distance to push the opponent\
//		speed: The speed to pull the opponent
//      return: whether or not we successfully pushed the opponent
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::PushAction(const float distance, float speed)
{
    // we have a target
    if (m_pPorfHit != nullptr)
    {
        // get the ability owner's rotation
        FRotator actorRot = m_pAbilityOwner->GetActorRotation();

        // determine direction to apply force
        //FVector actorVec = actorRot.Vector() * force;
        FVector actorVec = actorRot.Vector();
        actorVec.Normalize();
		
        // push the target along ability owners forward vector
        m_pPorfHit->Push(actorVec, distance, speed);
        return true;
    }
    else
    {
        // no target to push
        UE_LOG(Ability, Warning, TEXT("Attempting to push with no target."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
//  Pulls the opponent towards the ability owner.
//      distance: the distance to pull the opponent
//		speed: The speed to pull the opponent
//      return: whether or not we successfully pulled the opponent
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::PullAction(const float distance, float speed)
{
	// we have a target
	if (m_pPorfHit != nullptr)
	{
		// get the ability owners rotation
		FRotator actorRot = m_pAbilityOwner->GetActorRotation();

		// determine direction to apply force
		//FVector actorVec = actorRot.Vector() * force;
		FVector actorVec = actorRot.Vector();
		actorVec.Normalize();

		// pull the target toward the ability owner
		m_pPorfHit->Push(-actorVec, FMath::Abs(distance), speed);
		return true;
	}
	else
	{
		// no target to pull
		UE_LOG(Ability, Warning, TEXT("Attempting to pull with no target."));
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
//		Pushes the opponent away from its forward vector.
//      distance: the distance to push the opponent\
//		speed: The speed to pull the opponent
//      return: whether or not we successfully pushed the opponent
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::BackupAction(const float distance, float speed)
{
	// we have a target
	if (m_pPorfHit != nullptr)
	{
		// get the opponents rotation
		FRotator actorRot = m_pPorfHit->GetActorRotation();

		// determine direction to apply force
		//FVector actorVec = actorRot.Vector() * force;
		FVector actorVec = actorRot.Vector();
		actorVec.Normalize();

		// push the target along ability owners forward vector
		m_pPorfHit->Push(-actorVec, distance, speed);
		return true;
	}
	else
	{
		// no target to push
		UE_LOG(Ability, Warning, TEXT("Attempting to push with no target."));
		return false;
	}
}

bool AAbilityActionChain::PushSpeedAction(const float pushSpeed)
{
	// we have a target
	if (m_pPorfHit != nullptr)
	{
		m_pPorfHit->SetPushSpeed(pushSpeed);
		return true;
	}
	else
	{
		// no target to push
		UE_LOG(Ability, Warning, TEXT("Attempting to push with no target."));
		return false;
	}
}

bool AAbilityActionChain::MovementSpeedAction(const float movementSpeed)
{
	// we have a target
	if (m_pPorfHit != nullptr)
	{
		m_pPorfHit->SetMovementSpeed(movementSpeed);
		return true;
	}
	else
	{
		// no target to push
		UE_LOG(Ability, Warning, TEXT("Attempting to push with no target."));
		return false;
	}
}

bool AAbilityActionChain::RotationAction(const float rotSpeed, const float lerpRatio)
{
	// we have a target
	if (m_pAbilityOwner != nullptr)
	{
		m_pAbilityOwner->SetRotationVariables(rotSpeed, lerpRatio);
		return true;
	}
	else
	{
		// no owner of ability
		UE_LOG(Ability, Warning, TEXT("Attempting to change rotation with no target."));
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
//  Every porf is required to have a hitbox in front of them.  This activates that hitbox.
//  Use this action when you want to do some melee action chain. This calls the CastHit() method.
//      duration: how long the hitbox should be active for
//      return: whether or not we successfully activated a hitbox
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::MeleeAction(const float duration, const FName hitBox)
{
    UShapeComponent* pMeleeBox = m_pAbilityOwner->GetMeleeBox(hitBox);

	//Scott: adding an error check since we can now have typos.
	if (pMeleeBox == nullptr)
	{
		UE_LOG(Character, Error, TEXT("Hitbox tag was not found. Check your MeleeAction hitBox parameters for typos."));
		return false;
	}

	m_currentHitboxTag = hitBox;

    // Activate the collider
    /*pMeleeBox->SetActive(true);
    StartMeleeTimer(duration);

    // Get overlapping actors
    TArray<AActor*> overlappingActors;
    pMeleeBox->GetOverlappingActors(overlappingActors, TSubclassOf<APorfCharacterBase>()); // The filter doesn't work the way I thought it did.

    // We know that the first actor will be it's owner, but out of caution we will check
    for (int i{ 0 }; i < overlappingActors.Num(); ++i)
    {
        if (overlappingActors[i] != m_pAbilityOwner)
        {
            if (overlappingActors[i]->ActorHasTag("Porf"))
            {
                UE_LOG(Character, Warning, TEXT("Found overlapping actors."));
                CastHit(Cast<APorfCharacterBase>(overlappingActors[i]));
                return true;
            }
        }
    }*/

	//Scott: We need to exclude attack boxes, so I'm trying this with OverlapComponent instead.
	TArray<UPrimitiveComponent*> overlappingComponents;
	pMeleeBox->GetOverlappingComponents(overlappingComponents);

	// We know that the first actor will be it's owner, but out of caution we will check
	//for (int i{ 0 }; i < overlappingActors.Num(); ++i)

	for (int i{ 0 }; i < overlappingComponents.Num(); ++i)
	{
		if (overlappingComponents[i]->ComponentHasTag("HitCheck"))
		{
			//if (overlappingActors[i] != m_pAbilityOwner)
			if (overlappingComponents[i]->GetOwner() != m_pAbilityOwner)
			{
				//if (overlappingActors[i]->ActorHasTag("Porf"))
				if (overlappingComponents[i]->GetOwner()->ActorHasTag("Porf"))
				{
					UE_LOG(Character, Warning, TEXT("Found overlapping actors."));
					CastHit(Cast<APorfCharacterBase>(overlappingComponents[i]->GetOwner()));
					return true;
				}
			}
		}
	}

    // TODO see how to handle this
    return true;
}

//-------------------------------------------------------------------------------------------------
// This action applies a force to this porf in the direction he's facing.
//      force: the force to apply
//      return: whether or not we successfully charged
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::ChargeAction(const float distance, float speed)
{
    // we should always have an owner
    if (m_pAbilityOwner)
    {
        // get the direction to apply force
       // FVector chargeForce = m_pAbilityOwner->GetActorForwardVector() * force;
        FVector chargeForce = m_pAbilityOwner->GetActorForwardVector();
        chargeForce.Normalize();

        // apply force
        Cast<APorfCharacterBase>(m_pAbilityOwner)->Push(chargeForce, distance, speed);
        return true;
    }
    else
    {
        // this ability doesn't have an owner
        UE_LOG(Ability, Error, TEXT("Attempting to call charge action, but no owning Porf."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
// This action deals a specified damage to the opponent.
//      damage: the damage to deal
//      return: whether or not we successfully dealt damage
//-------------------------------------------------------------------------------------------------
bool AAbilityActionChain::DealDamageAction(const float damage)
{
    // we have an target
    if (m_pPorfHit != nullptr)
    {
        // deal damage to opponent
        m_pPorfHit->ReceiveDamage(damage);
        return true;
    }
    else
    {
        // no target to deal damage to
        UE_LOG(Ability, Warning, TEXT("Attempting to deal damage with no target."));
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
// This action stops this Porf
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::StopAction()
{
    m_pAbilityOwner->Stop();
}

//-------------------------------------------------------------------------------------------------
// This incapacitates this Porf for a specified duration.
//      incapacitatedDuration: the amount of time to be incapacitated
//-------------------------------------------------------------------------------------------------
void AAbilityActionChain::IncapacitateSelfAction(const float incapacitateDuration)
{
    m_pAbilityOwner->AddStatusEffect(Incapacitated, incapacitateDuration);
}

void AAbilityActionChain::FinishChainAction()
{
    OnAbilityEnd();
}
#pragma endregion