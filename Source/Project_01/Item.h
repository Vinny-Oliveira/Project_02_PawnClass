// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class PROJECT_01_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	/** Base shape collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent* CollisionVolume{ nullptr };

	/** Base Mesh Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	UStaticMeshComponent* Mesh{ nullptr };

	/** Idle Particle component of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystemComponent* IdleParticlesComponent{ nullptr };

	/** Particles when the item is overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystem* OverlapParticles{ nullptr };

	/** Sound played when the item is overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue* OverlapSound{ nullptr };

	/** Boolean to check if the item is supposed to rotate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Properties")
	bool bCanRotate{};

	/** Rate at which the item rotates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Properties")
	float RotationRate{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Event to detect when the switch is stepped on */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** Event to detect when the switch is no longer stepped on */
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//class AMain* Main { nullptr };

	//UFUNCTION()
	//void ValidateActors(AActor* OtherActor, AMain::Function* Func, float InputOfFunc);
};
