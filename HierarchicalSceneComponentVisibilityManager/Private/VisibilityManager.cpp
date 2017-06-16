#include "YourProject.h"
#include "VisibilityManager.h"

bool UVisibilityManager::GetVisibilitySafe(USceneComponent* Component)
{
	if (!Component) {
		return true;
	}

	return this->GetVisibility(Component);
}

void UVisibilityManager::SetComponentVisibility(USceneComponent* Component, bool bVisible)
{
	Component->SetVisibility(bVisible && this->GetVisibilitySafe(Component->GetAttachParent()), false);
}

bool UVisibilityManager::GetVisibilityNonRecursive(USceneComponent * Component)
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

void UVisibilityManager::SetVisibility(USceneComponent * Component, bool bVisible)
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

void UVisibilityManager::SetActorVisibility(AActor* Actor, bool bVisible)
{
	this->SetVisibility(Actor->GetRootComponent(), bVisible);
}

bool UVisibilityManager::GetVisibility(USceneComponent* Component)
{
	return this->GetVisibilityNonRecursive(Component) && this->GetVisibilitySafe(Component->GetAttachParent());
}

bool UVisibilityManager::GetActorVisibility(AActor* Actor)
{
	return this->GetVisibility(Actor->GetRootComponent());
}