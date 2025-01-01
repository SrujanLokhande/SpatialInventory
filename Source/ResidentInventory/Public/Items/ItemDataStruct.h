
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ResidentInventory/DataStructure/Point2D.h"
#include "ItemDataStruct.generated.h"

enum class EEquipmentSlotType : uint8;
class UItemBase;
class APickup;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RESIDENTINVENTORY_API UItemDataStruct : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//==========================================================
	// PROPERTIES
	//==========================================================
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FPrimaryAssetType AssetType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FSlateBrush Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	TSubclassOf<UItemBase> ItemBaseClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bCanBeDropped : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bCanBeDropped"), Category = "Item")
	TSubclassOf<APickup> PickupClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bCanBeStacked : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = 2, UIMin = 2), Category = "Item")
	int32 MaxStackSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bCanBeEquipped && !bCanBeConsumed"), Category = "Item")
	EEquipmentSlotType PrimaryEquipmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FPoint2D Size;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = 0.0f, UIMin = 0.0f, EditCondition = "!bUseScaledWeight"), Category = "Item")
	float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bCanBeRotated : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "!bCanBeConsumed"), Category = "Item")
	uint8 bCanBeEquipped : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	UStaticMesh* PickupStaticMesh;

	//==========================================================
	// FUNCTIONS
	//==========================================================	
	
	// Constructor
	UItemDataStruct();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintPure, Category = "Item")
	FString GetIdentifierString() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	FString GetAssetName() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	const FPrimaryAssetType& GetAssetType() const
	{
		return AssetType;
	}
	
	UFUNCTION(BlueprintPure, Category = "Item")
	TArray<FPoint2D> GetSizeInCells() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	virtual bool CanBeRotated() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	float GetScaledWeight() const;
};
