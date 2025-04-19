// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UMyInteractionComponent;
class UCameraComponent;
class USpringArmComponent;
class IHighlightInterface;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UCLASS()
class MYACTIONROUGELIKE_55_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	inline bool GetIsPlayerAttacking() const { return bIsAttacking; }
	inline bool GetIsPlayerInteracting() const { return bIsInteracting; }
	void SetIsPlayerAttacking(bool bIsAttacking);
	
	void Jump();
	void Landed(const FHitResult& Hit);
	UFUNCTION(BlueprintPure)
	void GetJumpState(bool& bOutJumpStart, bool& bOutJumping, bool& bOutJumpFalling, bool& bOutJumpEnded) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMyInteractionComponent* InteractionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations")
	bool bIsInteracting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations")
	bool bIsAttacking;

	FTimerHandle TimerHandle_PlayAttackAnim;

private:
	bool bJumpStart = false;
	bool bJumping = false;
	bool bJumpFalling = false;
	bool bJumpEnded = false;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PrimaryInteractAction;

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleFire(const FInputActionValue& Value);
	void SpawnProjectile();
	void HandlePrimaryInteract(const FInputActionValue& Value);

};
