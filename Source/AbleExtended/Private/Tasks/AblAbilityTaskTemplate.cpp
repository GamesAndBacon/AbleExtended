// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/AblAbilityTaskTemplate.h"
#include "ablAbility.h"
#include "AbleCore/Private/AbleCorePrivate.h"
#include "ablAbilityComponent.h"
/* Create our scratchpad */
UAblAbilityTaskTemplateScratchPad::UAblAbilityTaskTemplateScratchPad()
{
}
UAblAbilityTaskTemplateScratchPad::~UAblAbilityTaskTemplateScratchPad()
{
}

/* Construct and deconstruct task and set default parameters */
UAblAbilityTaskTemplate::UAblAbilityTaskTemplate(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
m_TaskRealm(ATR_ClientAndServer)
{
}
UAblAbilityTaskTemplate::~UAblAbilityTaskTemplate()
{
}

/* MAIN FUNCTIONS */
void UAblAbilityTaskTemplate::OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	/*	example of retrieving a dynamic variable,please ensure you have a Delegate UPROPERTY set, and they are bound in the BindDynamicDelegates function
	ABL_GET_DYNAMIC_PROPERTY_VALUE(Context,m_Vector));	*/
}

void UAblAbilityTaskTemplate::OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context,
	const EAblAbilityTaskResult Result) const
{
	Super::OnTaskEnd(Context, Result);
}

bool UAblAbilityTaskTemplate::IsSingleFrame() const
{
	return Super::IsSingleFrame();
}


/* Create our Scratchpad */
UAblAbilityTaskScratchPad* UAblAbilityTaskTemplate::CreateScratchPad(const TWeakObjectPtr<UAblAbilityContext>& Context) const
{
	return NewObject<UAblAbilityTaskTemplateScratchPad>(Context.Get());
}

/* declare an ID, needs a unique ID, usually the name of your task,with the default STATGROUP_Able */
TStatId UAblAbilityTaskTemplate::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAblAbilityTaskTemplate, STATGROUP_Able);
}

/* function that allows us to set parameters and dynamic.  */
void UAblAbilityTaskTemplate::BindDynamicDelegates(UAblAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
	/* Example property binding TEXT, must match the Displayname of the UProperty */
	/* ABL_BIND_DYNAMIC_PROPERTY(Ability, m_Vector, TEXT("Vector"));	*/
}

