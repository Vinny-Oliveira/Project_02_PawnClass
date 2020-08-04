// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8 {
	EEMS_Idle			UMETA(DisplayName = "Idle"),
	EEMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EEMS_Attacking		UMETA(DisplayName = "Attacking"),

	EEMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class PROJECT_01_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus{ EEnemyMovementStatus::EEMS_Idle };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
};
