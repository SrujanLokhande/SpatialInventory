// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Pickup.generated.h"


struct FInteractableData;
struct FItemData;
class UItemBase;
class UStaticMeshComponent;
class UDataTable;


UCLASS()
class RESIDENTINVENTORY_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickup();

	// take the items from the data table during begin play
	void InitializePickup(const TSubclassOf<UItemBase> BaseClass, int32 ItemQuantity);

	// to initialize the items from the data table to be ready for dropping
	void InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity);	

	// getter for the Item
	FORCEINLINE UItemBase* GetItemData() const { return ItemReference;}

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupStaticMesh;	

	// The data table with all the info we give to a specific actor
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Database")
	UDataTable* ItemDataTable;

	// the ID of the item which can be referenced with the ItemID in the data table to get a specific item
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Database")
	FName DesiredID;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Reference")
	UItemBase* ItemReference;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	int32 PickupItemQuantity;

	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;
	
	virtual void BeginPlay() override;

	virtual void Interact(AResidentInventoryCharacter* PlayerCharacter) override;
	void UpdateInteractableData();
	void TakePickup(const AResidentInventoryCharacter* PickupTaker);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;	
#endif
};
