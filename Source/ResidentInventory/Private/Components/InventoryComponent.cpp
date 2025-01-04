// Copyright Srujan Lokhande @2024

#include "Components/InventoryComponent.h"

#include "Components/InventoryFunctionLibrary.h"
#include "Items/ItemBase.h"
#include "Items/ItemDataStruct.h"
#include "ResidentInventory/DataStructure/Point2D.h"
#include "World/Pickup.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	GridSize = FPoint2D(1, 1);
	CellSize = 50.0f;	

	CurrentWeight = 0.0f;
	MaxWeight = 0.0f;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
}

void UInventoryComponent::Initialize()
{
	CurrentWeight = 0.0f;
	Cells.Empty();
	Slots.Empty();

	for (int32 I = 0; I < GridSize.X; I++)
	{
		for (int32 J = 0; J < GridSize.Y; J++)
		{
			Cells.Add(FPoint2D(I, J));
			
		}
	}	
	NotifyInventoryInitialized();
}

UItemBase* UInventoryComponent::CreateItemInstance(const TSubclassOf<UItemBase> InItemBaseClass) const
{
	UItemBase* ItemInstance = NewObject<UItemBase>(GetOwner(), InItemBaseClass);
	check(ItemInstance != nullptr);

	ItemInstance-> NativeOnConstruct();
	ItemInstance->OnConstruct();

	if (ItemInstance->Item == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Item data asset. Please select an item data asset in %s"), *GetNameSafe(ItemInstance));
	}

	if (ItemInstance->Item && ItemInstance->Item->ItemBaseClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ItemInstance class. Please select an ItemInstance class in %s"), *GetNameSafe(ItemInstance->Item));
	}

	return ItemInstance;
}

bool UInventoryComponent::AddNewItem(UItemDataStruct* InItemDataStruct, int32 Quantity, int32& AddedQuantity)
{
	AddedQuantity = 0;

	if (InItemDataStruct == nullptr)
	{
		return false;
	}

	if (Quantity <= 0)
	{
		return false;
	}

	if (IsFull())
	{
		NotifyInventoryInsufficientSpace();
		return false;
	}
	
	int32 RemainingQuantity = Quantity;

	if (InItemDataStruct->bCanBeStacked)
	{
		if (DoesItemExist(InItemDataStruct))
		{
			for (FSlot& Slot: Slots)
			{
				const bool bCanStack = !Slot.IsOnMaxStackSize() && Slot.ItemBase->Item == InItemDataStruct;
				if (bCanStack)
				{
					const int32 MissingStackQuantity = Slot.GetMissingStackQuantity();
				
					if (RemainingQuantity <= MissingStackQuantity)
					{
						if (!CanCarryItem(InItemDataStruct, MissingStackQuantity))
						{
							NotifyInventoryUpdated();
							NotifyInventoryInsufficientSpace();
							return false;
						}
					
						Slot.UpdateQuantity(MissingStackQuantity);
						AddedQuantity += MissingStackQuantity;
						RemainingQuantity -= MissingStackQuantity;
					
						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(InItemDataStruct, AddedQuantity);
						return true;
					}

					if (!CanCarryItem(InItemDataStruct, MissingStackQuantity))
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				
					Slot.UpdateQuantity(MissingStackQuantity);
					AddedQuantity += MissingStackQuantity;
					RemainingQuantity -= MissingStackQuantity;
				}
			}
		}

		while (RemainingQuantity >= InItemDataStruct->MaxStackSize)
		{
			UItemBase* NewItemInstance = CreateItemInstance(InItemDataStruct->ItemBaseClass);
			check(NewItemInstance != nullptr);

			FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(InItemDataStruct, InItemDataStruct->MaxStackSize))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
				
				FSlot NewSlot = FSlot(NewItemInstance, InItemDataStruct->MaxStackSize, this);
				Slots.Add(NewSlot);

				AddedQuantity += InItemDataStruct->MaxStackSize;
				RemainingQuantity -= InItemDataStruct->MaxStackSize;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (InItemDataStruct->CanBeRotated())
				{
					NewItemInstance->Rotate();
					FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);

					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(InItemDataStruct, InItemDataStruct->MaxStackSize))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;

						FSlot NewSlot = FSlot(NewItemInstance, InItemDataStruct->MaxStackSize, this);
						Slots.Add(NewSlot);

						AddedQuantity += InItemDataStruct->MaxStackSize;
						RemainingQuantity -= InItemDataStruct->MaxStackSize;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
		
		if (RemainingQuantity > 0)
		{
			UItemBase* NewItemInstance = CreateItemInstance(InItemDataStruct->ItemBaseClass);
			check(NewItemInstance != nullptr);

			const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(InItemDataStruct, RemainingQuantity))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;

				const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
				Slots.Add(NewSlot);

				AddedQuantity += RemainingQuantity;

				NotifyInventoryUpdated();
				NotifyInventoryWeightChanged();
				NotifyInventoryItemAdded(InItemDataStruct, RemainingQuantity);

				return true;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (InItemDataStruct->CanBeRotated())
				{
					NewItemInstance->Rotate();
					const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);

					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(InItemDataStruct, RemainingQuantity))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;

						const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
						Slots.Add(NewSlot);

						AddedQuantity += RemainingQuantity;

						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(InItemDataStruct, RemainingQuantity);

						return true;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemAdded(InItemDataStruct, AddedQuantity);

		return true;
	}

	while (RemainingQuantity > 0)
	{
		UItemBase* NewItemInstance = CreateItemInstance(InItemDataStruct->ItemBaseClass);
		check(NewItemInstance != nullptr);

		const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
		if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(InItemDataStruct, 1))
		{
			NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;

			const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
			Slots.Add(NewSlot);

			AddedQuantity += 1;
			RemainingQuantity -= 1;
		}
		else
		{
			// try to rotate the item and see if it fits
			if (InItemDataStruct->CanBeRotated())
			{
				NewItemInstance->Rotate();
				const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);

				if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(InItemDataStruct, 1))
				{
					NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;

					const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
					Slots.Add(NewSlot);

					AddedQuantity += 1;
					RemainingQuantity -= 1;					
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
			else
			{
				NotifyInventoryUpdated();
				NotifyInventoryInsufficientSpace();
				return false;
			}
		}
	}

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemAdded(InItemDataStruct, AddedQuantity);

	return true;
}

