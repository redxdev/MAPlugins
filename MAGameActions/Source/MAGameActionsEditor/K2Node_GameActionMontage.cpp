// Copyright (c) MissiveArts LLC

#include "K2Node_GameActionMontage.h"

#include "BlueprintActionFilter.h"
#include "MAGameActions/GameActionMontageCallbackProxy.h"
#include "MAGameActions/GameAction.h"

#define LOCTEXT_NAMESPACE "MAGameActionsEditor_Graph"

UK2Node_GameActionMontage::UK2Node_GameActionMontage()
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UGameActionMontageCallbackProxy, CreateProxyObjectForPlayMontage);
	ProxyFactoryClass = UGameActionMontageCallbackProxy::StaticClass();
	ProxyClass = UGameActionMontageCallbackProxy::StaticClass();
}

FText UK2Node_GameActionMontage::GetTooltipText() const
{
	return LOCTEXT("K2Node_GameActionMontage_Tooltip", "Plays a Montage on a SkeletalMeshComponent, for use in Game Actions");
}

FText UK2Node_GameActionMontage::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PlayMontage", "Game Action: Play Montage");
}

void UK2Node_GameActionMontage::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		FText ToolTipText = LOCTEXT("K2Node_GameActionMontage_OnNotifyBegin_Tooltip", "Event called when using a PlayMontageNotify or PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		FText ToolTipText = LOCTEXT("K2Node_GameActionMontage_OnNotifyEnd_Tooltip", "Event called when using a PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}

FText UK2Node_GameActionMontage::GetMenuCategory() const
{
	return LOCTEXT("GameActionMontageCategory", "Game Actions");
}

bool UK2Node_GameActionMontage::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	// Only display this node for game action blueprints.
	for (UBlueprint* Blueprint : Filter.Context.Blueprints)
	{
		if (Blueprint && Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(UGameAction::StaticClass()))
		{
			return false;
		}
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
