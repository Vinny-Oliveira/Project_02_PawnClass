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
	UPROPERTY(VisibleAnywhere, BlueprintReadWritem, Category = "Enums")
		EMovementStatus MovementStatus {
		EMovementStatus::EMS_MAX
	};

	/** Speed for regular run */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed{};

	/** Speed for fast sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed{};

	bool bShiftKeyDown{};

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

	/** Getters for CameraBoom and FollowCamera */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void Die();

	void DecrementHealth(float Amount);

	void IncrementCoins(int32 Amount);

};
