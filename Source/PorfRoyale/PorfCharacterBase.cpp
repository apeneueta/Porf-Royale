// Fill out your copyright notice in the Description page of Project Settings.

#include "PorfCharacterBase.h"

// #include "Runtime/Engine/Classes//Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "PorfRoyale.h"
#include "PorfCharacterMovementComponent.h"
#include "AbilityActionChain.h"
#include "PorfPlayerControllerBase.h"
#include "Effect.h"

//-------------------------------------------------------------------------------------------------
// This is the base Porf (character) class.  This is an interface.  The character can move,
// use class abilities, and powers.  Every character has a break meter, which is similar to
// a health bar.  Characters have abilities.  Character can only cast 1 ability at a time.
// characters can also have status effects which can affect the character in different ways.
//
// Every Porf is required to have exactly 1 box component tagged "MeleeBox"
//-------------------------------------------------------------------------------------------------


// Sets default values
APorfCharacterBase::APorfCharacterBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UPorfCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    InitAbilities();
	
}

//-------------------------------------------------------------------------------------------
// Initialize the class ability and power ability
//-------------------------------------------------------------------------------------------
void APorfCharacterBase::InitAbilities()
{
    // Create the power ability
	FName powerAbilityName = "PowerAbility";
    m_pPowerAbility = CreateDefaultSubobject<AAbilityActionChain>(powerAbilityName);
	if (m_pPowerAbility == nullptr)
	{
		UE_LOG(Character, Warning, TEXT("Unable to load ability: %s"), *powerAbilityName.ToString());
	}

    // Create the class ability
    FName classAbilityName = "ClassAbility";
    m_pClassAbility = CreateDefaultSubobject<AAbilityActionChain>(classAbilityName);
    if (m_pClassAbility == nullptr)
    {
        UE_LOG(Character, Warning, TEXT("Unable to load ability: %s"), *classAbilityName.ToString());
    }
}

//-------------------------------------------------------------------------------------------------
// Validates variables and sets them to a acceptable value if the value is not valid
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::PostInitProperties()
{
    Super::PostInitProperties();
    ValidateHealth();
}

//-------------------------------------------------------------------------------------------------
// This makes sure that the Porf's max health is above 0
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::ValidateHealth()
{
    // Make sure max health is above 0
    if (m_maxHealth <= 0)
    {
        // Set the Porf's max health to 1
        UE_LOG(Character, Error, TEXT("Instantiating character with 0 or less health.  Settting health to 1."));
        m_maxHealth = 1;
    }

    // Set the current health to max health
    m_health = m_maxHealth;

	//m_mana = m_maxMana;
}

//-------------------------------------------------------------------------------------------------
// Setups the Porf character.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::BeginPlay()
{
	Super::BeginPlay();
    bUseControllerRotationYaw = false;

    SetupMeleeBox();
}

//-------------------------------------------------------------------------------------------------
// Sets the characters action state
//      state: the new state
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::SetActionState(ActionState state)
{
	m_state = state;
}

//-------------------------------------------------------------------------------------------------
// Sets the melee box callback
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::SetupMeleeBox()
{
	//Scott: Since we can now have multiple melee boxes, we now have to loop through them.
	for (FName tag : m_hitBoxTags)
	{
		// Get the melee box
		UShapeComponent* pMeleeBox = GetMeleeBox(tag);

		if (pMeleeBox)
		{
			// Set callback
			pMeleeBox->OnComponentBeginOverlap.AddDynamic(this, &APorfCharacterBase::OnMeleeHit);
			pMeleeBox->SetActive(false);
		}
		else
		{
			UE_LOG(Character, Warning, TEXT("Tried to locate a hitbox but couldn't find one with that tag. Check Hit Box Tags for typos."));
		}
	}
}