bool UInventoryComponent::AddItemExisting(UItemBase* InItemBase, int32 Quantity, int32 AddedQuantity)
{
	AddedQuantity = 0;
	UItemDataStruct* Item = InItemBase->Item;
	
	if (IsFull())
	{
		NotifyInventoryInsufficientSpace();
		return false;
	}
	
	if (Item == nullptr)
	{
		return false;
	}
	
	int32 RemainingQuantity = Quantity;
	
	if (Item->bCanBeStacked)
	{
		if (DoesItemExist(Item))
		{
			for (FSlot& Slot: Slots)
			{
				const bool bCanStack = !Slot.IsOnMaxStackSize() && Slot.ItemBase->Item == Item;
				if (bCanStack)
				{
					const int32 MissingStackQuantity = Slot.GetMissingStackQuantity();
				
					if (RemainingQuantity <= MissingStackQuantity)
					{
						if (!CanCarryItem(Item, MissingStackQuantity))
						{
							NotifyInventoryUpdated();
							NotifyInventoryInsufficientSpace();
							return false;
						}
					
						Slot.UpdateQuantity(MissingStackQuantity);
						AddedQuantity += MissingStackQuantity;
						RemainingQuantity -= MissingStackQuantity;
					
						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(Item, AddedQuantity);
						return true;
					}
	
					if (!CanCarryItem(Item, MissingStackQuantity))
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				
					Slot.UpdateQuantity(MissingStackQuantity);
					AddedQuantity += MissingStackQuantity;
					RemainingQuantity -= MissingStackQuantity;
				}
			}
		}
	
		while (RemainingQuantity >= Item->MaxStackSize)
		{
			UItemBase* NewItemInstance = CreateItemInstance(Item->ItemBaseClass);
			check(NewItemInstance != nullptr);
	
			FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, Item->MaxStackSize))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
				
				FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += Item->MaxStackSize;
				RemainingQuantity -= Item->MaxStackSize;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, Item->MaxStackSize))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += Item->MaxStackSize;
						RemainingQuantity -= Item->MaxStackSize;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
		
		if (RemainingQuantity > 0)
		{
			UItemBase* NewItemInstance = CreateItemInstance(Item->ItemBaseClass);
			check(NewItemInstance != nullptr);
	
			const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, RemainingQuantity))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
				const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += RemainingQuantity;
	
				NotifyInventoryUpdated();
				NotifyInventoryWeightChanged();
				NotifyInventoryItemAdded(Item, RemainingQuantity);
	
				return true;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, RemainingQuantity))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += RemainingQuantity;
	
						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(Item, RemainingQuantity);
	
						return true;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
	
		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemAdded(Item, AddedQuantity);
	
		return true;
	}
	
	while (RemainingQuantity > 0)
	{
		UItemBase* NewItemInstance = CreateItemInstance(Item->ItemBaseClass);
		check(NewItemInstance != nullptr);
	
		const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
		if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, 1))
		{
			NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
			const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
			Slots.Add(NewSlot);
	
			AddedQuantity += 1;
			RemainingQuantity -= 1;
		}
		else
		{
			// try to rotate the item and see if it fits
			if (Item->CanBeRotated())
			{
				NewItemInstance->Rotate();
				const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
				if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, 1))
				{
					NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
					const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
					Slots.Add(NewSlot);
	
					AddedQuantity += 1;
					RemainingQuantity -= 1;
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
			else
			{
				NotifyInventoryUpdated();
				NotifyInventoryInsufficientSpace();
				return false;
			}
		}
	}
	
	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemAdded(Item, AddedQuantity);

	return true;
}

