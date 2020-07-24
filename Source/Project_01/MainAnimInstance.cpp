// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"

void UMainAnimInstance::CheckForPawn() {
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
	}
}

void UMainAnimInstance::NativeInitializeAnimation() {
	UMainAnimInstance::CheckForPawn();
}

void UMainAnimInstance::UpdateAnimationProperties() {
	UMainAnimInstance::CheckForPawn();

}