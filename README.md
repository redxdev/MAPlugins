# A grab bag of project-agnostic plugins

This is a set of plugins that can be used in Unreal Engine 5. The plugins do not depend on each other unless otherwise
specified - you can use them individually.

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

Documentation forthcoming. Requires both MACommon and [UE5Coro](https://github.com/landelare/ue5coro) to be installed.
