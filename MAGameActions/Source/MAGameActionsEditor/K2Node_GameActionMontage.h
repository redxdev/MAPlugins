// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "K2Node_BaseAsyncTask.h"
#include "K2Node_GameActionMontage.generated.h"

UCLASS()
class MAGAMEACTIONSEDITOR_API UK2Node_GameActionMontage : public UK2Node_BaseAsyncTask
{
	GENERATED_BODY()

public:
	UK2Node_GameActionMontage();

	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;

	virtual FText GetMenuCategory() const override;
	virtual bool IsActionFilteredOut(FBlueprintActionFilter const& Filter) override;
};