bool UInventoryComponent::RemoveItem(UItemDataStruct* InItemDataStruct, int32 Quantity, int32& RemovedQuantity)
{
	RemovedQuantity = 0;

	if (InItemDataStruct == nullptr)
	{
		return false;
	}

	if (Quantity <= 0)
	{
		return false;
	}

	if (!DoesItemExist(InItemDataStruct))
	{
		return false;
	}

	const int32 ExistingQuantity = CountItemQuantity(InItemDataStruct);
	int32 PendingQuantity = Quantity;

	if (Quantity >= ExistingQuantity)
	{
		for (auto It = Slots.CreateIterator(); It; ++It)
		{
			if (InItemDataStruct == It->ItemBase->Item)
			{
				RemovedQuantity += It->Quantity;
				It.RemoveCurrent();
			}
		}

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemRemoved(InItemDataStruct, RemovedQuantity);
		return true;
	}

	if (InItemDataStruct->bCanBeStacked)
	{
		for (auto It = Slots.CreateIterator(); It; ++It)
		{
			if (InItemDataStruct == It->ItemBase->Item && PendingQuantity > 0)
			{
				if (PendingQuantity >= It->Quantity)
				{
					RemovedQuantity += It->Quantity;
					PendingQuantity -= It->Quantity;
					It.RemoveCurrent();
				}
				else
				{
					It->UpdateQuantity(-PendingQuantity);
					RemovedQuantity += PendingQuantity;
					PendingQuantity = 0;
					break;
				}
			}
		}

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemRemoved(InItemDataStruct, RemovedQuantity);
		return true;
	}

	// Removing non-stackable items
	for (auto It = Slots.CreateIterator(); It; ++It)
	{
		if (InItemDataStruct == It->ItemBase->Item && PendingQuantity > 0)
		{
			RemovedQuantity += It->Quantity;
			PendingQuantity -= It->Quantity;
			It.RemoveCurrent();
		}
	}

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemRemoved(InItemDataStruct, RemovedQuantity);
	return true;
}

bool UInventoryComponent::RemoveItemOnSlot(const FSlot& Slot, int32 Quantity, int32& RemovedQuantity)
{
	RemovedQuantity = 0;

	if (Quantity <= 0)
	{
		return false;
	}

	const int32 SlotIndex = GetSlotIndexByCoordinates(Slot.ItemBase->TopLeftCoordinates);

	if (Quantity >= Slot.Quantity)
	{
		UItemDataStruct* RemovedItem = Slot.ItemBase->Item;
		
		RemovedQuantity += Slot.Quantity;
		Slots.Remove(Slot);

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemRemoved(RemovedItem, RemovedQuantity);
		return true;
	}

	RemovedQuantity += Quantity;
	Slots[SlotIndex].UpdateQuantity(-Quantity);

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemRemoved(Slot.ItemBase->Item, RemovedQuantity);
	return true;
}

