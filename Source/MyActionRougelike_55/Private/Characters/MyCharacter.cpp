// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MyCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MyInteractionComponent.h"
#include "MyMagicProjectile.h"
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

void AMyCharacter::SetIsPlayerAttacking(bool bAttacking)
{
	bIsAttacking = bAttacking;
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
}

void AMyCharacter::HandleMove(const FInputActionValue& Value)
{
	if (bIsAttacking)
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

void AMyCharacter::SpawnProjectile()
{
	if (!ProjectileClass)
	{
		return;
	}

	const FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("Muzzle_01"));
	const FTransform SpawnTransform = FTransform(GetControlRotation(), SpawnLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AMyMagicProjectile>(ProjectileClass, SpawnTransform, SpawnParams);
}

void AMyCharacter::HandlePrimaryInteract(const FInputActionValue& Value)
{
	InteractionComponent->PrimaryInteract();
}