// Copyright Srujan Lokhande @2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UInventoryComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryWeightChanged);
DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;
/**
 * Point2D struct for grid coordinates
 */
USTRUCT(BlueprintType)
struct FPoint2D
{
    GENERATED_BODY()

    FPoint2D()
    {
        X = 0;
        Y = 0;
    }

    FPoint2D(const int32 InX, const int32 InY)
    {
        X = InX;
        Y = InY;
    }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
    int32 X;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
    int32 Y;

    bool operator == (const FPoint2D& Other) const
    {
        return Other.X == X && Other.Y == Y;
    }

    FPoint2D operator + (const FPoint2D& Other) const
    {
        return FPoint2D(Other.X + X, Other.Y + Y);
    }
};

/**
 * Slot struct for inventory items
 */
USTRUCT(BlueprintType)
struct FSlot
{
    GENERATED_BODY()

    FSlot()
    {
        OwnerInventory = nullptr;
        ItemInstance = nullptr;
        Quantity = 0;
    }

    FSlot(UItemBase* InItemInstance, const int32 InQuantity, UInventoryComponent* InOwnerInventory)
    {
        OwnerInventory = InOwnerInventory;
        ItemInstance = InItemInstance;
        Quantity = InQuantity;
    }

    UPROPERTY(BlueprintReadOnly)
    UItemBase* ItemInstance;

    UPROPERTY(BlueprintReadOnly)
    int32 Quantity;

    UPROPERTY(BlueprintReadOnly)
    UInventoryComponent* OwnerInventory;

    bool IsOnMaxStackSize() const;
    int32 GetMissingStackQuantity() const;
    void SetQuantity(int32 InQuantity);
    void UpdateQuantity(int32 InQuantity);
    bool IsEmpty() const;
    bool IsOccupied() const;
    bool IsValid() const;
};



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RESIDENTINVENTORY_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInventoryComponent();

    virtual void BeginPlay() override;

    // Grid System Functions
    UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
    bool IsWithinBoundaries(const FPoint2D& Coordinates) const;

    UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
    bool IsFreeCell(const FPoint2D& Coordinates);

    UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
    bool DoesItemFit(const TArray<FPoint2D>& SizeInCells, const FPoint2D& Coordinates);

    UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
    FPoint2D GetFreeCell();

    UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
    FPoint2D GetFreeCellWhereItemFit(UItemBase* Item);

    // Item Management Functions
    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    bool AddItem(UItemBase* Item, const FPoint2D& Coordinates);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    bool RemoveItem(UItemBase* Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
    bool MoveItem(UItemBase* Item, const FPoint2D& NewCoordinates);

    // Weight System
    UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
    bool CanCarryItem(UItemBase* Item) const;

    UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
    float GetCurrentWeight() const { return CurrentWeight; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
    float GetMaxWeight() const { return MaxWeight; }

    // Grid Properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Grid")
    FPoint2D GridSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Grid")
    float CellSize;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory|Grid")
    TArray<FPoint2D> Cells;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory|Grid")
    TArray<FSlot> Slots;

    // Weight Properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weight")
    float MaxWeight;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory|Weight")
    float CurrentWeight;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryUpdated OnInventoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryWeightChanged OnWeightChanged;

private:
    void InitializeGrid();
    void UpdateWeight();
    void NotifyInventoryUpdated();
    void NotifyWeightChanged();
};
