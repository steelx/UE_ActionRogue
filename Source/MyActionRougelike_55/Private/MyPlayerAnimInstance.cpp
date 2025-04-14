// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerAnimInstance.h"

#include "Characters/MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UMyPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPlayerCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	if (OwningPlayerCharacter)
	{
		OwningPlayerMovementComponent = OwningPlayerCharacter->GetCharacterMovement();
	}
}

void UMyPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!OwningPlayerCharacter && !OwningPlayerMovementComponent)
	{
		return;
	}

	bIsAttacking = OwningPlayerCharacter->GetIsPlayerAttacking();

	if (!bShouldEnterAttackState && AttackTimeElapsed <= 0.f && bIsAttacking)
	{
		bShouldEnterAttackState = true;
	}

	if (bShouldEnterAttackState)
	{
		AttackTimeElapsed += DeltaSeconds;

		if (AttackTimeElapsed >= AttackTimeThreshold) {
			AttackTimeElapsed = 0.f;
			bShouldEnterAttackState = false;
			bIsAttacking = false;
			OwningPlayerCharacter->SetIsPlayerAttacking(false);
		}
	}

	CurrentVelocity = OwningPlayerCharacter->GetVelocity().Size();
	// Get Right vector
	CurrentRotation = UKismetMathLibrary::GetRightVector(OwningPlayerCharacter->GetControlRotation()).Size();
}