//-------------------------------------------------------------------------------------------------
// This searches the components for the melee box.
//      return: a pointer to the melee box
//-------------------------------------------------------------------------------------------------
UShapeComponent* APorfCharacterBase::GetMeleeBox(const FName tag)
{
    // Get the melee box (should only be 1)
    TArray<UActorComponent*> boxComps = GetComponentsByTag(UShapeComponent::StaticClass(), tag);
    if (boxComps.Num() > 0 && boxComps[0])
    {
        return Cast<UShapeComponent>(boxComps[0]);
    }
    else
    {
        UE_LOG(Character, Warning, TEXT("Porf Character is lacking a melee box with tag %s"), *tag.ToString());
        return nullptr;
    }
}
//-------------------------------------------------------------------------------------------------
// Restores missing health, but will not exceed max health.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::RestoreHealth(float DeltaTime)
{
	// If we need to restore
	if (m_restoreHealth)
	{
		// Increase health by regen rate
		m_health += m_healthRegen * DeltaTime;
		if (m_health >= m_maxHealth)
		{
			// If we've reached the max health, we stop restoring
			m_health = m_maxHealth;
			m_restoreHealth = false;
		}
	}
}
void APorfCharacterBase::SetRagdollMeshLocation()
{
	// If we are in a ragdoll state, UE4 will not automatically have mesh follow root component
	if (m_ragdolled)
	{
		// Have mesh follow root component location manually
		TArray<USkeletalMeshComponent*> pMeshComps;
		// Get the mesh
		GetComponents(pMeshComps);

		// Get the Capsule Component (which is root component)
		TArray<UCapsuleComponent*> pCapComps;
		GetComponents(pCapComps);

		pMeshComps[0]->SetAllPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f));
		pMeshComps[0]->SetAllPhysicsAngularVelocityInDegrees(FVector(0.f, 0.f, 0.f));

		// We need to teleport the mesh, because it's a "fully simulated" mesh
		ETeleportType teleType = ETeleportType::TeleportPhysics;

		// TODO: Get by tag (how many skeletal meshes can an Actor have in Unreal?
		pMeshComps[0]->SetWorldLocation(pCapComps[0]->GetComponentLocation(), false, nullptr, teleType);
	}
}

//-------------------------------------------------------------------------------------------------
// Binds the different inputs with functions
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// TEMP 
void APorfCharacterBase::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

//-------------------------------------------------------------------------------------------------
// This sets the character's mesh in a ragdoll state
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::Ragdoll()
{
	// Make sure we aren't already ragdolled
	if (!m_ragdolled)
	{
		m_ragdolled = true;

		// ragdoll the Porf
		TArray<USkeletalMeshComponent*> pMeshComps;
		GetComponents(pMeshComps);
		pMeshComps[0]->SetSimulatePhysics(true);
	}
}

//-------------------------------------------------------------------------------------------------
// This stands the character back up by disabling ragdoll and reattaching it to the capsule component
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::StandUp()
{
	if (m_ragdolled)
	{
		m_ragdolled = false;

		// Get the mesh component
		TArray<USkeletalMeshComponent*> pMeshComps;
		GetComponents(pMeshComps);

		if (pMeshComps[0])
		{
			// Turn of ragdoll
			pMeshComps[0]->SetSimulatePhysics(false);

			// Reattach mesh to capsule(root)
			const FAttachmentTransformRules attachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);

			// Get the pivot ponit component based on it's tag (there should only be one of these per actor.
			TArray<UActorComponent*> pComponents;
			pComponents = GetComponentsByTag(UActorComponent::StaticClass(), "PivotPoint");

			// We successfully found the pivot
			if (pComponents[0])
			{
				// Stop the mesh from moving
				pMeshComps[0]->SetAllPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f));
				pMeshComps[0]->SetAllPhysicsAngularVelocityInDegrees(FVector(0.f, 0.f, 0.f));

				// Set the mesh to the pivot point's location
				USceneComponent* pPivot = Cast<USceneComponent>(pComponents[0]);
				pMeshComps[0]->AttachToComponent(pPivot, attachmentRules);
				pMeshComps[0]->SetWorldLocation(pPivot->GetComponentLocation());
				pMeshComps[0]->SetWorldRotation(GetActorRotation());
			}
			else
			{
				UE_LOG(Character, Warning, TEXT("Character doesn't have a pivot point."));
			}
		}
		else
		{
			UE_LOG(Character, Error, TEXT("Character doesn't have a mesh component."));
		}
	}
}

