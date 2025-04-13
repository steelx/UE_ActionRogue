// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInteractionComponent.h"

#include "Interface/MyGameplayInterface.h"

// Sets default values for this component's properties
UMyInteractionComponent::UMyInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMyInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMyInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMyInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector End = EyeLocation + EyeRotation.Vector() * 1000.f;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	if (AActor* HitActor = Hit.GetActor())
	{
		// if (HitActor->Implements<UMyGameplayInterface>())
		// {
		// 	APawn* InstigatorPawn = Cast<APawn>(MyOwner);
		// 	IMyGameplayInterface::Execute_Interact(HitActor, InstigatorPawn);
		// }
		if (IMyGameplayInterface* GameplayInterface = Cast<IMyGameplayInterface>(HitActor))
		{
			APawn* InstigatorPawn = Cast<APawn>(MyOwner);
			GameplayInterface->Execute_Interact(HitActor, InstigatorPawn);
		}
	}
}

