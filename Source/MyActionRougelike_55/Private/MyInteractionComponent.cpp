// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInteractionComponent.h"

#include "Interface/HighlightInterface.h"
#include "Interface/MyGameplayInterface.h"

// Sets default values for this component's properties
UMyInteractionComponent::UMyInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	FocusedActor = nullptr;
}


// Called when the game starts
void UMyInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UMyInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Look for objects to highlight each frame
	FindInteractable();
}

bool UMyInteractionComponent::IsInteractable(AActor* Actor)
{
	return Actor && (Actor->Implements<UMyGameplayInterface>());
}

void UMyInteractionComponent::FindInteractable()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector End = EyeLocation + EyeRotation.Vector() * TraceDistance;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	// If we're no longer looking at the same actor, unhighlight the old one
	if (AActor* HitActor = Hit.GetActor(); FocusedActor != HitActor)
	{
		// Clear highlight on a previously focused actor
		if (FocusedActor && FocusedActor->Implements<UHighlightInterface>())
		{
			ApplyHighlight(FocusedActor, false);
		}
        
		// Set new focused actor
		FocusedActor = IsInteractable(HitActor) ? HitActor : nullptr;
        
		// Apply highlight to new focused actor
		if (FocusedActor && FocusedActor->Implements<UHighlightInterface>())
		{
			ApplyHighlight(FocusedActor, true);
		}
	}
    
	// Optional: Draw debug line to show where we're looking
	// DrawDebugLine(GetWorld(), EyeLocation, End, FColor::Red, false, 0.0f, 0, 2.0f);
}

void UMyInteractionComponent::ApplyHighlight(AActor* Actor, bool bShouldHighlight)
{
	if (!Actor || !HighlightMaterial)
	{
		return;
	}

	// Find all static mesh components on the actor
	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

	if (MeshComponents.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No StaticMeshComponents found on %s"), *Actor->GetName());
		return;
	}
    
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		// Apply or remove the overlay material
		if (bShouldHighlight)
		{
			if (UMaterialInterface* OriginalMat = MeshComp->GetMaterial(0))
			{
				OriginalMaterials.Add(MeshComp, OriginalMat);
                    
				// Apply highlight material to all material slots
				MeshComp->SetMaterial(0, HighlightMaterial);
			}
		}
		else
		{
			// Restore original materials
			if (UMaterialInterface* OriginalMat = OriginalMaterials.FindRef(MeshComp))
			{
				MeshComp->SetMaterial(0, OriginalMat);
				OriginalMaterials.Remove(MeshComp);
			}
		}
	}
}

void UMyInteractionComponent::PrimaryInteract()
{
	if (FocusedActor && FocusedActor->Implements<UMyGameplayInterface>())
	{
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		IMyGameplayInterface::Execute_Interact(FocusedActor, InstigatorPawn);
	}
}