bool UInventoryComponent::MoveItemOnSlot(const FSlot& Slot, const FPoint2D& Destination)
{
	if (!IsFreeCell(Destination))
	{
		Slots.Add(Slot);
		return false;
	}

	if (!DoesItemFit(Slot.ItemBase->SizeInCells, Destination))
	{
		Slots.Add(Slot);
		return false;
	}

	Slot.ItemBase->TopLeftCoordinates = Destination;
	Slots.Add(Slot);

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	return true;
}

void UInventoryComponent::StackItemStackOnSlot(const FSlot& Slot, const FPoint2D& Destination, int32 Quantity)
{
	if (IsFreeCell(Destination))
	{
		return;
	}

	if (!IsWithinBoundaries(Destination))
	{
		return;
	}

	if (Quantity <= 0)
	{
		return;
	}

	const FSlot DestinationSlot = GetSlotByCoordinates(Destination);
	const int32 DestinationIndex = Slots.Find(DestinationSlot);
	const int32 SourceIndex = Slots.Find(Slot);

	if (Slot.ItemBase->Item != DestinationSlot.ItemBase->Item || !DestinationSlot.ItemBase->Item->bCanBeStacked)
	{
		return;
	}

	if (DestinationSlot.IsOnMaxStackSize() || Quantity > Slot.Quantity)
	{
		return;
	}

	const int32 MissingStackQuantity = DestinationSlot.GetMissingStackQuantity();

	if (Slot.Quantity <= Quantity)
	{
		if (Quantity <= MissingStackQuantity)
		{
			Slots[DestinationIndex].UpdateQuantity(Slot.Quantity);
			Slots.Remove(Slot);

			NotifyInventoryWeightChanged();
			NotifyInventoryUpdated();

			return;
		}

		Slots[DestinationIndex].UpdateQuantity(MissingStackQuantity);
		Slots[SourceIndex].UpdateQuantity(-MissingStackQuantity);

		NotifyInventoryWeightChanged();
		NotifyInventoryUpdated();

		return;
	}

	if (Quantity <= MissingStackQuantity)
	{
		Slots[DestinationIndex].UpdateQuantity(Quantity);
		Slots[SourceIndex].UpdateQuantity(-Quantity);

		NotifyInventoryWeightChanged();
		NotifyInventoryUpdated();

		return;
	}

	Slots[DestinationIndex].UpdateQuantity(MissingStackQuantity);
	Slots[SourceIndex].UpdateQuantity(-MissingStackQuantity);

	NotifyInventoryWeightChanged();
	NotifyInventoryUpdated();
}

void UInventoryComponent::EquipItemOnSlot(const FSlot& Slot)
{	
	if (EquipmentSlots.Num() <= 0)
	{
		return;
	}
	
	if (!Slot.IsValid() || Slot.IsEmpty())
	{
		return;
	}

	if (!Slot.ItemBase->Item->bCanBeEquipped)
	{
		return;
	}

	if (!UInventoryFunctionLibrary::DoesItemHaveValidEquipmentSlot(Slot.ItemBase->Item))
	{
		return;
	}

	if (!IsValidEquipmentSlots())	// check if we have filled equipment slots array from editor
	{
		return;
	}
	
	const FEquipmentSlot PrimarySlot = GetEquipmentSlotByType(Slot.ItemBase->Item->PrimaryEquipmentSlot);
	
	if (PrimarySlot.Data.IsEmpty())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(PrimarySlot.Type);

		EquipmentSlots[EquipmentSlotIndex].Data = Slot;
		EquipmentSlots[EquipmentSlotIndex].Data.ItemBase->ResetRotation();
		
		NotifyInventoryItemEquipped(Slot.ItemBase->Item, Slot.Quantity);
		K2_OnInventoryItemEquipped(Slot.ItemBase->Item, Slot.Quantity);
		Slots.Remove(Slot);

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
	}
	
	else if (PrimarySlot.Data.IsOccupied())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(PrimarySlot.Type);

		int32 AddedQuantity = 0;
		const bool bIsAdded = AddExistingItem_Internal(PrimarySlot.Data.ItemBase, PrimarySlot.Data.Quantity, AddedQuantity);

		if (bIsAdded)
		{
			EquipmentSlots[EquipmentSlotIndex].Data = Slot;
			EquipmentSlots[EquipmentSlotIndex].Data.ItemBase->ResetRotation();
			
			NotifyInventoryItemEquipped(Slot.ItemBase->Item, Slot.Quantity);
			K2_OnInventoryItemEquipped(Slot.ItemBase->Item, Slot.Quantity);
			Slots.Remove(Slot);

			NotifyInventoryUpdated();
			NotifyInventoryWeightChanged();
		}
	}
}

