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

	/** Enum of the movement status */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus{ EEnemyMovementStatus::EEMS_Idle };

	/** For when the enemy sees and chases the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere{ nullptr };
	
	/** For when the enemy can attack the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere{ nullptr };

	/** Enemy's AI Controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController{ nullptr };

	/** Checks if the Combat Sphere is being overlapped with */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere{};

	/** Target for the attack animation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* CombatTarget{ nullptr };

	/** Enemy's health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health{};

	/** Enemy's Maximum Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth{};

	/** The damage the enemy deals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage{};

	/** Particles emitted by the enemy when it is hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles{ nullptr };

	/** Sound played when the enemy is struck by a weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* StruckSound{ nullptr };
	
	/** Sound played when the enemy swings its claw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound{ nullptr };

	/** Box collider used for combat */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* CombatCollision{ nullptr };

	/** Enemy's combat montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage{ nullptr };

	/** Check if the enemy is attacking */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking{};

	/**
	/* Handlers of the enemy's attack delay
	*/

	FTimerHandle AttackTimer{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
 	float MinAttackTime{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxAttackTime{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// <summary>
	/// Set the movement status
	/// </summary>
	/// <param name="Status"></param>
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

	/// <summary>
	/// Event to detect when the Agro Sphere is overlapped */
	/// </summary>
	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// Event to detect when the Agro Sphere is no longer overlapped */
	/// </summary>
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/// <summary>
	/// Event to detect when the Combat Sphere is overlapped */
	/// </summary>
	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// Event to detect when the Combat Sphere is no longer overlapped */
	/// </summary>
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/// <summary>
	/// Make the enemy move to the targetted character
	/// </summary>
	/// <param name="Target"></param>
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	/// <summary>
	/// Gets the Character that is overlapping with the enemy
	/// </summary>
	UFUNCTION()
	AMain* GetValidCharacter(AActor* OtherActor);

	/// <summary>
	/// Detect when the combat box is overlapped
	/// </summary>
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// Detect when the combat box is no longer overlapped
	/// </summary>
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/// <summary>
	/// Allow the enemy to deal damage
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	/// <summary>
	/// Do not allow the enemy to deal damage anymore
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

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
