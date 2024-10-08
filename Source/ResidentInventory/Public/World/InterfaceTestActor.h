// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "InterfaceTestActor.generated.h"

UCLASS()
class RESIDENTINVENTORY_API AInterfaceTestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:	
	AInterfaceTestActor();

protected:
	
	UPROPERTY(EditAnywhere, Category = "Test Actor")
	UStaticMeshComponent* ActorMesh;

	UPROPERTY(EditInstanceOnly, Category = "Test Actor")
	FInteractableData InstanceInteractableData;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(AResidentInventoryCharacter* PlayerCharacter) override;
};
