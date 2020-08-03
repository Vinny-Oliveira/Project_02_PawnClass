// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8 {
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8 {
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	
	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PROJECT_01_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom{ nullptr };

	/** Camera that follows the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera{ nullptr };

	/** Rate of turning speed (%) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate{};
	
	/** Rate of looking up/down speed (%) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate{};

	/** Enum for the movement state of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus { EMovementStatus::EMS_Normal };
	
	/** Enum for the movement state of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus { EStaminaStatus::ESS_Normal };

	/** Speed for regular run */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed{};

	/** Speed for fast sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed{};

	/** Rate at which stamina is drained while the character sprints */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate{};
	
	/** Minimum stamina that allows sprinting after recovery */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina{};

	/** Boolean to check if the Shift key is being pressed or not */
	bool bShiftKeyDown{};

	/** TArray that stores the locations of the items picked up */
	TArray<FVector> PickupLocations{};

	/** Weapon equiped to the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon{ nullptr };

	/** Item the character is overlapping with */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem{ nullptr };

	/** Boolean to check if the left mouse button is being pressed */
	bool bLeftMouseBtnDown{};

	/** Boolean to check if the character is attacking */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bIsAttacking{};

	/** Reference to the montage with the combat animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage{ nullptr };

	/**
	/* Player Stats
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxStamina{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/// <summary>
	/// Move is a specific direction based on the camera and the axis (forward/backward or left/right)
	/// </summary>
	/// <param name="Value"></param>
	/// <param name="Axis"></param>
	void MoveInDirection(float Value, EAxis::Type Axis);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// <summary>
	/// Set movement status and running speed
	/// </summary>
	/// <param name="Status"></param>
	void SetMovementStatus(EMovementStatus Status);

	/// <summary>
	/// Setter for the StaminaStatus
	/// </summary>
	/// <param name="Status"></param>
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	/// <summary>
	/// Getter for the EquippedWeapon
	/// </summary>
	/// <returns></returns>
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	/// <summary>
	/// Setter for the Equipped Weapon
	/// </summary>
	/// <param name="WeaponToSet"></param>
	void SetEquippedWeapon(AWeapon* WeaponToSet);

	/// <summary>
	/// Setter for the Active Overlapping Item
	/// </summary>
	/// <param name="Item"></param>
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	/// <summary>
	/// Called for forward and backwards input
	/// </summary>
	/// <param name="value"></param>
	void MoveForward(float Value);

	/// <summary>
	/// Called for sideways input
	/// </summary>
	/// <param name="value"></param>
	void MoveRight(float Value);

	/// <summary>
	/// Called via input to turn at a given Rate
	/// </summary>
	/// <param name="Rate">This is a normalized turn rate</param>
	void TurnAtRate(float Rate);
	
	/// <summary>
	/// Called via input to look up/down at a given Rate
	/// </summary>
	/// <param name="Rate">This is a normalized look up/down rate</param>
	void LookUpAtRate(float Rate);

	/// <summary>
	/// Press shift down to enable sprint
	/// </summary>
	void ShiftKeyDown();
	
	/// <summary>
	/// Release shift to stop sprint
	/// </summary>
	void ShiftKeyUp();

	/// <summary>
	/// Call actions for when the left mouse button is pressed
	/// </summary>
	void LeftMouseBtnDown();
	
	/// <summary>
	/// Call actions for when the left mouse button is released
	/// </summary>
	void LeftMouseBtnUp();

	/// <summary>
	/// Handle how the status of the stamina bar
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleStaminaStatus(float DeltaTime);

	/// <summary>
	/// Getter for CameraBoom
	/// </summary>
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/// <summary>
	/// Getters for FollowCamera
	/// </summary>
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void Die();

	/// <summary>
	/// Decrease the health by an Amount
	/// </summary>
	/// <param name="Amount"></param>
	void DecrementHealth(float Amount);

	/// <summary>
	/// Increase the coins by an Amount
	/// </summary>
	/// <param name="Amount"></param>
	void IncrementCoins(int32 Amount);

	/// <summary>
	/// Display a debug sphere on the location of an item that has been picked up
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	/// <summary>
	/// Play attack animations and deal damage
	/// </summary>
	void Attack();

	/// <summary>
	/// Trigger this function when the attack animation finishes
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
};
