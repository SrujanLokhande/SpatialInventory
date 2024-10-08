// Fill out your copyright notice in the Description page of Project Settings.


#include "World/InterfaceTestActor.h"


AInterfaceTestActor::AInterfaceTestActor()
{	
	PrimaryActorTick.bCanEverTick = true;

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(ActorMesh);
}

void AInterfaceTestActor::BeginPlay()
{
	Super::BeginPlay();

	// to make the interactable data interface class communicate with this class
	InteractableData = InstanceInteractableData;	
}

// Called every frame
void AInterfaceTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInterfaceTestActor::BeginFocus()
{
	if(ActorMesh)
	{
		// this creates a layer on the top of the mesh and applies the material
		// which we provide on the top of the mesh
		ActorMesh->SetRenderCustomDepth(true);
	}
}

void AInterfaceTestActor::EndFocus()
{
	if(ActorMesh)
	{
		ActorMesh->SetRenderCustomDepth(false);
	}
}

void AInterfaceTestActor::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Begin Interact on Test Actor"));
}

void AInterfaceTestActor::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling End Interact on Test Actor"));
}

void AInterfaceTestActor::Interact(AResidentInventoryCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact on Test Actor"));
}

