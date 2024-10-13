// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UInventoryPanel;
class AResidentInventoryCharacter;
/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AResidentInventoryCharacter* PlayerCharacter;

	//FORCEINLINE UInventoryPanel* GetInventoryWidgetClass() const { return WBP_InventoryWidget; }

protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// when we do drag and drop for the inventory item
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	// UPROPERTY(meta=(BindWidget))
	// UInventoryPanel* WBP_InventoryWidget;
};
