// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Interaction/InteractionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interfaces/InteractionInterface.h"

void UInteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Calling the UpdateInteractionProgress Function to update the Progress Bar
	//BAR_InteractionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TXT_KeyPress->SetText(FText::FromString("Press"));
	CurrentInteractionDuration = 0.0f;
}
void UInteractionWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
	// doing a switch case on all types on Interactables
	switch(InteractableData->InteractableType)
	{
	case EInteractableType::Pickup:
		TXT_KeyPress->SetText(FText::FromString("Press"));
		BAR_InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

		if(InteractableData->Quantity < 2)
		{
			// if the quantity is only 1 then hide the quantity text
			TXT_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			// Language localization using NSLOCTEXT
			// C# type of printing text {0} means printing the Quantity
			TXT_Quantity->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "TXT_Quantity", "x: {0}"),
				InteractableData->Quantity));
			TXT_Quantity->SetVisibility(ESlateVisibility::Visible);
		}
		break;
		
	case EInteractableType::NonPlayerCharacter:
		break;
		
	case EInteractableType::Device:
		break;
		
	case EInteractableType::Toggle:
		break;
		
	case EInteractableType::Container:
		break;
		
	default: ;
	}
	TXT_Action->SetText(InteractableData->Action);
	TXT_Name->SetText(InteractableData->Name);
}

float UInteractionWidget::UpdateInteractionProgress()
{
	return 0.0f;
}

