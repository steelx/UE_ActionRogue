// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyPlayerAnimInstance.generated.h"

class UCharacterMovementComponent;
class AMyCharacter;
/**
 * 
 */
UCLASS()
class MYACTIONROUGELIKE_55_API UMyPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	AMyCharacter* OwningPlayerCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	UCharacterMovementComponent* OwningPlayerMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
	bool bShouldEnterAttackState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
	float AttackTimeThreshold = 1.f;
	
	float AttackTimeElapsed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
	float CurrentVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
	float CurrentRotation;

	UPROPERTY(BlueprintReadOnly, Category="AnimData|Jump State")
	bool bJumpStart = false;
	UPROPERTY(BlueprintReadOnly, Category="AnimData|Jump State")
	bool bJumping = false;
	UPROPERTY(BlueprintReadOnly, Category="AnimData|Jump State")
	bool bJumpFalling = false;
	UPROPERTY(BlueprintReadOnly, Category="AnimData|Jump State")
	bool bJumpEnded = false;

};