void UInventoryComponent::UnequipItem(EEquipmentSlotType EquipmentSlot)
{
	if (EquipmentSlot == EEquipmentSlotType::None)
	{
		return;
	}

	const FEquipmentSlot TargetSlot = GetEquipmentSlotByType(EquipmentSlot);

	if (TargetSlot.Data.IsOccupied())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(EquipmentSlot);
		
		int32 AddedQuantity = 0;
		const bool bIsAdded = AddExistingItem_Internal(TargetSlot.Data.ItemBase, TargetSlot.Data.Quantity, AddedQuantity);

		if (bIsAdded && AddedQuantity == TargetSlot.Data.Quantity)
		{
			//EquipmentSlots[EquipmentSlotIndex].Data.OwnerInventory = nullptr;
			EquipmentSlots[EquipmentSlotIndex].Data.ItemBase = nullptr;
			EquipmentSlots[EquipmentSlotIndex].Data.Quantity = 0;

			NotifyInventoryItemUnequipped(TargetSlot.Data.ItemBase->Item, TargetSlot.Data.Quantity);
			K2_OnInventoryItemUnequipped(TargetSlot.Data.ItemBase->Item, TargetSlot.Data.Quantity);

			NotifyInventoryUpdated();
			NotifyInventoryWeightChanged();
		}
	}
}

bool UInventoryComponent::DropItemOnSlot(const FSlot& Slot)
{
	if (!Slot.ItemBase->Item->bCanBeDropped)
	{
		return false;
	}

	if (!Slot.ItemBase->Item->PickupClass)
	{
		return false;
	}

	const FSlot DataCopy = Slot;
	
	int32 RemovedQuantity = 0;
	const bool bIsRemoved = RemoveItemOnSlot(Slot, Slot.Quantity, RemovedQuantity);
	if (bIsRemoved)
	{
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * PickupSpawnRadiusFromPlayer;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APickup* SpawnedPickup = GetWorld()->SpawnActor<APickup>(DataCopy.ItemBase->Item->PickupClass, SpawnLocation, FRotator(), SpawnParams);
		if (SpawnedPickup)
		{			
			SpawnedPickup->SetPickupData(DataCopy.ItemBase, DataCopy.Quantity);
			return true;
		}

		return false;
	}

	return false;
}

bool UInventoryComponent::HandleAddItem(APickup* Pickup, int32& LootedQuantity)
{
	LootedQuantity = 0;
	
	if (Pickup == nullptr)
	{
		return false;
	}

	int32 AddedQuantity = 0;
	bool bLooted = AddItemExisting(Pickup->ItemReference, Pickup->PickupItemQuantity, AddedQuantity);

	LootedQuantity = AddedQuantity;
	
	if (bLooted)
	{
		if (Pickup->PickupItemQuantity - LootedQuantity <= 0)
		{
			Pickup->Destroy();
		}
		else
		{
			Pickup->PickupItemQuantity = FMath::Clamp((Pickup->PickupItemQuantity - LootedQuantity), 0, INT32_MAX);
		}

		return true;
	}
	
	return false;
}

