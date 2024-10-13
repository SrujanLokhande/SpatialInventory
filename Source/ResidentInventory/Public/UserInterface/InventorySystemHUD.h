// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InventorySystemHUD.generated.h"

class UInteractionWidget;
struct FInteractableData;
class UMainMenuWidget;
/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API AInventorySystemHUD : public AHUD
{
	GENERATED_BODY()

public:
	//=============================================================================
	// PROPERTIES
	//=============================================================================

	// Main Menu Class
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	// Interaction Widget Class
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	bool bIsMenuVisible;

	//=============================================================================
	// FUNCTIONS
	//=============================================================================

	// Sets default values for this actor's properties
	AInventorySystemHUD();

	void HideMenu();
	void ToggleMenu();
	void ShowInteractionWidget();
	void HideInteractionWidget();
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:
	//=============================================================================
	// PROPERTIES
	//=============================================================================

	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;
	
	//=============================================================================
	// FUNCTIONS
	//=============================================================================

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void DisplayMenu();
};
