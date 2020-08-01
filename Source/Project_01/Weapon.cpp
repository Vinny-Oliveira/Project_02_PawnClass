// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
	: bCanEmitParticles{ false }, WeaponState{ EWeaponState::EWS_Pickup } {
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (GetWeaponState() == EWeaponState::EWS_Pickup) {
		if (AMain* Main{ GetValidCharacter(OtherActor) }) {
			Main->SetActiveOverlappingItem(this);
		}
	}

}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (AMain* Main{ GetValidCharacter(OtherActor) }) {
		Main->SetActiveOverlappingItem(nullptr);
	}
}

void AWeapon::Equip(AMain* Character) {
	if (Character) {
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // Camera does not zoom in if the sword is in the way
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetSimulatePhysics(false);

		// Attach the weapon to the character's socket
		const USkeletalMeshSocket* RightHandSocket{ Character->GetMesh()->GetSocketByName("RightHandSocket") };
		if (RightHandSocket) {
			//Character->GetEquippedWeapon()->Destroy();
			RightHandSocket->AttachActor(this, Character->GetMesh());
			bCanRotate = false;
			Character->SetEquippedWeapon(this);
			Character->SetActiveOverlappingItem(nullptr);
			SetWeaponState(EWeaponState::EWS_Equipped);

			// Play a sound when the weapon is equipped
			if (OnEquipSound) {
				UGameplayStatics::PlaySound2D(this, OnEquipSound);
			}

			if (!bCanEmitParticles) {
				IdleParticlesComponent->Deactivate();
			}
		}
	}
}