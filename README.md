# A grab bag of project-agnostic plugins

This is a set of plugins that can be used in Unreal Engine 5. The plugins do not depend on each other unless otherwise
specified - you can use them individually.

Most plugins are currently setup for use with Unreal 5.1, earlier versions may work with minor modifications.

# MACommon

This contains common C++ utilities that I have found useful and don't really fit into plugins of their own.

## Object Referencers

### Strong Referencer

Unreal provides two templated "GC scope guards" that prevent UObjects from being garbage collected while that scope is
active. Unfortunately, one is just for a single object (`TGCObjectScopeGuard`) and one can only deal with an array of
objects (`TGCObjectsScopeGuard`, note the plural). This means you need to allocate an array yourself which isn't quite as
nice to use.

As such, I've implemented a simple alternative that largely works the same way but lets you pass any number of objects to
it. This is a fairly minor workflow improvement overall, but fits with the Weak Referencer which is a bit more important.

#### Usage

```c++
#include "MACommon/ObjectReferencer.h"

void MyFunction(UObject* MyFirstObject, UObject* MySecondObject)
{
    MA_STRONG_REF(MyFirstObject, MySecondObject);
    
    // Both objects are now protected against garbage collection for the lifetime of this function.
    // If this is async code you will want to check that the objects are still valid.
}
```

### Weak Referencer

