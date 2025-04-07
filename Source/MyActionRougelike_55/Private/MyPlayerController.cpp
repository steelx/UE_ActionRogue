
#include "MyPlayerController.h"


#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
//#include "Interface/HighlightInterface.h"


AMyPlayerController::AMyPlayerController()
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	checkf(InputContext, TEXT("IMC InputContext not found!"));

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	//CursorTrace();
}

// void AMyPlayerController::CursorTrace()
// {
// 	FHitResult CursorHit;
// 	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
// 	if (!CursorHit.bBlockingHit) return;
//
// 	LastActor = ThisActor;
// 	ThisActor = Cast<IHighlightInterface>(CursorHit.GetActor());
//
// 	/**
// 	 * Highlight scenarios if;
// 	 * A. LastActor & ThisActor is null
// 	 *		- Do Nothing
// 	 * B. LastActor is null, ThisActor is valid
// 	 *		- Highlight ThisActor
// 	 * C. LastActor is valid, ThisActor is null
// 	 *		- UnHighlight LastActor
// 	 * D. Both are valid, but LastActor != ThisActor
// 	 *		- UnHighlight LastActor & Highlight ThisActor
// 	 * E. Both are valid, but LastActor == ThisActor
// 	 *		- Do Nothing
// 	 */
// 	if (LastActor == nullptr)
// 	{
// 		if (ThisActor != nullptr)
// 		{
// 			// Case B
// 			ThisActor->HighlightActor();
// 		}
// 		// else Case A - Do nothing
// 	} else// LastActor is valid
// 	{
// 		if (ThisActor == nullptr)
// 		{
// 			// Case C
// 			LastActor->UnHighlightActor();
// 		} else
// 		{
// 			// Case D Both are valid
// 			if (LastActor != ThisActor)
// 			{
// 				LastActor->UnHighlightActor();
// 				ThisActor->HighlightActor();
// 			}
// 			// else Case E - Do nothing
// 		}
// 	}
// }

void AMyPlayerController::HandleMove(const FInputActionValue& Value)
{
	FVector2d InputAxisVector = Value.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