//-------------------------------------------------------------------------------------------------
// This moves the Porf along the x-axis.
//  axisValue: the direction to move
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::MoveUpDown(float axisValue)
{
	if (!Immobilized())
	{
		FVector forwardVec = FVector(axisValue, 0.f, 0.f);
		Move(forwardVec);
	}
}

//-------------------------------------------------------------------------------------------------
// This moves the Porf along the y-axis.
//  axisValue: the direction to move
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::MoveLeftRight(float axisValue)
{
	if (!Immobilized())
	{
		FVector rightVec = FVector(0.f, axisValue, 0.f);
		Move(rightVec);
	}
}

//-------------------------------------------------------------------------------------------------
// This attempts to cast this Porf's unique class ability.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::UseClassAbility()
{
	CastAbility(m_pClassAbility);
}

//-------------------------------------------------------------------------------------------------
// This attempts to cast this Porf's unique class ability.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::UseComboAbility()
{
	CastAbility(m_pComboAbility);
}
//-------------------------------------------------------------------------------------------------
// This attempts to cast this Porf's Unique Block ability.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::UseBlockAbility()
{
	CastAbility(m_pBlockAbility);
}

//-------------------------------------------------------------------------------------------------
// This attempts to cast this Porf's unique class ability.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::UseDodgeAbility()
{
	CastAbility(m_pDodgeAbility);
}

//-------------------------------------------------------------------------------------------------
// This attempts to cast the Porf's power ability.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::UsePowerAbility()
{
	CastAbility(m_pPowerAbility);
}
//-------------------------------------------------------------------------------------------
// This stops the ability that's currently being cast.  To interupt an ability use
// InteruptAbility().
//      isHoldDown: whether or not we care if it's a hold down ability
//-------------------------------------------------------------------------------------------
void APorfCharacterBase::StopAbility(bool isHoldDown)
{
	if (m_pCurrAbility)
	{
		if (m_pCurrAbility->StopCast(isHoldDown))
		{
			UE_LOG(Character, Warning, TEXT("(Stopping) REMOVING current ability."));
			m_pCurrAbility = nullptr;
		}
	}
}

//-------------------------------------------------------------------------------------------
// This interupts the ability that's currently being cast.
//-------------------------------------------------------------------------------------------
void APorfCharacterBase::InteruptAbility()
{
	if (m_pCurrAbility)
	{
		UE_LOG(Character, Warning, TEXT("(Interupting) REMOVING current ability."));
		m_pCurrAbility->InteruptAbility();
		m_pCurrAbility = nullptr;
	}
}

//-------------------------------------------------------------------------------------------
// Deals the specified damage to this character
//      amount: the amount of damage to deal
//      return: True, if health is equal to or less than 0.  False if it's greater than 0
//-------------------------------------------------------------------------------------------
bool APorfCharacterBase::DealDamage(float amount)
{
	m_health -= amount;

	if (m_health <= 0.f)
	{
		m_health = 0.f;
		return true;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
// This checks all of the Porf's status effects and determines if it's immobilized.  The are many
// effects which can cause a Porf to be immobilized.  Immobilized essentially means that the Porf
// won't respond to input from the player.
//-------------------------------------------------------------------------------------------------
bool APorfCharacterBase::Immobilized()
{
	// It only takes 1 status effect to immobilize a Porf, so once we find 1 we can return
	for (auto& effect : m_statusEffects)
	{
		if (effect->GetType() <= Incapacitated)
		{
			return true;
		}
	}

	// We aren't immobilized
	return false;
}
//-------------------------------------------------------------------------------------------------
// This is the callback when the melee box collides with an object.  This will call the appropriate
// ability's CastHit().
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::OnMeleeHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor->GetClass()->IsChildOf(APorfCharacterBase::StaticClass()))
		&& OverlappedComp->IsActive() && OverlappedComp->ComponentHasTag("HitCheck"))
	{
		if (m_pCurrAbility)
		{
			m_pCurrAbility->CastHit(Cast<APorfCharacterBase>(OtherActor));
		}
	}
}
//-------------------------------------------------------------------------------------------------
// Turn on gravity
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::TurnOnGravity()
{
	if (GetCharacterMovement() != nullptr)
	{
		UCharacterMovementComponent* pMovement = GetCharacterMovement();
		pMovement->GravityScale = 1.0f;
	}
}

