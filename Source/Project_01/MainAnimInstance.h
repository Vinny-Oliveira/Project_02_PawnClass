// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_01_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement)
	float MovementSpeed{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	APawn* Pawn{ nullptr };

	virtual void NativeInitializeAnimation() override;

	void UpdateAnimationProperties();

private:

	void CheckForPawn();
};
