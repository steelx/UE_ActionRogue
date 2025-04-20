// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MyCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MyInteractionComponent.h"
#include "MyMagicProjectile.h"
#include "Components/ArrowComponent.h"
//#include "Interface/HighlightInterface.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 200.f;
	// Camera above the character with a right offset
	SpringArmComp->SocketOffset = FVector(0.f, 55.f, 65.f);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	InteractionComponent = CreateDefaultSubobject<UMyInteractionComponent>(TEXT("InteractionComponent"));
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    checkf(InputContext, TEXT("IMC InputContext not found!"));

    // Get the player controller
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
    	PlayerController->bShowMouseCursor = false;
        // Get the local player from the player controller
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(InputContext, 0);
            }
        }

        // Set input mode if needed
        FInputModeGameAndUI InputModeData;
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputModeData.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(InputModeData);
    }
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->IsFalling())
	{
		bJumpStart = false;
		bJumping = true;
		bJumpFalling = GetVelocity().Z < 0.f;
		bJumpEnded = false;
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLook);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::HandleFire);
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::HandlePrimaryInteract);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
}

void AMyCharacter::HandleMove(const FInputActionValue& Value)
{
	if (bIsAttacking || GetCharacterMovement()->IsFalling())
	{
		return;
	}
	const FVector2d InputAxisVector = Value.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InputAxisVector.Y);
	AddMovementInput(RightDirection, InputAxisVector.X);
}

void AMyCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2d LookAxisVector = Value.Get<FVector2d>();
    
	// the Y-axis is typically inverted for mouse look
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y * -1.0f); // Invert Y-axis for more natural mouse control
}

void AMyCharacter::HandleFire(const FInputActionValue& Value)
{
	if (!ProjectileClass)
	{
		return;
	}

	SetIsPlayerAttacking(true);
	GetWorldTimerManager().SetTimer(TimerHandle_PlayAttackAnim, this, &ThisClass::SpawnProjectile, 0.2, false);
	// GetWorldTimerManager().ClearTimer(TimerHandle_PlayAttackAnim);
}

/*
* Problem:
Projectile is spawned at Muzzle_01 player hand which can be left or right based on orientation
Camera Perspective; aiming at something close will miss the target due to camera alignment with character.
* Solution:
Line trace from Camera to World and find the desired impact location.
 */
void AMyCharacter::SpawnProjectile()
{
	if (!ProjectileClass || !GetWorld() || !CameraComp)
	{
		return;
	}

	// 1. Get camera location and forward vector
	const FVector CameraLocation = CameraComp->GetComponentLocation();
	const FRotator CameraRotation = CameraComp->GetComponentRotation(); // Or use GetControlRotation() if SpringArm uses PawnControlRotation
	const FVector TraceDirection = CameraRotation.Vector();
	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = TraceStart + (TraceDirection * TraceDistance);

	// 2. Perform Line Trace
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the character itself
	QueryParams.bTraceComplex = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility, // Or your preferred collision channel for aiming
		QueryParams
	);

	// 3. Determine Target Location
	const FVector TargetLocation = bHit ? HitResult.ImpactPoint : TraceEnd;

	// Optional: Draw debug line to visualize the trace
	// DrawDebugLine(GetWorld(), TraceStart, TargetLocation, FColor::Green, false, 2.0f, 0, 1.0f);

	// 4. Calculate spawn location and the direction towards the target
	const FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("Muzzle_01"));
	const FVector DirectionToTarget = (TargetLocation - SpawnLocation).GetSafeNormal();
	const FRotator SpawnRotation = DirectionToTarget.Rotation();

	// 5. Spawn Projectile
	const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator(); // Set the instigator if needed
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Or AdjustIfPossibleButAlwaysSpawn

	GetWorld()->SpawnActor<AMyMagicProjectile>(ProjectileClass, SpawnTransform, SpawnParams);
}


void AMyCharacter::HandlePrimaryInteract(const FInputActionValue& Value)
{
	InteractionComponent->PrimaryInteract();
}


void AMyCharacter::SetIsPlayerAttacking(bool bAttacking)
{
	bIsAttacking = bAttacking;
}

void AMyCharacter::Jump()
{
	Super::Jump();
	bJumpStart = true;

}
/*
- **Landed** is a **virtual function** in the `ACharacter` class (from Unreal Engine's C++ API).
- The engine automatically calls `Landed()` whenever your character hits the ground after falling or jumping.
 */
void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bJumpStart = false;
	bJumping = false;
	bJumpFalling = false;
	// bJumpEnded = true;
	// OR: Reset after a short delay
	GetWorldTimerManager().SetTimerForNextTick([this](){bJumpEnded = true;});
	// Or use a specific delay if needed
	// GetWorldTimerManager().SetTimer(LandingDelayHandle, this, &AMyCharacter::ClearJumpEnded, 0.2f, false);
}

void AMyCharacter::GetJumpState(bool& bOutJumpStart, bool& bOutJumping, bool& bOutJumpFalling, bool&bOutJumpEnded) const
{
	bOutJumpStart = bJumpStart;
	bOutJumping = bJumping;
	bOutJumpFalling = bJumpFalling;
	bOutJumpEnded = bJumpEnded;
}
