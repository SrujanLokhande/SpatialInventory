// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResidentInventory/DataStructure/Point2D.h"
#include "InventoryPanel.generated.h"

/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UInventoryPanel(const FObjectInitializer& ObjectInitializer);

	void NativeOnInventoryDataReceived();

	int32 GetCellIndex(const FPoint2D& Coordinates);
	
	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetInventoryData(UInventoryComponent* NewInventory);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnPrePopulateData();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnInventoryDataReceived();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnCellWidgetCreated(UCellWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnSlotWidgetCreated(USlotWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnSlotWidgetRemoved(USlotWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void SetWeight(float CurrentWeight, float MaxWeight);

	UFUNCTION()
	void OnInventoryUpdated();
	
	UFUNCTION()
	void OnInventoryWeightChanged();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
	TSubclassOf<UCellWidget> CellWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
	TSubclassOf<USlotWidget> SlotWidgetClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Grid")
	TArray<USlotWidget*> SlotsWidgets;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Grid")
	TArray<UCellWidget*> CellsWidgets;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Grid")
	UInventoryComponent* Inventory;

	
};