void UInventoryComponent::SpawnItem(const UItemDataStruct* InItemDataStruct, int32 Quantity,
	const FTransform& Transform)
{
	if (InItemDataStruct == nullptr)
	{
		return;
	}

	if (Quantity <= 0)
	{
		return;
	}

	if (!Transform.IsValid())
	{
		return;
	}

	if (!InItemDataStruct->bCanBeDropped)
	{
		return;
	}

	if (!InItemDataStruct->PickupClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UItemBase* ItemInstance = CreateItemInstance(InItemDataStruct->ItemBaseClass);
	if (ItemInstance == nullptr)
	{
		return;
	}
	
	APickup* SpawnedPickup = GetWorld()->SpawnActor<APickup>(InItemDataStruct->PickupClass, Transform.GetLocation(), Transform.GetRotation().Rotator(), SpawnParams);
	if (SpawnedPickup)
	{		
		SpawnedPickup->SetPickupData(ItemInstance, Quantity);
	}
}

bool UInventoryComponent::IsValidEquipmentSlots()
{
	if (EquipmentSlots.Num() <= 0)
	{
		return false;
	}
	
	for (const FEquipmentSlot& EquipmentSlot: EquipmentSlots)
	{
		if (EquipmentSlot.Type == EEquipmentSlotType::None)
		{
			return false;
		}
	}

	int32 CountSlotType = 0;
	
	// check if equipment slots have some duplicate entries
	for (const FEquipmentSlot& PrimarySlot: EquipmentSlots)
	{
		for (const FEquipmentSlot& SecondarySlot: EquipmentSlots)
		{
			if (PrimarySlot.Type == SecondarySlot.Type)
			{
				CountSlotType++;
			}
		}

		if (CountSlotType > 1)
		{
			return false;
		}
		else
		{
			CountSlotType = 0;
		}
	}

	return true;
}

FEquipmentSlot UInventoryComponent::GetEquipmentSlotByType(EEquipmentSlotType SlotType)
{
	for (const FEquipmentSlot& Slot: EquipmentSlots)
	{
		if (Slot.Type == SlotType)
		{
			return Slot;
		}
	}

	return FEquipmentSlot();
}

int32 UInventoryComponent::GetEquipmentSlotIndexByType(EEquipmentSlotType SlotType)
{
	int32 Index = INDEX_NONE;
	for (const FEquipmentSlot& Slot: EquipmentSlots)
	{
		Index++;
		
		if (Slot.Type == SlotType)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}


bool UInventoryComponent::AddExistingItem_Internal(const UItemBase* InItemBase, int32 Quantity, int32& AddedQuantity)
{
	AddedQuantity = 0;
	const UItemDataStruct* Item = InItemBase->Item;
	
	if (IsFull())
	{
		NotifyInventoryInsufficientSpace();
		return false;
	}
	
	if (Item == nullptr)
	{
		return false;
	}
	
	int32 RemainingQuantity = Quantity;
	
	if (Item->bCanBeStacked)
	{
		if (DoesItemExist(Item))
		{
			for (FSlot& Slot: Slots)
			{
				const bool bCanStack = !Slot.IsOnMaxStackSize() && Slot.ItemBase->Item == Item;
				if (bCanStack)
				{
					const int32 MissingStackQuantity = Slot.GetMissingStackQuantity();
				
					if (RemainingQuantity <= MissingStackQuantity)
					{
						if (!CanCarryItem(Item, MissingStackQuantity))
						{
							NotifyInventoryInsufficientSpace();
							return false;
						}
					
						Slot.UpdateQuantity(MissingStackQuantity);
						AddedQuantity += MissingStackQuantity;
						RemainingQuantity -= MissingStackQuantity;
						
						return true;
					}
	
					if (!CanCarryItem(Item, MissingStackQuantity))
					{
						NotifyInventoryInsufficientSpace();
						return false;
					}
				
					Slot.UpdateQuantity(MissingStackQuantity);
					AddedQuantity += MissingStackQuantity;
					RemainingQuantity -= MissingStackQuantity;
				}
			}
		}
	
		while (RemainingQuantity >= Item->MaxStackSize)
		{
			UItemBase* NewItemInstance = CreateItemInstance(Item->ItemBaseClass);
			check(NewItemInstance != nullptr);
	
			FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, Item->MaxStackSize))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
				
				FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += Item->MaxStackSize;
				RemainingQuantity -= Item->MaxStackSize;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, Item->MaxStackSize))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += Item->MaxStackSize;
						RemainingQuantity -= Item->MaxStackSize;
					}
					else
					{
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
		
		if (RemainingQuantity > 0)
		{
			UItemBase* NewItemInstance = CreateItemInstance(Item->ItemBaseClass);
			check(NewItemInstance != nullptr);
	
			const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, RemainingQuantity))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
				const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += RemainingQuantity;
	
				return true;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, RemainingQuantity))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += RemainingQuantity;
	
						return true;
					}
					else
					{
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
	
		return true;
	}
	
	while (RemainingQuantity > 0)
	{
		UItemBase* NewItemInstance = CreateItemInstance(Item->ItemBaseClass);
		check(NewItemInstance != nullptr);
	
		const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
		if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, 1))
		{
			NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
			const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
			Slots.Add(NewSlot);
	
			AddedQuantity += 1;
			RemainingQuantity -= 1;
		}
		else
		{
			// try to rotate the item and see if it fits
			if (Item->CanBeRotated())
			{
				NewItemInstance->Rotate();
				const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
				if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, 1))
				{
					NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
					const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
					Slots.Add(NewSlot);
	
					AddedQuantity += 1;
					RemainingQuantity -= 1;
				}
				else
				{
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
			else
			{
				NotifyInventoryInsufficientSpace();
				return false;
			}
		}
	}

	return true;
}

