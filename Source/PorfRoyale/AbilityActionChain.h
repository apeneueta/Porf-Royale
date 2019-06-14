// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/Containers/EnumAsByte.h"
#include "PorfCharacterBase.h"
#include "GameFramework/Actor.h"
#include "AbilityActionChain.generated.h"

class APorfCharacterBase;

UCLASS()
class PORFROYALE_API AAbilityActionChain : public AActor
{
	GENERATED_BODY()
	
public:	

	// Collapsed Code
	AAbilityActionChain();

	virtual void Tick(float DeltaTime) override;

    /** Sets this ability's owner (who's casting the ability */
    UFUNCTION(BlueprintCallable)
        virtual void SetAbilityOwner(APorfCharacterBase * pAbilityOwner);

    /** Whether or not this ability hit a character */
	UFUNCTION(BlueprintCallable)
	    bool GetHit() { return m_hit; }

    /** What happens when this abilty is cast. */
    UFUNCTION(BlueprintImplementableEvent)
        void OnCast();

    /** What happens when this ability his an opponent*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
        void OnCastHit();

    /** What happens if this ability is cast when the modifier button is being held down*/
    UFUNCTION(BlueprintImplementableEvent)
        void OnModify();

    /** What happens when this ability is cast, while it's already being cast*/
    UFUNCTION(BlueprintImplementableEvent)
        void OnFollowUp();

    /** What happens when this ability misses and doesn't hit an opponent*/
    UFUNCTION(BlueprintImplementableEvent)
        void OnMiss();

    /** What happens when this ability ends*/
    UFUNCTION(BlueprintImplementableEvent)
        void OnEnd();

    /** What happens when the button is released while casting this ability*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
        void OnReleaseCast();

    /** This checks this ability versus the action state of the opponent.  True, if this ability "beats" the opponent's current action state.  False, otherwise*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
        bool Clash(ActionState otherActionState);

    /** Stops this ability*/
    UFUNCTION(BlueprintCallable)
        void InteruptAbility();

    void BeginCast();
    void StartAbilityTimer();
    bool StopCast(bool isHoldDown);
    void OnAbilityEnd();
    bool CastHit(APorfCharacterBase * pPorfHit);
    bool IsCasting() { return m_casting; }
    bool OnCooldown() { return m_remainingCooldown > 0.f; }

protected:

	// Aaron Peneueta
	// Functions I have created or augmented
	// For specific diffs I have done see word doc PorfDiffs

	/** Pushes the opponent a specified distance and speed*/
	UFUNCTION(BlueprintCallable)
		bool PushAction(const float distance, float speed);

	/** Pulls the opponent a specified distance and speed*/
	UFUNCTION(BlueprintCallable)
		bool PullAction(const float distance, float speed);

	/** Pushes the opponent backward a specified distance and speed*/
	UFUNCTION(BlueprintCallable)
		bool BackupAction(const float distance, float speed);

	/** sets the pushSpeed*/
	UFUNCTION(BlueprintCallable)
		bool PushSpeedAction(const float pushSpeed);
	/** sets the MovementSpeed*/
	UFUNCTION(BlueprintCallable)
		bool MovementSpeedAction(const float movementSpeed);
	/** sets the Rotation speed and lerpRatio*/
	UFUNCTION(BlueprintCallable)
		bool RotationAction(const float rotSpeed, const float lerpRatio);
	/** Moves the character casting this ability a specified distance*/
	UFUNCTION(BlueprintCallable)
		bool ChargeAction(const float distance, float speed);

	// Collapsed Code
	virtual void BeginPlay() override;

    /** Stuns the opponent*/
    UFUNCTION(BlueprintCallable)
        bool StunAction(const float stunDuration);
    /** Grabs the opponent*/
    UFUNCTION(BlueprintCallable)
        bool GrabAction();
    /** Releases the opponent (stop grabbing the opponent)*/
    UFUNCTION(BlueprintCallable)
        bool ReleaseAction();
   
    /** Activates the melee box for a specified duration.  If -1, for the character casting this ability*/
    UFUNCTION(BlueprintCallable)
        bool MeleeAction(const float duration, const FName hitBox);
    /** Deactivates the melee box of the character casting this ability*/
    UFUNCTION(BlueprintCallable)
        void DeactivateMeleeAction();
    
    /** Deals damage to the opponent*/
    UFUNCTION(BlueprintCallable)
        bool DealDamageAction(const float damage);
    /** Stops the movement of the character casting this ability*/
    UFUNCTION(BlueprintCallable)
        void StopAction();
    /** Incapacitates the character casting this ability for a specified time.  If -1, the character will be incapacitated until the status effect is explicitly removed*/
    UFUNCTION(BlueprintCallable)
        void IncapacitateSelfAction(const float incapacitateDuration);
    /** Ends this ability*/
	UFUNCTION(BlueprintCallable)
		void FinishChainAction();
    /** Ends this ability only if it didn't hit an opponent*/
    UFUNCTION(BlueprintCallable)
        void EndAbilityNoHit();

private:
	// Collapsed Code
    void StartMeleeTimer(const float time);
    void StartCooldown() { m_remainingCooldown = m_cooldownLength; }

protected:
	// Collapsed Code
    /** The character who owns this ability*/
    UPROPERTY(BlueprintReadWrite)
        APorfCharacterBase* m_pAbilityOwner;
    /** The character hit by this ability*/
    UPROPERTY(BlueprintReadWrite)
        APorfCharacterBase* m_pPorfHit;

    /** The amount of time before this ability can be cast again*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_cooldownLength = 5.f;
    /** Whether or not this ability is currently being cast*/
    UPROPERTY(BlueprintReadWrite)
        bool m_casting = false;
    /** How long this ability last before it ends*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_abilityDuration;
    /** Whether or not this ability can be charged*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        bool m_isChargable = false;
    /** How long this ability has been charged*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        float m_timeCharged = 0.f;
    /** Whether or not this ability hit an opponent*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        bool m_hit = false;
    /** Whether or not this is a hold down ability*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunable Values")
        bool m_isHoldDownAbility = false;
    /** The amount of remaining time before this ability can be cast*/
    UPROPERTY(BlueprintReadOnly)
        float m_remainingCooldown = 0.f;

private:
	// Collapsed Code
    FTimerHandle m_abilityDurationHandle;
    FTimerHandle m_meleeDurationHandle;

	//Scott: adding this to allow multiple melee boxes.
	FName m_currentHitboxTag;

};
