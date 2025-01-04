// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"

#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Items/ItemDataStruct.h"
#include "ResidentInventory/ResidentInventoryCharacter.h"


// Sets default values
APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickUp Actor Mesh");
	PickupStaticMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupStaticMesh);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();	
}

void APickup::BeginFocus()
{
	if(PickupStaticMesh)
	{
		// this creates a layer on the top of the mesh and applies the material
		PickupStaticMesh->SetRenderCustomDepth(true);
	}	
}

void APickup::EndFocus()
{
	if(PickupStaticMesh)
	{
		PickupStaticMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact(AResidentInventoryCharacter* PlayerCharacter)
{
	if(PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}


void APickup::TakePickup(const AResidentInventoryCharacter* PickupTaker)
{
	// checks if this actor is on the way for destruction because after the player picks it up
	// we will destroy the actor from the world, so if the player spams the equip Key
	// it won't run this function again if the item is already picked up
	if(!IsPendingKillPending())
	{
		if(ItemReference)
		{
			// if(UInventoryComponent* PlayerInventory = PickupTaker->GetInventory())
			// {
			// 	const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);
			// 	
			// 	switch (AddResult.OperationResult) {
			// 	case EItemAddResult::IAR_NoItemAdded:
			// 		break;
			// 	case EItemAddResult::IAR_PartialAmountItemAdded:					
			// 		PickupTaker->UpdateInteractionWidget();
			// 		break;
			// 	case EItemAddResult::IAR_AllItemAdded:
			// 		Destroy();
			// 		break;
			// 	default: ;
			// 	}
			// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
		 // 	}
			// else
			// {
			// 	UE_LOG(LogTemp, Warning, TEXT("Inventory Component Null"));
			// }
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup Item Reference is Null"));
		 }
	}
}

void APickup::OnPickupDataReceived() const
{
	if (ItemReference->Item->PickupStaticMesh)
	{
		PickupStaticMesh->SetStaticMesh(ItemReference->Item->PickupStaticMesh);
	}
}

void APickup::SetPickupData(UItemBase* InItemInstance, int32 InQuantity)
{
	ItemReference = InItemInstance;
	PickupItemQuantity = InQuantity;

	OnPickupDataReceived();
	K2_OnPickupDataReceived();
}

void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.GetPropertyName() : NAME_None;
	
	if(ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, PickupStaticMesh))
	{
		if(ItemReference)
		{
			PickupStaticMesh->SetStaticMesh(ItemReference->Item->PickupStaticMesh);			
		}
	}
}






