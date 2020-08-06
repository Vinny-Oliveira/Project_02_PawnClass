// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Pickup		UMETA(DisplayName = "Pickup"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
	
	EWS_MAX			UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class PROJECT_01_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	/** Enum for the state of the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EWeaponState WeaponState{ EWeaponState::EWS_Pickup };

	/** Skeletal mesh of the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh{ nullptr };

	/** Sound played when the weapon is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound{ nullptr };

	/** Boolean to set if the weapon can emit particles or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bCanEmitParticles{};

	/** Box collider used for combat */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision{ nullptr };

public:

	/// <summary>
	/// Getter for the WeaponState
	/// </summary>
	/// <returns></returns>
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	/// <summary>
	/// Setter of the WeaponState
	/// </summary>
	/// <param name="State"></param>
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }

	/** Event to detect when the switch is stepped on */
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	/** Event to detect when the switch is no longer stepped on */
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	/// <summary>
	/// Equip the weapon to a Character' socket
	/// </summary>
	/// <param name="Character"></param>
	void Equip(class AMain* Character);
};
