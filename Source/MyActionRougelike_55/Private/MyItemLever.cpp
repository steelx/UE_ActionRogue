// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItemLever.h"

#include "MyExplosiveBarrel.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyItemLever::AMyItemLever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	LeverMesh->SetupAttachment(BaseMesh);
}

// Called when the game starts or when spawned
void AMyItemLever::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyItemLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyItemLever::Interact_Implementation(APawn* Caller)
{
	IMyGameplayInterface::Interact_Implementation(Caller);

	LeverMesh->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	
	if (TargetBarrel)
	{
		TargetBarrel->Explode();
	}
}

