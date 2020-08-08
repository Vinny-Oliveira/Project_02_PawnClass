// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Weapon.h"

// Sets default values
AEnemy::AEnemy()
	: bOverlappingCombatSphere{ false },
	Health{ 75.f },
	MaxHealth{ 100.f },
	Damage{ 10.f },
	bAttacking{ false },
	MinAttackTime{ 0.5f },
	MaxAttackTime{ 1.2f },
	EnemyMovementStatus{ EEnemyMovementStatus::EEMS_Idle },
	DestroyDelay{ 3.f } {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);
	
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Add the AI Controller
	AIController = Cast<AAIController>(GetController());

	// Bind the overlapping actions
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
	
	// Bind the overlapping actions
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
	
	// Bind the overlapping actions
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	// Have the combat collision detect overlap collision with Pawns
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!IsAlive()) {
		return;
	}
	
	if (AMain* Main{ GetValidCharacter(OtherActor) }) {
		MoveToTarget(Main);
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (!IsAlive()) {
		return;
	}
	
	if (AMain* Main{ GetValidCharacter(OtherActor) }) {
		// Hide health bar
		// Display Enemy health bar
		if (Main->MainPlayerController) {
			Main->MainPlayerController->HideEnemyHealthBar();
		}

		// Stop movement
		SetEnemyMovementStatus(EEnemyMovementStatus::EEMS_Idle);
		if (AIController) {
			AIController->StopMovement();
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!IsAlive()) {
		return;
	}
	
	if (AMain* Main{ GetValidCharacter(OtherActor) }) {
		// Prepare attack state
		bOverlappingCombatSphere = true;
		SetEnemyMovementStatus(EEnemyMovementStatus::EEMS_Attacking);
		CombatTarget = Main;
		Main->SetCombatTarget(this);
		
		// Display Enemy health bar
		if (Main->MainPlayerController) {
			Main->MainPlayerController->DisplayEnemyHealthBar();
		}
		
		Attack();
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (!IsAlive()) {
		return;
	}
	
	if (AMain* Main{ GetValidCharacter(OtherActor) }) {
		bOverlappingCombatSphere = false;

		// Clear the timer of the attack
		GetWorldTimerManager().ClearTimer(AttackTimer);

		if (EnemyMovementStatus != EEnemyMovementStatus::EEMS_Attacking) {
			MoveToTarget(Main);
			CombatTarget = nullptr;
			
			// Null out the Main's CombatTarget if its currrent one is this Enemy
			if (Main->CombatTarget == this) {
				Main->SetCombatTarget(nullptr);
			}
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target) {
	//UE_LOG(LogTemp, Warning, TEXT("MoveToTarget Called"));
	SetEnemyMovementStatus(EEnemyMovementStatus::EEMS_MoveToTarget);

	if (AIController) {
		// Use built-in functions to program the AI's movement
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		/** Display the path taken by the enemy */
		//TArray<FNavPathPoint> PathPoints{ NavPath->GetPathPoints() };
		auto PathPoints{ NavPath->GetPathPoints() };

		for (auto Point : PathPoints) {
			FVector Location = Point.Location;
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Red, 5.f, 2.f);
		}
	}
}

AMain* AEnemy::GetValidCharacter(AActor* OtherActor) {
	if (OtherActor) {
		AMain* Main{ Cast<AMain>(OtherActor) };
		if (Main) {
			return Main;
		}
	}

	return nullptr;
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!IsAlive()) {
		return;
	}
	
	if (AMain* Main{ GetValidCharacter(OtherActor) }) {
		// Particles
		if (Main->HitParticles) {

			// Spawn the particles at the Weapon Socket, a location by the blade
			const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName("TipSocket") };
			if (TipSocket) {
				FVector SocketLocation{ TipSocket->GetSocketLocation(GetMesh()) };
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
			}
		}

		// Hit sound
		if (Main->StruckSound) {
			UGameplayStatics::PlaySound2D(this, Main->StruckSound);
		}

		// Apply Damage
		if (DamageTypeClass) {
			UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (!IsAlive()) {
		return;
	}
}

void AEnemy::ActivateCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (SwingSound) {
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack() {
	if (!IsAlive()) {
		return;
	}

	if (AIController) {
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EEMS_Attacking);
	}

	if (!bAttacking) {
		bAttacking = true;

		// Play attack animation
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage) {
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}
	}
}

void AEnemy::AttackEnd() {
	bAttacking = false;

	// Attack again if still next to the character
	if (bOverlappingCombatSphere) {
		float AttackTime = FMath::FRandRange(MinAttackTime, MaxAttackTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
	Health -= DamageAmount;
	if (Health <= 0.f) {
		Health = 0.f;
		Die(DamageCauser);
	}

	return DamageAmount;
}

void AEnemy::Die(AActor* DamageCauser) {
	UAnimInstance* AnimInstance{ GetMesh()->GetAnimInstance() };
	// Play death animation
	if (AnimInstance && CombatMontage) {
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}

	SetEnemyMovementStatus(EEnemyMovementStatus::EEMS_Dead);

	// Hide the health bar
	if (DamageCauser) {
		AWeapon* Weapon = Cast<AWeapon>(DamageCauser);
		if (Weapon) {
			AMain* Main = Weapon->Wielder;
			if (Main->CombatTarget == this) {
				Main->CombatTarget = nullptr;
				Main->MainPlayerController->HideEnemyHealthBar();
			}
		}
	}

	// Deactivate enemy collision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DeathEnd() {
	//UE_LOG(LogTemp, Warning, TEXT("Enemy Dead"));

	// Freeze position and animation
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	// Destroy the enemy in the world
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy::Disappear, DestroyDelay);
}

void AEnemy::Disappear() {
	UE_LOG(LogTemp, Warning, TEXT("Disappear"));
	Destroy();
}