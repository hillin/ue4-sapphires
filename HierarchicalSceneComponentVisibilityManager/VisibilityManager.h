#pragma once

#include "UObject/NoExportTypes.h"
#include "VisibilityManager.generated.h"

/**
 * A hierarchical scene component visibility manager
 */
UCLASS(Blueprintable)
class YOURPROJECT_API UVisibilityManager : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(transient)
	TMap<uint32, bool> VisibilityMap;

	/** Safe version of GetVisibility, if Component is nullptr, return true. */
	bool GetVisibilitySafe(USceneComponent* Component)
	{
		if (!Component) {
			return true;
		}

		return this->GetVisibility(Component);
	}

	/** Write component's hierarchical visibility  */
	void SetComponentVisibility(USceneComponent* Component, bool bVisible)
	{
		Component->SetVisibility(bVisible && this->GetVisibilitySafe(Component->GetAttachParent()), false);
	}


	/** Get plain visibility of this component from the visiblity map. If not existed in map, retrieve and store */
	bool GetVisibilityNonRecursive(USceneComponent* Component)
	{
		uint32 CompId = Component->GetUniqueID();

		bool* VisibilityPtr = this->VisibilityMap.Find(CompId);
		if (VisibilityPtr == nullptr) {
			this->VisibilityMap.Add(CompId, Component->bVisible);
			return Component->bVisible;
		}
		else {
			return *VisibilityPtr;
		}
	}

public:

	/** Set visiblity for the specified component and its subtree */
	UFUNCTION(BlueprintCallable, Category=Visibility)
	void SetVisibility(USceneComponent* Component, bool bVisible)
	{
		bool& CurrentVisibility = this->VisibilityMap.FindOrAdd(Component->GetUniqueID());
		CurrentVisibility = bVisible;

		this->SetComponentVisibility(Component, bVisible);

		const TArray<USceneComponent*>& AttachedChildren = Component->GetAttachChildren();
		if (AttachedChildren.Num() > 0)
		{
			// fully traverse down the attachment tree
			// we do it entirely inline here instead of recursing in case a primitivecomponent is a child of a non-primitivecomponent
			TInlineComponentArray<USceneComponent*, NumInlinedActorComponents> ComponentStack;

			// prime the pump
			ComponentStack.Append(AttachedChildren);

			while (ComponentStack.Num() > 0)
			{
				USceneComponent* const CurrentComp = ComponentStack.Pop(/*bAllowShrinking=*/ false);
				if (CurrentComp)
				{
					ComponentStack.Append(CurrentComp->GetAttachChildren());

					UPrimitiveComponent* const PrimComp = Cast<UPrimitiveComponent>(CurrentComp);
					if (PrimComp)
					{
						this->SetComponentVisibility(PrimComp, this->GetVisibilityNonRecursive(PrimComp));
					}
				}
			}
		}
	}

	/** Set visiblity for the specified actor and its subtree */
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void SetActorVisibility(AActor* Actor, bool bVisible)
	{
		this->SetVisibility(Actor->GetRootComponent(), bVisible);
	}

	/** Get if this component is visible in its hierarchy */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Visibility)
	bool GetVisibility(USceneComponent* Component)
	{
		return this->GetVisibilityNonRecursive(Component) && this->GetVisibilitySafe(Component->GetAttachParent());
	}

	/** Get if this actor is visible in its hierarchy */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Visibility)
	bool GetActorVisibility(AActor* Actor)
	{
		return this->GetVisibility(Actor->GetRootComponent());
	}

	
};