//=============================================================================
// INFORMATIVE FUNCTIONS ABOUT THE ITEM
//=============================================================================

bool UInventoryComponent::IsWithinBoundaries(const FPoint2D& Coordinates) const
{
	return Coordinates.X >= 0 && Coordinates.Y >= 0 && Coordinates.X < GridSize.X && Coordinates.Y < GridSize.Y;	
}

bool UInventoryComponent::IsFreeCell(const FPoint2D& Coordinates)
{
	if (!IsWithinBoundaries(Coordinates))
	{
		return false;
	}

	for (const FSlot& Slot: Slots)
	{
		for (const FPoint2D& Cell: Slot.ItemBase->SizeInCells)
		{
			const bool bIsNotFree = (Cell + Slot.ItemBase->TopLeftCoordinates == Coordinates);
			if (bIsNotFree)
			{
				return false;
			}
		}
	}

	return true;
}

bool UInventoryComponent::DoesItemFit(const TArray<FPoint2D>& SizeInCells, const FPoint2D& Coordinates)
{
	for (const FPoint2D& Cell: SizeInCells)
	{
		if (!IsFreeCell(FPoint2D(Coordinates + Cell)))
		{
			return false;
		}
	}
	return true;
}

FPoint2D UInventoryComponent::GetFreeCell()
{
	for (const FPoint2D& Cell: Cells)
	{
		if (IsFreeCell(Cell))
		{
			return Cell;
		}
	}

	return FPoint2D(INDEX_NONE, INDEX_NONE);
}

FPoint2D UInventoryComponent::GetFreeCellWhereItemCanFit(const TArray<FPoint2D>& SizeInCells)
{
	for (const FPoint2D& Cell: Cells)
	{
		const bool bItemCanFit = IsFreeCell(Cell) && DoesItemFit(SizeInCells, Cell);
		if (bItemCanFit)
		{
			return Cell;
		}
	}

	return FPoint2D(INDEX_NONE, INDEX_NONE);
}

bool UInventoryComponent::IsFull() const
{
	return CurrentWeight >= MaxWeight;
}

bool UInventoryComponent::DoesItemExist(const UItemDataStruct* InItemDataStruct)
{
	for (const FSlot& Slot: Slots)
	{
		if (Slot.ItemBase->Item == InItemDataStruct)
		{
			return true;
		}
	}

	return false;
}

int32 UInventoryComponent::CountItemQuantity(const UItemDataStruct* InItemDataStruct)
{
	int32 Quantity = 0;
	
	for (const FSlot& Slot: Slots)
	{
		if (Slot.ItemBase->Item == InItemDataStruct)
		{
			Quantity += Slot.Quantity;
		}
	}

	return Quantity;
}

FSlot UInventoryComponent::GetSlotByCoordinates(const FPoint2D& Coordinates)
{
	for (const FSlot& Slot: Slots)
	{
		for (const FPoint2D& Cell: Slot.ItemBase->SizeInCells)
		{
			const bool bIsWithinThisSlot = (Slot.ItemBase->TopLeftCoordinates + Cell == Coordinates);
			if (bIsWithinThisSlot)
			{
				return Slot;
			}
		}
	}

	return FSlot();
}

int32 UInventoryComponent::GetSlotIndexByCoordinates(const FPoint2D& Coordinates)
{
	int32 Index = INDEX_NONE;
	
	for (const FSlot& Slot: Slots)
	{
		Index++;
		
		for (const FPoint2D& Cell: Slot.ItemBase->SizeInCells)
		{
			const bool bIsWithinThisSlot = (Slot.ItemBase->TopLeftCoordinates + Cell == Coordinates);
			if (bIsWithinThisSlot)
			{
				return Index;
			}
		}
	}

	return INDEX_NONE;
}

