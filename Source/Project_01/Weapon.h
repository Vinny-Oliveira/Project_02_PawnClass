// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_01_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	/** Skeletal mesh of the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh{ nullptr };

	/** Sound played when the weapon is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound{ nullptr };

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
