// Copyright Srujan Lokhande @2024

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResidentInventory/DataStructure/Point2D.h"
#include "InventoryComponent.generated.h"

class APickup;
struct FPoint2D;
class UItemDataStruct;
class UInventoryComponent;
class UItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemEvent, UItemDataStruct*, ItemDataStruct, int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryEquipmentEvent, UItemDataStruct*, ItemDataStruct, int32, Quantity);

USTRUCT(BlueprintType)
struct RESIDENTINVENTORY_API FSlot
{
	GENERATED_BODY()

	FSlot()
	{
		OwnerInventory = nullptr;
		ItemBase = nullptr;
		Quantity = 0;
	}

	FSlot(UItemBase* InItemBase, const int32 InQuantity, UInventoryComponent* InOwnerInventory)
	{
		OwnerInventory = InOwnerInventory;
		ItemBase = InItemBase;
		Quantity = InQuantity;
	}

	UPROPERTY(BlueprintReadOnly)
	UItemBase* ItemBase;

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity;

	UPROPERTY(BlueprintReadOnly)
	UInventoryComponent* OwnerInventory;
	

	bool operator == (const FSlot& Other) const
	{
		// instead of this make use of the Item ID
		return Other.ItemBase == ItemBase && Other.Quantity == Quantity;
	}

	bool operator != (const FSlot& Other) const
	{
		return Other.ItemBase != ItemBase && Other.Quantity != Quantity;
	}
	
	bool IsOnMaxStackSize() const;
	int32 GetMissingStackQuantity() const;

	void SetQuantity(int32 InQuantity);	
	void UpdateQuantity(int32 InQuantity);

	bool IsEmpty() const;
	bool IsOccupied() const;
	bool IsValid() const;
	
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	None								UMETA(DisplayName = "None"),
	PrimaryWeapon						UMETA(DisplayName = "PrimaryWeapon"),
	SecondaryWeapon						UMETA(DisplayName = "SecondaryWeapon"),

};

/**
 * Startup Item
 */

// no need of this because we are not using startup items
USTRUCT(BlueprintType)
struct RESIDENTINVENTORY_API FEquipmentSlot
{
	GENERATED_BODY()

