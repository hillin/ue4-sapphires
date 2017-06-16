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
	bool GetVisibilitySafe(USceneComponent* Component);

	/** Write component's hierarchical visibility  */
	void SetComponentVisibility(USceneComponent* Component, bool bVisible);

	/** Get plain visibility of this component from the visiblity map. If not existed in map, retrieve and store */
	bool GetVisibilityNonRecursive(USceneComponent* Component);


public:

	/** Set visiblity for the specified component and its subtree */
	UFUNCTION(BlueprintCallable, Category = Visibility)
		void SetVisibility(USceneComponent* Component, bool bVisible);

	/** Set visiblity for the specified actor and its subtree */
	UFUNCTION(BlueprintCallable, Category = Visibility)
		void SetActorVisibility(AActor* Actor, bool bVisible);

	/** Get if this component is visible in its hierarchy */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Visibility)
		bool GetVisibility(USceneComponent* Component);

	/** Get if this actor is visible in its hierarchy */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Visibility)
		bool GetActorVisibility(AActor* Actor);


	
};