//-------------------------------------------------------------------------------------------------
// Turn off gravity
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::TurnOffGravity()
{
	if (GetCharacterMovement() != nullptr)
	{
		UCharacterMovementComponent* pMovement = GetCharacterMovement();
		pMovement->GravityScale = 0.0f;
	}
}

//-------------------------------------------------------------------------------------------------
// Stops the Porf's movement
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::Stop()
{
	if (GetCharacterMovement() != nullptr)
	{
		UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());
		pMovement->Stop();
	}
}

//-------------------------------------------------------------------------------------------------
//  Set's the currently ability to null
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::EndAbility()
{
	m_pCurrAbility = nullptr;
}

// Aaron Peneueta
// Functions I have created or augmented
// For specific diffs I have done see word doc PorfDiffs

//-------------------------------------------------------------------------------------------------
// Update for the Porf.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Check if any of our status effects are over
    RemoveFinishedStatusEffects();
    RestoreHealth(DeltaTime);
    SetRagdollMeshLocation();
	RestoreMana(DeltaTime);
	RegenerateStunMeter(DeltaTime);
}

//-------------------------------------------------------------------------------------------------
// Restores missing Mana, but will not exceed max Mana.
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::RestoreMana(float DeltaTime)
{
	if (m_delayManaRegen >= 0.0f)
	{
		m_delayManaRegen -= (1.0f * DeltaTime);
	}
	else
	{
		m_delayManaRegen = 0.0f;

		//If we need to restore
		if (m_restoreMana)
		{
			//Increase Mana by regen rate
			m_mana += m_manaRegen * DeltaTime;
			if (m_mana >= m_maxMana)
			{
				//If we've reached the max Man, we stop restoring
				m_mana = m_maxMana;
				m_restoreMana = false;
			}
		}
	}
}


//-------------------------------------------------------------------------------------------
// This cast the currently selected ability
//      pAbility: the ability to cast
//-------------------------------------------------------------------------------------------
void APorfCharacterBase::CastAbility(/*int index*/AAbilityActionChain* pAbility)
{
    // Only cast if we are able
    if (!Immobilized())
    {
        // If the ability is not already being cast and it's not our current ability, cast the ability
		if (m_abilityHeldDown)
		{
			UE_LOG(Character, Warning, TEXT("Hold Down Ability"));
			return;
		}
		if (pAbility->IsCasting() == false)
        {
            if (m_pCurrAbility == nullptr)
            {
                if (pAbility->OnCooldown() == false)
                {
                    m_pCurrAbility = pAbility;
                    m_pCurrAbility->BeginCast();
                    UE_LOG(Character, Warning, TEXT("setting current ability."));
                }
            }
			else if (m_pCurrAbility == pAbility)
			{
				UE_LOG(Character, Warning, TEXT("Ability Overlap"));

				StopAbility(false);
				/*m_pCurrAbility->StopCast(false);
				m_pCurrAbility = nullptr;*/

			}
            else
            {
                UE_LOG(Character, Warning, TEXT("No ability is casting and no ability overlap so Interupting current ability and casting new ability"));

				InteruptAbility();
				m_pCurrAbility = pAbility;
				m_pCurrAbility->BeginCast();
            }
        }
        // We are casting this ability, make sure it's our current ability
        else if (m_pCurrAbility == pAbility)
        {
            UE_LOG(Character, Warning, TEXT("Doing Follow-up"));
			UE_LOG(Character, Warning, TEXT("Ability Overlap"));

			StopAbility(false);
			/*m_pCurrAbility->StopCast(false);
			m_pCurrAbility = nullptr;*/

        }

		else if (pAbility->IsCasting() == true)
		{
			UE_LOG(Character, Warning, TEXT("Character Casting."));
		}
        else
        {
            UE_LOG(Character, Error, TEXT("Ability currently being cast, but currAbility is null."));
        }
    }
	else
	{
		UE_LOG(Character, Warning, TEXT("Character Immobilized."));
	}
}

