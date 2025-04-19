// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/HighlightInterface.h"
#include "Interface/MyGameplayInterface.h"
#include "MyItemLever.generated.h"

class USphereComponent;
class AMyExplosiveBarrel;

UCLASS()
class MYACTIONROUGELIKE_55_API AMyItemLever : public AActor, public IMyGameplayInterface, public IHighlightInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItemLever();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Interact_Implementation(APawn* Caller) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeverMesh;

	UPROPERTY(EditInstanceOnly, Category = "Interaction")
	TObjectPtr<AMyExplosiveBarrel> TargetBarrel;

};
