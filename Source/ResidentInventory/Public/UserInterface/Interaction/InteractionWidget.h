// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UProgressBar;
struct FInteractableData;
class UTextBlock;
class AResidentInventoryCharacter;
/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// The Owning player ref
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Player Reference")
	AResidentInventoryCharacter* PlayerCharacterRef;

	void UpdateWidget(const FInteractableData* InteractableData) const;

protected:

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* TXT_Name;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* TXT_Action;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* TXT_Quantity;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* TXT_KeyPress;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UProgressBar* BAR_InteractionProgressBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	float CurrentInteractionDuration;

	UFUNCTION(Category = "Interaction Widget | Interactable Data")
	static float UpdateInteractionProgress();
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