	FEquipmentSlot()
	{
		Type = EEquipmentSlotType::None;
		Data = FSlot();
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlotType Type;
	
	UPROPERTY(BlueprintReadWrite)
	FSlot Data;
	
	bool IsValid() const
	{
		return Type != EEquipmentSlotType::None;
	}
};

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RESIDENTINVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	//=============================================================================
	// PROPERTIES
	//=============================================================================
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FPoint2D GridSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1.0f, UIMin = 1.0f), Category = "Inventory")
	float CellSize;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FPoint2D> Cells;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FSlot> Slots;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	float CurrentWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1.0f, UIMin = 1.0f, EditCondition = "!bUseScaledMaxWeight"), Category = "Inventory")
	float MaxWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TArray<FEquipmentSlot> EquipmentSlots;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnInventoryUpdated;
	
	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnWeightChanged;
	
	//=============================================================================
	// FUNCTIONS
	//=============================================================================
	
	UInventoryComponent();		

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemBase* CreateItemInstance(TSubclassOf<UItemBase> InItemBaseClass) const;	

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsWithinBoundaries(const FPoint2D& Coordinates) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsFreeCell(const FPoint2D& Coordinates);
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool DoesItemFit(const TArray<FPoint2D>& SizeInCells, const FPoint2D& Coordinates);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FPoint2D GetFreeCell();
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FPoint2D GetFreeCellWhereItemCanFit(const TArray<FPoint2D>& SizeInCells);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsFull() const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool DoesItemExist(const UItemDataStruct* InItemDataStruct);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 CountItemQuantity(const UItemDataStruct* InItemDataStruct);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FSlot GetSlotByCoordinates(const FPoint2D& Coordinates);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSlotIndexByCoordinates(const FPoint2D& Coordinates);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanCarryItem(const UItemDataStruct* InItemDataStruct, const int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Initialize();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddNewItem(UItemDataStruct* InItemDataStruct, int32 Quantity, int32& AddedQuantity);
	
	UFUNCTION()
	bool AddItemExisting(UItemBase* InItemBase, int32 Quantity, int32 AddedQuantity);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemDataStruct* InItemDataStruct, int32 Quantity, int32& RemovedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemOnSlot(const FSlot& Slot, int32 Quantity, int32& RemovedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItemOnSlot(const FSlot& Slot, const FPoint2D& Destination);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void StackItemStackOnSlot(const FSlot& Slot, const FPoint2D& Destination, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItemOnSlot(const FSlot& Slot);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnequipItem(EEquipmentSlotType EquipmentSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItemOnSlot(const FSlot& Slot);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HandleAddItem(APickup* Pickup, int32& LootedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SpawnItem(const UItemDataStruct* InItemDataStruct, int32 Quantity, const FTransform& Transform);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsValidEquipmentSlots();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FEquipmentSlot GetEquipmentSlotByType(EEquipmentSlotType SlotType);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetEquipmentSlotIndexByType(EEquipmentSlotType SlotType);

	void NotifyInventoryInitialized();
	void NotifyInventoryUpdated();
	void NotifyInventoryInsufficientSpace();
	void NotifyInventoryWeightChanged();
	void NotifyInventoryItemAdded(UItemDataStruct* InItemDataStruct, int32 InQuantity);
	void NotifyInventoryItemRemoved(UItemDataStruct* InItemDataStruct, int32 InQuantity);
	void NotifyInventoryItemEquipped(UItemDataStruct* InItemDataStruct, int32 InQuantity);
	void NotifyInventoryItemUnequipped(UItemDataStruct* InItemDataStruct, int32 InQuantity);
	void NotifyInventoryItemUsed(UItemDataStruct* InItemDataStruct, int32 InQuantity);

	bool AddExistingItem_Internal(const UItemBase* InItemBase, int32 Quantity, int32& AddedQuantity);	

protected:
	//=============================================================================
	// PROPERTIES
	//=============================================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1, UIMin = 1), Category = "Inventory")
	float PickupSpawnRadiusFromPlayer;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnInventoryInitialized;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnInsufficientSpace;

	UPROPERTY(BlueprintAssignable)	
	FInventoryItemEvent OnItemAdded;

	UPROPERTY(BlueprintAssignable)	
	FInventoryItemEvent OnItemRemoved;

	UPROPERTY(BlueprintAssignable)
	FInventoryEquipmentEvent OnItemEquipped;

	UPROPERTY(BlueprintAssignable)
	FInventoryEquipmentEvent OnItemUnequipped;

	UPROPERTY(BlueprintAssignable)
	FInventoryItemEvent OnItemUsed;

	//=============================================================================
	// FUNCTIONS
	//=============================================================================
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Initialized"), Category = "Inventory")
	void K2_OnInventoryInitialized();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Money Changed"), Category = "Inventory")
	void K2_OnMoneyChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Updated"), Category = "Inventory")
	void K2_OnInventoryUpdated();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Insufficient Space"), Category = "Inventory")
	void K2_OnInventoryInsufficientSpace();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Weight Changed"), Category = "Inventory")
	void K2_OnInventoryWeightChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Added"), Category = "Inventory")
	void K2_OnInventoryItemAdded(UItemDataStruct* InItemDataStruct, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Removed"), Category = "Inventory")
	void K2_OnInventoryItemRemoved(UItemDataStruct* InItemDataStruct, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Equipped"), Category = "Inventory")
	void K2_OnInventoryItemEquipped(UItemDataStruct* InItemDataStruct, int32 Quantity);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Unequipped"), Category = "Inventory")
	void K2_OnInventoryItemUnequipped(UItemDataStruct* InItemDataStruct, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Used"), Category = "Inventory")
	void K2_OnInventoryItemUsed(UItemDataStruct* InItemDataStruct, int32 Quantity);
	
	
};

	
