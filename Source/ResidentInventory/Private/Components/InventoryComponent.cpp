// Copyright Srujan Lokhande @2024

#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"



UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Default grid size of 10x10
	GridSize = FPoint2D(10, 10);
	CellSize = 50.0f;
	MaxWeight = 100.0f;
	CurrentWeight = 0.0f;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeGrid();
}

void UInventoryComponent::InitializeGrid()
{
	// Initialize the grid cells
	Cells.Empty();
	Slots.Empty();

	// Create grid cells based on size
	for (int32 X = 0; X < GridSize.X; X++)
	{
		for (int32 Y = 0; Y < GridSize.Y; Y++)
		{
			Cells.Add(FPoint2D(X, Y));
		}
	}

	NotifyInventoryUpdated();
}

bool UInventoryComponent::IsWithinBoundaries(const FPoint2D& Coordinates) const
{
	return Coordinates.X >= 0 && Coordinates.Y >= 0 && 
		   Coordinates.X < GridSize.X && Coordinates.Y < GridSize.Y;
}

bool UInventoryComponent::IsFreeCell(const FPoint2D& Coordinates)
{
	if (!IsWithinBoundaries(Coordinates))
	{
		return false;
	}

	// Check if any item occupies this cell
	for (const FSlot& Slot : Slots)
	{
		// Get the item's occupied cells
		TArray<FPoint2D> OccupiedCells;
		const FVector2D ItemDimensions = Slot.ItemInstance->NumericData.ItemDimensions;
        
		// Convert item dimensions to occupied cells
		for (int32 X = 0; X < ItemDimensions.X; X++)
		{
			for (int32 Y = 0; Y < ItemDimensions.Y; Y++)
			{
				OccupiedCells.Add(FPoint2D(X, Y));
			}
		}

		// Check if any of the item's cells overlap with the target coordinates
		for (const FPoint2D& Cell : OccupiedCells)
		{
			if (Cell == Coordinates)
			{
				return false;
			}
		}
	}

	return true;
}

bool UInventoryComponent::DoesItemFit(const TArray<FPoint2D>& SizeInCells, const FPoint2D& Coordinates)
{
	for (const FPoint2D& Cell : SizeInCells)
	{
		if (!IsFreeCell(FPoint2D(Coordinates.X + Cell.X, Coordinates.Y + Cell.Y)))
		{
			return false;
		}
	}
	return true;
}

FPoint2D UInventoryComponent::GetFreeCell()
{
	for (const FPoint2D& Cell : Cells)
	{
		if (IsFreeCell(Cell))
		{
			return Cell;
		}
	}
	return FPoint2D(-1, -1);
}

FPoint2D UInventoryComponent::GetFreeCellWhereItemFit(UItemBase* Item)
{
	if (!Item) return FPoint2D(-1, -1);

	// Convert item dimensions to cells
	TArray<FPoint2D> ItemCells;
	const FVector2D& Dimensions = Item->NumericData.ItemDimensions;
    
	for (int32 X = 0; X < Dimensions.X; X++)
	{
		for (int32 Y = 0; Y < Dimensions.Y; Y++)
		{
			ItemCells.Add(FPoint2D(X, Y));
		}
	}

	// Check each cell
	for (const FPoint2D& Cell : Cells)
	{
		if (DoesItemFit(ItemCells, Cell))
		{
			return Cell;
		}
	}

	return FPoint2D(-1, -1);
}

bool UInventoryComponent::AddItem(UItemBase* Item, const FPoint2D& Coordinates)
{
	if (!Item) return false;

	// Check weight
	if (!CanCarryItem(Item))
	{
		return false;
	}

	// Convert item dimensions to cells
	TArray<FPoint2D> ItemCells;
	const FVector2D& Dimensions = Item->NumericData.ItemDimensions;
    
	for (int32 X = 0; X < Dimensions.X; X++)
	{
		for (int32 Y = 0; Y < Dimensions.Y; Y++)
		{
			ItemCells.Add(FPoint2D(X, Y));
		}
	}

	// Check if item fits
	if (!DoesItemFit(ItemCells, Coordinates))
	{
		return false;
	}

	// Add item to inventory
	FSlot NewSlot(Item, 1, this);
	Slots.Add(NewSlot);

	// Update weight
	CurrentWeight += Item->GetItemSingleWeight();
    
	NotifyInventoryUpdated();
	NotifyWeightChanged();

	return true;
}

bool UInventoryComponent::RemoveItem(UItemBase* Item)
{
	if (!Item) return false;

	// Find and remove item
	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i].ItemInstance == Item)
		{
			CurrentWeight -= Item->GetItemSingleWeight();
			Slots.RemoveAt(i);
            
			NotifyInventoryUpdated();
			NotifyWeightChanged();
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::MoveItem(UItemBase* Item, const FPoint2D& NewCoordinates)
{
	if (!Item) return false;

	// Remove and re-add at new position
	if (RemoveItem(Item))
	{
		return AddItem(Item, NewCoordinates);
	}

	return false;
}

bool UInventoryComponent::CanCarryItem(UItemBase* Item) const
{
	if (!Item) return false;
	return (CurrentWeight + Item->GetItemSingleWeight()) <= MaxWeight;
}

void UInventoryComponent::NotifyWeightChanged()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::NotifyInventoryUpdated()
{
	OnWeightChanged.Broadcast();
}

void UInventoryComponent::UpdateWeight()
{	
}






