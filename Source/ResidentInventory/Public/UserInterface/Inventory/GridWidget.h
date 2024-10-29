// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridWidget.generated.h"

class UCellWidget;
class USlotWidget;
class UInventoryComponent;
class UItemBase;
/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API UGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGridWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Grid|Widgets")
	TSubclassOf<UCellWidget> CellWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grid|Widgets")
	TSubclassOf<USlotWidget> SlotWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	UInventoryComponent* Inventory;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetInventoryComponent(UInventoryComponent* InInventory);

	UFUNCTION(BlueprintPure, Category = "Grid")
	int32 GetCellIndex(const FPoint2D& Coordinates) const;

	UFUNCTION(BlueprintPure, Category = "Grid")
	const TArray<UCellWidget*>& GetCellWidgets() const { return CellWidgets; }

	UFUNCTION(BlueprintPure, Category = "Grid")
	const TArray<USlotWidget*>& GetSlotWidgets() const { return SlotWidgets; }

	UFUNCTION(BlueprintPure, Category = "Grid")
	UInventoryComponent* GetInventory() const { return Inventory; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	TArray<UCellWidget*> CellWidgets;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	TArray<USlotWidget*> SlotWidgets;

	UFUNCTION()
	void OnInventoryUpdated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnCellWidgetCreated(UCellWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnSlotWidgetCreated(USlotWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnInventorySet();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void UpdateSlotPositions();
};