Similar to the strong referencer above, but creates weak references from local UObject pointers. This isn't particularly
useful in "normal" code - on the main thread a local weak pointer isn't useful and in async code you will hit race
conditions - but it *is* very useful [when working with coroutines](https://github.com/landelare/ue5coro) where a single
function may pause/resume across multiple frames.

#### Usage

```c++
#include "MACommon/ObjectReferencer.h"

using namespace UE5Coro;

FAsyncCoroutine UMyFunctionLibrary::MyLatentCoroutine(UObject* MyFirstObject, UObject* MySecondObject, FLatentActionInfo LatentInfo)
{
    MA_WEAK_REF(MyFirstObject, MySecondObject);
    
    // Both objects are now weak references and will be set to null if they are garbage collected.
    // Yes, this means the local pointers will be set to null without the overhead of TWeakObjectPointer's operator->
    
    co_await Latent::Seconds(2.0f);
    
    // Always make sure to check weak pointers after await/yield. They will be set to null if the underlying object
    // was deleted.
    if (MyFirstObject && MySecondObject)
    {
        MyFirstObject->DoSomething(MySecondObject);
    }
}
```

## UE5Coro

Various utilities that can be used when [UE5Coro](https://github.com/landelare/ue5coro) is installed and enabled.

### Latent Callbacks

Macros in the style of `ON_SCOPE_EXIT` to support various situations where latent coroutines are exited.
**These macros only work with latent coroutines, not normal latent actions.**

#### MA_ON_LATENT_ABORT

Called when the latent action is cancelled. Equivalent to overriding `FPendingLatentAction::NotifyActionAborted`.

```c++
MA_ON_LATENT_ABORT
{
    UE_LOG(LogTemp, Display, TEXT("This latent action was cancelled."));
};
```

#### MA_ON_LATENT_DESTROY

Called when the callback object for the latent action has been destroyed. This is called *after* the object's destruction, so do not try
to access it. Equivalent to overriding `FPendingLatentAction::NotifyObjectDestroyed`.

```c++
MA_ON_LATENT_DESTROY
{
    UE_LOG(LogTemp, Display, TEXT("The callback target of this latent action was destroyed."));
};
```

#### MA_ON_ABNORMAL_EXIT

Called when either of the above events (action abort or object destruction) occurs.

```c++
MA_ON_LATENT_ABNORMAL_EXIT
{
    UE_LOG(LogTemp, Display, TEXT("Either the callback target of this latent action was destroyed or the action was cancelled."));
};
```

#### MA_ON_LATENT_EXIT

Called when the latent action ends for any reason (including successful completion). Equivalent to `ON_SCOPE_EXIT`, included simply for consistency
with the other macros.

```c++
MA_ON_LATENT_EXIT
{
    UE_LOG(LogTemp, Display, TEXT("The latent action has exited."));
};
```

### "Until" Macro

Simple macro wrapper around `UE5Coro::Latent::Until` to simplify the syntax.

```c++
co_await MA_LATENT_UNTIL(bSomeVariable); // checks bSomeVariable every frame until it is true.
```

# MAGameActions

A lightweight alternative to the Gameplay Ability System (GAS). Nothing but a simple action system that takes similar advantage of gameplay tags, but
without built-in multiplayer, executions, attribute sets, effects, etc. Designed for those who like the basic functions of abilities but don't want
any of the extra complexity.

Requires both MACommon and [UE5Coro](https://github.com/landelare/ue5coro) to be installed.

## Game Action Component

This component manages actions for an actor. Actors using this component should implement `IGameActionInterface`.

## Game Action

The base class for an action. Actions can be implemented in C++ or Blueprint. Every action has three sets of tags that define
whether the action is allowed to run and what other actions can run at the same time

### Owned Tags

Tags that apply to the action itself.

### Block Tags

If an action is running it will block any other actions that have these tags. Additionally, an action blocking tags will be prevented from executing
if there are already active actions with blocked tags.

### Cancel Tags

When an action is executed it will cancel any other actions that have these tags.

## Cooldowns

A cooldown is a gameplay tag applied to a game action component that expires after a set amount of time. During that time the component
counts as having that tag applied, and as such can block actions from executing. Note that applying a cooldown tag doesn't do anything to
already-running actions, even if they are blocked by that tag.

`CancelTags` on an action cannot affect cooldown tags.

## Game Action Handles

When executing an action, a handle to that action is returned instead of an instance of the action itself. The action can be retrieved via the handle.

This is a remnant of an older design for actions, but it is still recommended that you hold onto handles rather than actions themselves.

# MADamage

This is a simple damage system meant to replace the built-in one (which is supposed to be deprecated/removed at some point anyway).

The design is largely similar, but with a focus on using gameplay tags and allowing the actor being damaged to respond to the thing damaging it.

## IMADamageableInterface

Implement this interface in either C++ or blueprint on any actor that can potentially be damaged. When applying damage, `OnIncomingDamage` is called
and a reference to the damage event is passed in - this event both contains information about the incoming damage and also allows the actor to let the
damager know how the damage was responded to via gameplay tags. The event can return false to indicate the actor wasn't damageable.

## FMADamageResponse

This structure is used to pass around data about a damage event - the target, the damage causer and instigator, etc.

`IncomingTags` can be used to indicate the type of damage or any special effects caused by the damage. `ResponseTags` can be used by the target to indicate
how the damage was responded to. `Damage` indicates the amount of damage to apply, but the target is free to change this value if it will not take the full amount.

## UMADamageTypeBase and UMATaggedDamageType

Used for damage "archetypes" - instead of having to apply tags and other values to a damage event manually every time you apply damage, you can simply pass in a subclass
of `UMADamageTypeBase`. Unlike the built-in damage system, this class is not passed around with the event and instead gets an early opportunity to modify the event itself.

`UMATaggedDamageType` is a simple subclass that applies a list of tags to the damage event's incoming tags.

## UMADamageLibrary

This is the primary point of interaction for applying damage - use either `ApplyDamage` or `ApplyDamageFromHit` to apply damage to an actor.

# MAGameplay

Generic gameplay utilities.

## UAIStateComponent and UPawnAIStateComponent

Very simple "state machine" components that use behavior trees for each state. While behavior trees themselves can be used for this sort of task, this makes it easier to create smaller
reusable behaviors.

For example, if you have different idle and combat behaviors for an enemy and you want to be able to control them more easily both within the enemy and from the level's own logic, you
could have separate behavior trees for each and assign them to separate idle and combat states.

This is *very* simple and made to fit a specific workflow, though the `FAIState` structure may be updated in the future to support more options or even constructs beyond behavior trees.

### Controller vs Pawn

`UAIStateComponent` is the "brain" of the system and must be placed on an `AAIController`. It manages the state of the AI itself. It also lets you configure states and behaviors.

`UPawnAIStateComponent` is a container for additional or overriding states on a specific pawn. States here will replace states with the same tag in the AI controller's component. This
can be useful if, for example, you have a generic AI controller for enemies with some common states but you have one specific enemy type that needs to act slightly differently. Or if
you want a specific enemy placed in a level to have different behaviors than the normal version.

## Behavior Tree Nodes

### Composite: Random

Acts like a selector but randomly selects children to execute instead of going from left to right. By default all children have a weight of 1.0, but you can adjust the weighting by
adding a "Random Weight" decorator to your child nodes.

If `bExcludeFailedChildren` is true then any child that fails will be excluded from subsequent random selections until the random composite is exited entirely. If `bExcludeFailedChildren` is
false then a child can be selected again even immediately after it fails.

# MASmartObjects

Extensions to the UE5 SmartObjects plugin.

# MAStateTree

Common tasks, evaluators, etc for the UE5 StateTree plugin. Requires UE5.1+.