bool UInventoryComponent::CanCarryItem(const UItemDataStruct* InItemDataStruct, const int32 Quantity) const
{
	float EstimatedWeight = Quantity * InItemDataStruct->Weight;
	return (CurrentWeight + EstimatedWeight <= MaxWeight);
}

//=============================================================================
// FSLOT HELPER FUNCTIONS
//=============================================================================

bool FSlot::IsOnMaxStackSize() const
{
	if (ItemBase == nullptr)
	{
		return false;
	}

	if (ItemBase->Item->bCanBeStacked)
	{
		if (Quantity >= ItemBase->Item->MaxStackSize)
		{
			return true;
		}

		return false;
	}

	return false;
}

int32 FSlot::GetMissingStackQuantity() const
{
	if (ItemBase == nullptr)
	{
		return 0;
	}

	if (ItemBase->Item->bCanBeStacked)
	{
		return ItemBase->Item->MaxStackSize - Quantity;
	}

	return 0;
}

void FSlot::SetQuantity(int32 InQuantity)
{
	if (ItemBase == nullptr)
	{
		return;
	}
	
	if (ItemBase->Item->bCanBeStacked)
	{
		Quantity = FMath::Clamp(InQuantity, 0, ItemBase->Item->MaxStackSize);
	}
	else
	{
		Quantity = FMath::Clamp(InQuantity, 0, 1);
	}
}

void FSlot::UpdateQuantity(int32 InQuantity)
{
	if (ItemBase == nullptr)
	{
		return;
	}
	
	if (ItemBase->Item->bCanBeStacked)
	{
		Quantity = FMath::Clamp(Quantity + InQuantity, 0, ItemBase->Item->MaxStackSize);
	}
	else
	{
		Quantity = FMath::Clamp(Quantity + InQuantity, 0, 1);
	}
}

bool FSlot::IsEmpty() const
{
	return (ItemBase == nullptr && Quantity == 0 && OwnerInventory != nullptr);
}

bool FSlot::IsOccupied() const
{
	return (ItemBase != nullptr && Quantity > 0 && OwnerInventory != nullptr);
}

bool FSlot::IsValid() const
{
	return (OwnerInventory != nullptr && Quantity >= 0);
}

//=============================================================================
// DELEGATES
//=============================================================================

void UInventoryComponent::NotifyInventoryInitialized()
{
	OnInventoryInitialized.Broadcast();
	K2_OnInventoryInitialized();
}

void UInventoryComponent::NotifyInventoryUpdated()
{
	OnInventoryUpdated.Broadcast();
	K2_OnInventoryUpdated();
}

void UInventoryComponent::NotifyInventoryInsufficientSpace()
{
	OnInsufficientSpace.Broadcast();
	K2_OnInventoryInsufficientSpace();
}

void UInventoryComponent::NotifyInventoryWeightChanged()
{
	OnWeightChanged.Broadcast();
	K2_OnInventoryWeightChanged();
}

void UInventoryComponent::NotifyInventoryItemAdded(UItemDataStruct* InItemDataStruct, int32 InQuantity)
{
	OnItemAdded.Broadcast(InItemDataStruct, InQuantity);
	K2_OnInventoryItemAdded(InItemDataStruct, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemRemoved(UItemDataStruct* InItemDataStruct, int32 InQuantity)
{
	OnItemRemoved.Broadcast(InItemDataStruct, InQuantity);
	K2_OnInventoryItemRemoved(InItemDataStruct, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemEquipped(UItemDataStruct* InItemDataStruct, int32 InQuantity)
{
	OnItemEquipped.Broadcast(InItemDataStruct, InQuantity);
	K2_OnInventoryItemEquipped(InItemDataStruct, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemUnequipped(UItemDataStruct* InItemDataStruct, int32 InQuantity)
{
	OnItemUnequipped.Broadcast(InItemDataStruct, InQuantity);
	K2_OnInventoryItemUnequipped(InItemDataStruct, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemUsed(UItemDataStruct* InItemDataStruct, int32 InQuantity)
{
	OnItemUsed.Broadcast(InItemDataStruct, InQuantity);
	K2_OnInventoryItemUsed(InItemDataStruct, InQuantity);
}














