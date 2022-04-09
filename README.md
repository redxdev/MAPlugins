# MACommon: A grab bag of common utilities.

This is my catch-all library for project-agnostic utilities in Unreal Engine 5.

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