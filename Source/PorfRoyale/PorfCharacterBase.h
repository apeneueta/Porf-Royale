// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Effect.h"
#include "PorfCharacterBase.generated.h"

class AAbilityActionChain;
class UBoxComponent;

// Collapsed code
UENUM(BlueprintType)
enum ActionState
{
    Idle,
    Grabbing,
    Deflecting,
    Headbutt,
	Holding,
	TankCombo1,
	TankCombo2,
	TankCombo3,
	Hit,
	Dodge,
	MysticCombo1,
	MysticCombo2,
	MysticCombo3,
	SuperPunch,
	HitHeavy,
	HitLeft,
	DodgeBack,
	Death,
	Stunned,
	Block,
	CastStart,
	CastLoop,
	CastOver,
	SuperPunchCharged,
	Pushed,
	VictoryPose,




};

UCLASS()
class PORFROYALE_API APorfCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	// Aaron Peneueta
	// Functions I have created or augmented
	// For specific diffs I have done see word doc PorfDiffs

	/** This will set the characters movement speed*/
	UFUNCTION(BlueprintCallable)
		void SetMovementSpeed(float movementSpeed);

	/** This will set the characters rotation variables speed and lerp ratio*/
	UFUNCTION(BlueprintCallable)
		void SetRotationVariables(float rotSpeed, float lerpRatio);

	/** This will set the characters push speed*/
	UFUNCTION(BlueprintCallable)
		void SetPushSpeed(float pushSpeed);

	UFUNCTION(BlueprintCallable)
		float GetPushSpeed();

	/** This will increase the specified amount for the character's stun meter.  Returns true, if stun meter is at the max.  False, if it's less than the max.*/
	UFUNCTION(BlueprintCallable)
		bool IncreaseStunMeter(float amount);

	/** This function will decrease the character's stun meter by the amount that is set in the variable m_stunMeterRegen*/
	UFUNCTION(BlueprintCallable)
		void RegenerateStunMeter(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void SnapTurn(FVector direction);

	/** Searches for status effect*/
	UFUNCTION(BlueprintCallable)
		bool CheckStatusEffect(StatusEffects effect);

	// Collapesed code
	APorfCharacterBase(const FObjectInitializer& ObjectInitializer);

    // Initialization
    virtual void PostInitProperties() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void Tick(float DeltaTime) override;


	// Scott did this don't be mad.
	/** What happens when this character has been hit by an ability */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnHasBeenHit();

	// Andrew did this don't be mad.
	/** What happens when this character blocks. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Block();

	// Andrew did this don't be mad.
	/** What happens when this character stops blocking. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UnBlock();

	// Andrew did this don't be mad.
	/** What happens when mystic Porf uses the dodge. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Teleport();

	// Andrew did this don't be mad.
	/** What happens when this character has been hit by an ability. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnHasBeenHitHeavy();


	// Andrew did this don't be mad.
	/** What happens when this character has been hit by an ability. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnHasBeenHitLeft();

	// Andrew did this don't be mad.
	/** What happens when this character has been hit by the lazer ability. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnLazerHit();

	// Andrew did this don't be mad.
	/** What happens when this character has been hit by the lazer ability. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void LazerStopHit();

	// Andrew did this don't be mad.
	/** What happens when this character has been hit by an ability. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnBarrierHit();
    
    /** This will push the character in the specified direction a specified distance at a speed of character's movement speed*/
	UFUNCTION(BlueprintCallable)
		void Push(FVector direction, float distance, float speed);

	

	/** This will stop the current ability being cast.  True, if you care if it's a hold button down ability.  False, will always end the ability. */
	UFUNCTION(BlueprintCallable)
		void StopAbility(bool isHoldDown);

	/** This will interupt whichever ability this character is casting.  */
	UFUNCTION(BlueprintCallable)
		void InteruptAbility();

	/** This will drain the specified amount from the character's mana.  Returns true, if mana is at 0.  False, if it's greater than 0.*/
	UFUNCTION(BlueprintCallable)
		bool DrainMana(float amount);

    // TODO: bring SetSimulatePhysics to true out of blueprint and into function, think about deleting this function
    /** This ragdolls the character*/
    UFUNCTION(BlueprintCallable)
        void Ragdoll();

    // TODO: think about deleting this function
    /** Gets the character out of ragdoll*/
    UFUNCTION(BlueprintCallable)
        void StandUp();
    
    // TODO: Think about removing this
    /** Turns gravity off*/
    UFUNCTION()
        void TurnOffGravity();

    // TODO: Think about removing this
    /** Turns gravity on*/
    UFUNCTION()
        void TurnOnGravity();

    /** For development purposes only, restarts the level */
    UFUNCTION(BlueprintCallable)
        void RestartLevel();

    
		
	/** This deals damage to the character.  Returns true, if health is less than or equal to 0.  False, if it's greater than 0.*/
	UFUNCTION(BlueprintCallable)
		bool DealDamage(float amount);

	

    /** This will add a status effect to the Porf for the specified duration. */
    UFUNCTION(BlueprintCallable)
        void AddStatusEffect(StatusEffects effect, const float statusDuration);
    
    /** Searches and removes the specified status effect */
    UFUNCTION(BlueprintCallable)
        void RemoveStatusEffect(StatusEffects toRemove);

	

    /** This sets the characters action state*/
	UFUNCTION(BlueprintCallable)
	    void SetActionState(ActionState state);

    /** Get the character's current action state*/
    UFUNCTION(BlueprintCallable)
        ActionState GetActionState() { return m_state; }

    /** Have this character take the specified damage*/
    UFUNCTION()
        void ReceiveDamage(const float damage);
   
    UFUNCTION()
        void OnMeleeHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	

    // Input
    UFUNCTION(BlueprintCallable)
        void MoveUpDown(float axisValue);
    UFUNCTION(BlueprintCallable)
        void MoveLeftRight(float axisValue);
    UFUNCTION(BlueprintCallable)
        void UseClassAbility();
	UFUNCTION(BlueprintCallable)
		void UseComboAbility();
    UFUNCTION(BlueprintCallable)
        void UsePowerAbility();
	UFUNCTION(BlueprintCallable)
		void UseDodgeAbility();
	UFUNCTION(BlueprintCallable)
		void UseBlockAbility();

	UShapeComponent* GetMeleeBox(const FName tag);
    void Stop();
    void EndAbility();

protected:
    virtual void BeginPlay() override;
    void InitAbilities();

private:
    void ValidateHealth();
    void Move(FVector& movementVec);
    void RemoveFinishedStatusEffects();
    bool Immobilized();
    void RestoreHealth(float DeltaTime);
	void RestoreMana(float DeltaTime);
    void SetRagdollMeshLocation();
    void SetupMeleeBox();
    int CreateStatusEffect();
    void CastAbility(AAbilityActionChain * pAbility);

protected:

	// Aaron Peneueta
	// Functions I have created or augmented
	// For specific diffs I have done see word doc PorfDiffs

	/** How long before character mana meter starts regenerating*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_manaMeterDelay = 2.f;
	/** The maximum Stun Meter of the character*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_maxStunMeter = 100.f;
	/** The current level of the stun meter of the character*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_stunMeter = 0.f;
	/** The regeneration rate of the stun meter of the character*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_stunMeterRegen = 5.f;
	/** How long before character stun meter starts lowering*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_stunMeterDelay = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		bool m_abilityHeldDown = false;

	// Collapsed code


    // TODO: move this into an array of abilities
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        AAbilityActionChain* m_pClassAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AAbilityActionChain* m_pComboAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AAbilityActionChain* m_pDodgeAbility;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        AAbilityActionChain* m_pPowerAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AAbilityActionChain* m_pBlockAbility;
    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        TArray<AAbilityActionChain*> m_abilities;*/
    UPROPERTY(BlueprintReadOnly)
        AAbilityActionChain* m_pCurrAbility;

    /** The maximum health of the character*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_maxHealth = 100.f;
   
	/** The health regeneration rate of the character*/
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Tunable Values")
		float m_healthRegen = 5.f;

    /** The current health of the player*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_health;
    /** The defense of the character.  The higher the defense the less damage this character will take*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values", meta=(ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
        float m_defense;

    /** The maximum mana of the character*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_maxMana = 100.f;
    /** The mana regeneration rate of the character*/
    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_manaRegen = 5.f;
    /** The current mana of the character*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_mana;
	

    /** How long the character will be stunned for once his break meter is depleted*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_breakMeterStunDuration = 2.0f;



    // TODO: Think about deleting this
    /** Whether or not this character is currently ragdolled*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        bool m_ragdolled = false;

	/** Scott: Adding this so that all hit boxes get the OnMeleeHit event added. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
		TArray<FName> m_hitBoxTags;

	

private:

	// Aaron Peneueta
	// Functions I have created or augmented
	// For specific diffs I have done see word doc PorfDiffs

	float m_delayManaRegen = 0.0f;
	float m_delayStunMeterRegen = 0.0f;

	// Collapsed code
    TArray<AEffect*> m_statusEffects;
    ActionState m_state{ Idle };
    bool m_restoreHealth{ false };
	bool m_restoreMana{ false };
	
};