//-------------------------------------------------------------------------------------------
// Drains mana from this character
//      amount: the amount of mana to drain
//      return: True, if mana is equal to or less than 0.  False if it's greater than 0.
//-------------------------------------------------------------------------------------------
bool APorfCharacterBase::DrainMana(float amount)
{
	m_mana -= amount;

	if (m_mana <= 0.f)
	{
		m_mana = 0.f;
		return true;
	}

	m_delayManaRegen = m_manaMeterDelay;
	m_restoreMana = true;

	return false;
}

//-------------------------------------------------------------------------------------------
// Increases stun meter for this character
//      amount: the amount to increase to the stun meter
//      return: True, if is equal to the maximum of the stun meter.  False if it's less than the max.
//-------------------------------------------------------------------------------------------
bool APorfCharacterBase::IncreaseStunMeter(float amount)
{
	m_stunMeter += amount;

	if (m_stunMeter >= m_maxStunMeter)
	{
		m_stunMeter = m_maxStunMeter;
		return true;
	}

	m_delayStunMeterRegen = m_stunMeterDelay;
	return false;
}

void APorfCharacterBase::RegenerateStunMeter(float DeltaTime)
{
	if (m_delayStunMeterRegen >= 0.0f)
	{
		m_delayStunMeterRegen -= (1.0f * DeltaTime);
	}
	else
	{
		if (m_stunMeter <= 0)
		{
			m_stunMeter = 0.0f;
			return;
		}

		m_stunMeter -= m_stunMeterRegen * DeltaTime;
	}
}

//-------------------------------------------------------------------------------------------------
// This moves the Porf by adding a movement vector to it's current vector.
//      movementVec: the vector to move the Porf
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::Move(FVector& movementVec)
{
    if (GetCharacterMovement() != nullptr)
    {
        UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());

		if (!pMovement->GetOwner()->IsRootComponentStatic())
			pMovement->AddToMovementThisFrame(movementVec);
    }
    else
    {
        UE_LOG(Character, Error, TEXT("Character doesn't have a movement component."));
    }
}

//-------------------------------------------------------------------------------------------------
// Pushes the Porf a specified distance in a specified direction.
//      direction: the direction to push the Porf
//      distance: the distance the Porf should be pushed in that direction
//		speed: the speed that the porf should be pushed at
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::Push(FVector direction, float distance, float speed)
{
    if (GetCharacterMovement() != nullptr)
    {
        UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());

		if (!pMovement->GetOwner()->IsRootComponentStatic())
			pMovement->Push(direction, distance, speed);
    }
    else
    {
        UE_LOG(Character, Warning, TEXT("Porf does NOT have a movement component."));
    }
}

void APorfCharacterBase::SetPushSpeed(float pushSpeed)
{
	if (GetCharacterMovement() != nullptr)
	{
		UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());
		pMovement->SetPushSpeed(pushSpeed);
	}
	else
	{
		UE_LOG(Character, Warning, TEXT("Porf does NOT have a movement component."));
	}
}

float APorfCharacterBase::GetPushSpeed()
{
	if (GetCharacterMovement() != nullptr)
	{
		UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());
		return pMovement->GetPushSpeed();
	}
	else
	{
		UE_LOG(Character, Warning, TEXT("Porf does NOT have a movement component."));
	}

	return 0.0f;
}

void APorfCharacterBase::SetMovementSpeed(float movementSpeed)
{
	if (GetCharacterMovement() != nullptr)
	{
		UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());
		pMovement->SetMovementSpeed(movementSpeed);
	}
	else
	{
		UE_LOG(Character, Warning, TEXT("Porf does NOT have a movement component."));
	}
}

void APorfCharacterBase::SetRotationVariables(float rotSpeed, float lerpRatio)
{
	if (GetCharacterMovement() != nullptr)
	{
		UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());
		pMovement->SetRotationSpeedAndLerpRatio(rotSpeed, lerpRatio);
	}
	else
	{
		UE_LOG(Character, Warning, TEXT("Porf does NOT have a movement component."));
	}
}

