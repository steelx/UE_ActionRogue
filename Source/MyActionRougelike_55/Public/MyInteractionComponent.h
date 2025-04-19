// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyInteractionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYACTIONROUGELIKE_55_API UMyInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyInteractionComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrimaryInteract();

	// Get the currently focused actor
	AActor* GetFocusedActor() const { return FocusedActor; }


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Trace distance for interaction
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float TraceDistance = 1000.0f;

	// Highlight material to apply to interactable objects
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInstance* HighlightMaterial;

	// Last actor that was highlighted
	TObjectPtr<AActor> FocusedActor;

	// Check if an actor can be interacted with
	bool IsInteractable(AActor* Actor);

	// Line trace to find interactable objects
	void FindInteractable();

	// Original materials of meshes (to restore when no longer focusing)
	TMap<UPrimitiveComponent*, UMaterialInterface*> OriginalMaterials;
    
	// Apply highlight to an actor
	void ApplyHighlight(AActor* Actor, bool bShouldHighlight);

private:
	
};
