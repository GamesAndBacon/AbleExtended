// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAblAbilityTask.h"
#include "ablAbilityTypes.h"
#include "AblAbilityTaskTemplate.generated.h"

#define LOCTEXT_NAMESPACE "AblAbilityTask"

/*	find and replace "UAblAbilityTaskTemplate" with your task name	*/

/* WE CREATE A SCRATCHPAD AT RUNTIME */
UCLASS(Transient)
class UAblAbilityTaskTemplateScratchPad : public UAblAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	UAblAbilityTaskTemplateScratchPad();
	virtual ~UAblAbilityTaskTemplateScratchPad();
	/* example of Skeletal Mesh Component saved into our scratchpad to use globally across the task
	 UPROPERTY(transient)
	 USkeletalMeshComponent* Mesh;	*/
};

UCLASS()
class ABLEEXTENDED_API UAblAbilityTaskTemplate : public UAblAbilityTask
{
	GENERATED_BODY()
public:
	/* Constructor and Destructor */
	UAblAbilityTaskTemplate(const FObjectInitializer& ObjectInitializer);
	virtual ~UAblAbilityTaskTemplate();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const override;
	
	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context, const EAblAbilityTaskResult Result) const override;
	
	/* Returns true if our Task only lasts a single frame. */
	virtual bool IsSingleFrame() const override; //set directly in header with { return true/false; } or create a function to decide.
	
	/* Returns the realm this Task belongs to. by default theres is an exposed enum */
	virtual EAblAbilityTaskRealm GetTaskRealm() const override {return m_TaskRealm;}// set directly in header with { return EAblAbilityTaskRealm::ATR_ClientAndServer; }

	/* Creates the Scratchpad for our Task. */
	virtual UAblAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAblAbilityContext>& Context) const;

	/* Returns the Profiler Stat ID for our Task. */
	virtual TStatId GetStatId() const override;

	/* Setup Dynamic Binding. */
	virtual void BindDynamicDelegates( UAblAbility* Ability ) override;

	
	/* EDITOR FUNCTIONS	*/
#if WITH_EDITOR
	/* Returns the category for our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("TEMPLATE", "TEMPLATE"); }
	
	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("TEMPLATE", "CPP TEMPLATE"); }
#endif


	/*	VARIABLE EXAMPLES	*/
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "TaskRealm"))
	TEnumAsByte<EAblAbilityTaskRealm> m_TaskRealm;
	/*
	 *remember to add AblBindableProperty into the meta data if you want to have it being bindable 
	UPROPERTY(EditAnywhere, Category = "Vector", meta = (DisplayName = "Vector", AblBindableProperty))
	FVector m_Vector;
	// Delegate for dynamic property, same property name with Delegate added to the end //
	UPROPERTY()
	FGetAblVector m_VectorDelegate;	*/
};
