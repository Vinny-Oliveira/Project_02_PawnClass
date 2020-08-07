// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AMain::AMain()
	: BaseTurnRate{ 65.f },
	BaseLookUpRate{ 65.f },
	MaxHealth{ 100.f },
	Health{ 65.f },
	MaxStamina{ 150.f },
	Stamina{ 120.f },
	Coins{ 0 },
	RunningSpeed{ 650.f },
	SprintingSpeed{ 950.f },
	bShiftKeyDown{ false },
	bLeftMouseBtnDown{ false },
	bIsAttacking{ false },
	MovementStatus{ EMovementStatus::EMS_Normal },
	StaminaStatus{ EStaminaStatus::ESS_Normal },
	StaminaDrainRate{ 25.f },
	MinSprintStamina{ 50.f } {

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera fllows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Do not rotate this camera

	// Do not rotate when the controller rotates. That should just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // Rotation rate for the character movement
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleStaminaStatus(DeltaTime);
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent); // This macro stops the function execution if the PlayerInputComponent is not valid

	// Bind movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
	
	// Bind turns
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Bind turn rates
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	// Bind jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	// Bind sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);
	
	// Bind sprint
	PlayerInputComponent->BindAction("LeftMouseBtn", IE_Pressed, this, &AMain::LeftMouseBtnDown);
	PlayerInputComponent->BindAction("LeftMouseBtn", IE_Released, this, &AMain::LeftMouseBtnUp);


}

void AMain::MoveInDirection(float Value, EAxis::Type Axis) {
	if ((Controller != nullptr) && (Value != 0.f) && (!bIsAttacking)) {
		// Find out which way is forward (where the camera is pointing to)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// Move in that direction
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(Axis);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveForward(float Value) {
	AMain::MoveInDirection(Value, EAxis::X);
}

void AMain::MoveRight(float Value) {
	AMain::MoveInDirection(Value, EAxis::Y);
}

void AMain::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::Die() {
	UE_LOG(LogTemp, Warning, TEXT("DEAD"));
}

void AMain::DecrementHealth(float Amount) {
	Health -= Amount;

	// Check if the character dies
	if (Health <= 0.f) {
		Health = 0.f;
		Die();
	}
}

void AMain::IncrementCoins(int32 Amount) {
	Coins += Amount;
}

void AMain::SetMovementStatus(EMovementStatus Status) {
	MovementStatus = Status;

	if (MovementStatus == EMovementStatus::EMS_Sprinting) {
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	} else {
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShiftKeyDown() {
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp() {
	bShiftKeyDown = false;
}

void AMain::HandleStaminaStatus(float DeltaTime) {
	float DeltaStamina{ StaminaDrainRate * DeltaTime };
	float CurrentSpeed{ GetVelocity().Size() };
	bool bIsInAir{ GetMovementComponent()->IsFalling() };

	switch (StaminaStatus) {
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown && (CurrentSpeed > 0.f) && !bIsInAir) { // Decrease stamina and sprint
			Stamina -= DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Sprinting);

			// Threshhold to change color
			if (Stamina < MinSprintStamina) {
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}

		} else { // Replenish stamina and do not sprint
			Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
			if (Stamina > MaxStamina) {
				Stamina = MaxStamina;
			}
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown && (CurrentSpeed > 0.f) && !bIsInAir) { // Decrease stamina and sprint
			Stamina -= DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Sprinting);

			// Character exhausted
			if (Stamina < 0.f) {
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				SetMovementStatus(EMovementStatus::EMS_Normal);
				Stamina = 0.f;
			}

		} else { // Replenish stamina and do not sprint
			Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
			if (Stamina > MinSprintStamina) {
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		// Do not sprint, zero out the Stamina if shift is pressed, and go to recovery state if shift key is let go
		SetMovementStatus(EMovementStatus::EMS_Normal);
		if (bShiftKeyDown && (CurrentSpeed > 0.f) && !bIsInAir) {
			Stamina = 0.f;
		} else {
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
		}
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		// Replenish stamina and get back to normal if past the threshold
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		if (Stamina > MinSprintStamina) {
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		break;

	default:
		break;
	}
}

void AMain::ShowPickupLocations() {
	//for (int32 i{ 0 }; i < PickupLocations.Num(); i++) { // TArray uses Num() to collect the number of items
	for (auto Location : PickupLocations) {
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Red, 5.f, 2.f);
	}
	
}

void AMain::LeftMouseBtnDown() {
	bLeftMouseBtnDown = true;
	if (ActiveOverlappingItem) {
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon) {
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	} else if (EquippedWeapon) { 
		Attack();
	}
}

void AMain::LeftMouseBtnUp() {
	bLeftMouseBtnDown = false;
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet) { 
	if (EquippedWeapon) {
		EquippedWeapon->Destroy();
	}
	
	EquippedWeapon = WeaponToSet;
}

void AMain::Attack() {
	if (!bIsAttacking) { // Do not attack if an attack animation is playing
		bIsAttacking = true;

		// Play attack animation
		UAnimInstance* AnimInstance{ GetMesh()->GetAnimInstance() };
		if (AnimInstance && CombatMontage) {
			int32 Section{ FMath::RandRange(0, 1) };
			FName AnimName;

			// Choose an attack animation randomly
			switch (Section) {
			case 0:
				AnimName = FName("Attack_01");
				break;

			case 1:
				AnimName = FName("Attack_02");
				break;

			default:
				AnimName = FName("Attack_01");
				break;
			}

			AnimInstance->Montage_Play(CombatMontage, 2.f);
			AnimInstance->Montage_JumpToSection(AnimName, CombatMontage);
		}
	}
}

void AMain::AttackEnd() {
	bIsAttacking = false;
	if (bLeftMouseBtnDown) {
		Attack(); // Attack again if the button is held down
	}
}

void AMain::PlaySwingSound() {
	if (EquippedWeapon->SwingSound) {
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}