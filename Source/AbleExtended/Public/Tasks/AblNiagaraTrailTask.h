// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAblAbilityTask.h"
#include "ablAbilityTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "AblNiagaraTrailTask.generated.h"

#define LOCTEXT_NAMESPACE "AblAbilityTask"

/*	find and replace "UAblNiagaraTrailTask" with your task name	*/

/* WE CREATE A SCRATCHPAD AT RUNTIME */
UCLASS(Transient)
class UAblNiagaraTrailTaskScratchPad : public UAblAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	UAblNiagaraTrailTaskScratchPad();
	virtual ~UAblNiagaraTrailTaskScratchPad();
	UPROPERTY(transient)
	UNiagaraComponent* component;
	USkeletalMeshComponent* skeleton;
	
};

UCLASS()
class ABLEEXTENDED_API UAblNiagaraTrailTask : public UAblAbilityTask
{
	GENERATED_BODY()
public:
	/* Constructor and Destructor */
	UAblNiagaraTrailTask(const FObjectInitializer& ObjectInitializer);
	virtual ~UAblNiagaraTrailTask();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const override;
	
	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context, const EAblAbilityTaskResult Result) const override;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAblAbilityContext>& Context, float deltaTime) const override;
	
	
	/* Returns true if our Task only lasts a single frame. */
	virtual bool IsSingleFrame() const override { return false; }
	
	/* Returns the realm this Task belongs to. */
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
	virtual FText GetTaskCategory() const override { return LOCTEXT("AblNiagaraTrailcatagory", "Effects"); }
	
	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("AblNiagaraTrailTask", "Niagara Ribbon Trail "); }
#endif


	/*	VARIABLE EXAMPLES	*/
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "TaskRealm"))
	TEnumAsByte<EAblAbilityTaskRealm> m_TaskRealm;
	
	
	UPROPERTY(EditAnywhere, Category = "Location", meta = (DisplayName = "StartVector", AblBindableProperty))
	FVector m_StartVector;
	// Delegate for dynamic property, same property name with Delegate added to the end //
	UPROPERTY()
	FGetAblVector m_StartVectorDelegate;
	
	UPROPERTY(EditAnywhere, Category = "Location", meta = (DisplayName = "EndVector", AblBindableProperty))
	FVector m_EndVector;
	// Delegate for dynamic property, same property name with Delegate added to the end //
	UPROPERTY()
	FGetAblVector m_EndVectorDelegate;

	UPROPERTY(EditAnywhere, Category = "System", meta = (DisplayName = "System", AblBindableProperty))
	UNiagaraSystem* m_System;
	// Delegate for dynamic property, same property name with Delegate added to the end //
	UPROPERTY()
	FGetAblNiagaraSystem m_SystemDelegate;
	
	UPROPERTY(EditAnywhere, Category = "System", meta = (DisplayName = "UseCharacterMeshSockets"))
	bool m_UseCharacterMesh;

	UPROPERTY(EditAnywhere, Category = "Sockets", meta = (DisplayName = "StartSocket"))
	FName m_StartSocket;

	UPROPERTY(EditAnywhere, Category = "Sockets", meta = (DisplayName = "EndSocket"))
	FName m_EndSocket;

	
};