void APorfCharacterBase::SnapTurn(FVector direction)
{
	if (GetCharacterMovement() != nullptr)
	{
		UPorfCharacterMovementComponent* pMovement = Cast<UPorfCharacterMovementComponent>(GetCharacterMovement());
		pMovement->Snap(direction);
	}
	else
	{
		UE_LOG(Character, Warning, TEXT("Porf does NOT have a movement component."));
	}
}

//-------------------------------------------------------------------------------------------------
// Inflicts X damage to the Porf.
//      damage: the amount of damage to do to the player
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::ReceiveDamage(const float damage)
{
	// Player takes no damage when invulnerable
	for (auto& effect : m_statusEffects)
	{
		if (effect->GetType() == Invulnerable)
		{
			return;
		}
	}

    // if we are hit we stop restoring health
    m_restoreHealth = false;
    m_health -= damage * (1.f - m_defense);

    // break meter is depeleted
    if (m_health <= 0.f)
    {
        // we are stunned
        AddStatusEffect(Stun, m_breakMeterStunDuration);

        // start to restore health
        m_restoreHealth = true;
    }
}


#pragma region StatusEffects
//-------------------------------------------------------------------------------------------------
// Status Effect Methods
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Checks for a specific status effect
//      effect: the status effect to check for
//-------------------------------------------------------------------------------------------------
bool APorfCharacterBase::CheckStatusEffect(StatusEffects effect)
{
	for (int i{ 0 }; i < m_statusEffects.Num(); ++i)
	{
		if (m_statusEffects[i]->GetType() == effect)
		{
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
// This loops through all of our status effects and removes them if they are marked finished
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::RemoveFinishedStatusEffects()
{
	int i{ 0 };
	while (i < m_statusEffects.Num())
	{
		if (m_statusEffects[i]->IsFinished())
		{
			m_statusEffects[i]->Destroy();
			m_statusEffects.RemoveAt(i);
			continue;
		}
		++i;
	}
}
// End Section Aaron Peneueta


//-------------------------------------------------------------------------------------------------
// Creates and adds a status effect to the character.
//      status: type of status effect to add
//      statusDuration: how long the status will last until it is marked as"Finished"
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::AddStatusEffect(StatusEffects effect, const float statusDuration)
{
    int ID = CreateStatusEffect();

    if (ID != -1)
    {
        m_statusEffects.Last()->Init(ID, effect, statusDuration);
    }
    else
        UE_LOG(Character, Warning, TEXT("Unable to find appropriate player id for status effect."));
}

//-------------------------------------------------------------------------------------------------
// Removes a specified status effect from the character
//      toRemove: the status effect to remove
//-------------------------------------------------------------------------------------------------
void APorfCharacterBase::RemoveStatusEffect(StatusEffects toRemove)
{
    for (int i{ 0 }; i < m_statusEffects.Num(); ++i)
    {
        if (m_statusEffects[i]->GetType() == toRemove)
        {
            m_statusEffects.RemoveAt(i);
        }
    }
}



//-------------------------------------------------------------------------------------------------
//  This creates a status effect in the world.  This does NOT add it to the character.
//      return: the Id of this character
//-------------------------------------------------------------------------------------------------
int APorfCharacterBase::CreateStatusEffect()
{
    // I have to spawn the actor instead of calling NewObject, because I need the world in
    // order to have the effect have a timer for it.
    FVector location(-1000.f, -1000.0f, -1000.0f);
    FRotator rotation(0.0f, 0.0f, 0.0f);
    FActorSpawnParameters spawnInfo;
    m_statusEffects.Add(GetWorld()->SpawnActor<AEffect>(location, rotation, spawnInfo));

    // We only pass in player index to status effect to display the status effect UI.  Otherwise, we can remove all this code getting ID (and remove parameter from Init()
    int ID = -1;

    AController* pController = GetController();
    if (pController)
    {
        APorfPlayerControllerBase* pPlayerController = Cast<APorfPlayerControllerBase>(pController);
        if (pPlayerController)
        {
            ID = UGameplayStatics::GetPlayerControllerID(pPlayerController);
        }
        else
        {
            UE_LOG(Character, Warning, TEXT("Failed to cast controller to porf controller"));
        }
    }
    else
    {
        UE_LOG(Character, Warning, TEXT("Unable to find controller for porf character."));
    }

    return ID;
}


#pragma endregion