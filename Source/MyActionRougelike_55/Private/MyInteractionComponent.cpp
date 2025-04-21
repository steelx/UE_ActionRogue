// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
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

/**
 * Performs a line trace from the player's camera (or eye position) to find interactable objects
 * and highlights them when targeted.
 *
 * A line trace is performed from the camera component (preferred) or actor eye position,
 * checking for objects implementing UMyGameplayInterface.
 * When a new interactable object is found, the previous highlight is removed and the new
 * object is highlighted via the ApplyHighlight() function.
 *
 * @note Relies on UCameraComponent for most accurate targeting. Falls back to actor eye position if no camera found.
 * @note Objects must implement UMyGameplayInterface to be interactable and UHighlightInterface to be highlighted.
 */
void UMyInteractionComponent::FindInteractable()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner || !GetWorld())
	{
		return;
	}

	FVector TraceStart;
	FVector TraceDirection;

	// Try to get the camera component from the owning character
	const ACharacter* MyCharacter = Cast<ACharacter>(MyOwner);

	if (const UCameraComponent* CameraComp = MyCharacter ? MyCharacter->FindComponentByClass<UCameraComponent>() : nullptr)
	{
		// Use Camera location and rotation for the trace
		TraceStart = CameraComp->GetComponentLocation();
		TraceDirection = CameraComp->GetComponentRotation().Vector();
	}
	else
	{
		// Fallback: Use owner's eye view point if camera is not found (less accurate for 3rd person)
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(TraceStart, EyeRotation);
		TraceDirection = EyeRotation.Vector();
		UE_LOG(LogTemp, Warning, TEXT("Interaction trace falling back to GetActorEyesViewPoint. Ensure Owning Actor is AMyCharacter with a UCameraComponent for best results."));
	}

	const FVector TraceEnd = TraceStart + (TraceDirection * TraceDistance);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // Make sure this channel includes interactable objects

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner); // Ignore the owner
	QueryParams.bTraceComplex = true; // Or false depending on needs

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		TraceStart,
		TraceEnd,
		ObjectQueryParams,
		QueryParams
	);

	AActor* HitActor = Hit.GetActor();

	// If we're no longer looking at the same interactable actor, update highlight
	if (FocusedActor != HitActor)
	{
		// Clear highlight on the previously focused actor (if it was interactable)
		if (FocusedActor && FocusedActor->Implements<UHighlightInterface>())
		{
			ApplyHighlight(FocusedActor, false);
		}

		// Set new focused actor only if it's interactable
		FocusedActor = IsInteractable(HitActor) ? HitActor : nullptr;

		// Apply highlight to the new focused actor (if it's valid and interactable)
		if (FocusedActor && FocusedActor->Implements<UHighlightInterface>())
		{
			ApplyHighlight(FocusedActor, true);
		}
	}

	// Optional: Draw debug line to show where we're looking
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FocusedActor ? FColor::Cyan : FColor::Red, false, 0.0f, 0, 1.0f);
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
				MeshComp->SetMaterial(0, HighlightMaterial);
				// MeshComp->SetOverlayMaterial(HighlightMaterial);
			}
		}
		else
		{
			// Restore original materials
			if (UMaterialInterface* OriginalMat = OriginalMaterials.FindRef(MeshComp))
			{
				MeshComp->SetMaterial(0, OriginalMat);
				OriginalMaterials.Remove(MeshComp);
				// MeshComp->SetOverlayMaterial(nullptr);
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

