// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"

#include "Items/ItemBase.h"


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

	InitializePickup(UItemBase::StaticClass(), PickupItemQuantity);
}

void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, int32 ItemQuantity)
{
	if(ItemDataTable && !DesiredID.IsNone())
	{
		//struct from the data Table
		const FItemData* ItemDataRow = ItemDataTable->FindRow<FItemData>(DesiredID, DesiredID.ToString());

		ItemReference = NewObject<UItemBase>(this, BaseClass);
		
		ItemReference->ID = ItemDataRow->ID;
		ItemReference->ItemType = ItemDataRow->ItemType;
		ItemReference->ItemRarity = ItemDataRow->ItemRarity;
		ItemReference->NumericData = ItemDataRow->NumericData;
		ItemReference->TextData = ItemDataRow->TextData;
		ItemReference->AssetData = ItemDataRow->AssetData;
		
		ItemQuantity <=0 ? ItemReference->SetItemQuantity(1) : ItemReference->SetItemQuantity(ItemQuantity);

		PickupStaticMesh->SetStaticMesh(ItemDataRow->AssetData.ItemMesh);

		UpdateInteractableData();
	}
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Quantity = ItemReference->ItemQuantity;
	InteractableData = InstanceInteractableData;
}

void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference =ItemToDrop;	
	PickupItemQuantity <=0 ? ItemReference->SetItemQuantity(1) : ItemReference->SetItemQuantity(InQuantity);
	ItemReference->NumericData.ItemWeight = ItemToDrop->GetItemSingleWeight();
	PickupStaticMesh->SetStaticMesh(ItemToDrop->AssetData.ItemMesh);

	UpdateInteractableData();
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
			// 		UpdateInteractableData();
			// 		PickupTaker->UpdateInteractionWidget();
			// 		break;
			// 	case EItemAddResult::IAR_AllItemAdded:
			// 		Destroy();
			// 		break;
			// 	default: ;
			// 	}
			// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			// }
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

void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.GetPropertyName() : NAME_None;
	
	if(ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredID))
	{
		if(ItemDataTable)
		{
			if(const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredID, DesiredID.ToString()))
			{
				PickupStaticMesh->SetStaticMesh(ItemData->AssetData.ItemMesh);
			}
			
		}
	}
}






