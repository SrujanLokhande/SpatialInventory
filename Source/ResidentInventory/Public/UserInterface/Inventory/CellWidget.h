// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "CellWidget.generated.h"

class UGridWidget;
class UDragDropOperation;

UCLASS()
class RESIDENTINVENTORY_API UCellWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//=============================================================================
	// PROPERTIES
	//=============================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell")
	FLinearColor DefaultColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell")
	FLinearColor HoveredColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell")
	FLinearColor ClickedColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell")
	FLinearColor ValidPlacementColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell")
	FLinearColor InvalidPlacementColor;

	UPROPERTY(BlueprintReadOnly, Category = "Cell")
	FPoint2D Coordinates;

	UPROPERTY(BlueprintReadOnly, Category = "Cell")
	float CellSize;

	UPROPERTY(BlueprintReadOnly, Category = "Cell")
	UGridWidget* ParentWidget;

	//=============================================================================
	// FUNCTIONS
	//=============================================================================
	
	void SetCellData(const FPoint2D& InCoordinates, float InSize, UGridWidget* InParentWidget);

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Cell")
	void OnCellDataReceived();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cell")
	void SetCellColor(const FLinearColor& Color);

private:
	
	FLinearColor LastStateColor;
	bool bMouseWasDragging;

};
