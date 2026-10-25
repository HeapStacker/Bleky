# Bleky

![C++](https://img.shields.io/badge/C++-23-orange.svg)

Bleky is a C++23 entity-component-system library. The program is one `App`. The app owns named scenes. A scene is a tree of entities, and it is also the place where systems, scene resources, and scene events live. Entities hold components. A component decides which systems should see its entity. A system is a function that walks those entities.

The library is a static library. Behavior that must be generated for each component, resource, and event type lives in the headers. The frame loop, scene graph, and system scheduling are compiled from `src/`.

Storage, polymorphic copy, and destruction go through [Vault](https://github.com/HeapStacker/Vault). Failed lookups and similar diagnostics go through [Logger](https://github.com/HeapStacker/Logger).

## Build

The desktop build is a Pixi task. It configures xmake and compiles the static library target `Bleky`.

```bash
pixi run buildDesktop
```

xmake requires C++23 and treats every warning as an error. On Windows the toolchain is MSVC, with `WIN32_LEAN_AND_MEAN`, `NOMINMAX`, and UTF-8 source. Logger and Vault are xmake packages fetched from GitHub while the library is built, and both are public dependencies of `Bleky`.

Headers under `include/bleky` include headers under `private/bleky`. A target that uses the library needs both directories on its include path, then it links the static library. The umbrella header is `bleky/ECS.hpp`. It pulls in the public API and Logger.

There is no application target in this repository. `Bleky` is the library.

## The frame

`App::run` is the loop. It runs while the app is alive. `App::Get().destroy()` ends it.

Each iteration does six things, in this order:

1. It measures `deltaTime` as the wall-clock seconds since the previous iteration started.
2. It increments the frame counter.
3. It delivers the events queued on the app.
4. If a scene is active, it runs that scene: scene events, then systems, then the scene's removal sweep.
5. It applies a scene change that was requested during this frame.
6. It erases scenes that were destroyed during this frame, then sleeps so the iteration lasts `1 / fps` seconds. `fps` defaults to `60`. If the work already took longer than that, it does not sleep.

`deltaTime` is the time the last iteration actually took, not `1 / fps`. The first iteration is special: the previous timestamp starts at the clock's epoch, so the first `deltaTime` is huge. Later frames are normal.

`App::Get().fps`, `deltaTime`, and `getFrameCount()` are how gameplay code reads that clock. Systems do not receive the time as an argument.

## App and scenes

`App` is a singleton. `App::Get()` creates it on first use.

```cpp
auto &app = bleky::App::Get();
auto &scene = app.add("main");
app.run();
```

`add` creates a scene under that name. `has` and `get` look one up. Asking for a missing scene, or adding a name that already exists, fails through Logger.

The first scene becomes the active one as it is created. Every later scene stays stored on the app and does nothing until it is activated:

```cpp
app.add("pause").activate();
```

`activate` makes that scene the one that will run. If a scene is already running, the switch happens after the current frame, so the scene that called `activate` still finishes its own systems. The app runs only one scene per frame.

A scene is the root entity of its own tree. It also has its own system groups, its own resources, and its own event bus. Those are not the app's. A texture fetched from the app is invisible to `scene.fetchResource`, and an event emitted on the app is not delivered to subscribers on the scene.

## Entities

An entity is a named node. A scene adds a root-level entity with `scene.add`. An entity adds a child with `entity.add`. Both can also copy an existing entity. The three `add` overloads are a new name, a copy of another entity, or a Vault container that is moved in.

```cpp
auto &player = scene.add("player");
auto &weapon = player.add("weapon");
```

Those two calls do not mean the same thing for simulation. `Scene::add` activates the new entity. `Entity::add` leaves the child inactive. Systems only see active entities, so `weapon` stays out of every system until `weapon.activate()`. `activate(false)` puts it back out. `active()` reports the flag.

Activation walks up to the scene. An entity that is not attached to a scene cannot be activated.

Children of one parent are found by name: `has`, `get`, `getAll`, `empty`. `getName` and `rename` use the entity's own name. If `add` is given a name that this parent already has, the new entity is still created, under the next free number: `"orc"` becomes `"orc1"`, and `"orc1"` becomes `"orc2"`.

`getScene()` walks parents until it reaches the scene. Copying an entity clones its components and its children.

## Components

A component type inherits `BLEKY_COMPONENT(YourType)`. That macro makes it a `Component` that Vault can clone, which is what entity copy uses.

```cpp
struct Velocity : BLEKY_COMPONENT(Velocity) {
    float dx = 0.f;
    float dy = 0.f;

    Velocity(const std::string &name, bleky::Entity *owner, float dx, float dy)
        : BLEKY_COMPONENT(Velocity)(name, owner), dx(dx), dy(dy) {}
};
```

The constructor always receives the component name and the owning entity, then whatever arguments you pass to `addComponent`. `addComponent<Velocity>(1.f, 2.f)` stores it under the empty name. `addNamedComponent<Velocity>("knockback", 4.f, 0.f)` stores another `Velocity` on the same entity under a different name. A second add with the same type and the same name returns the component that is already there.

```cpp
player.addComponent<Velocity>(1.f, 2.f);
player.getComponent<Velocity>().dx += 1.f;
player.hasComponent<Velocity>();
player.getAllComponents<Velocity>();
player.removeComponent<Velocity>();
player.removeAllComponentsOfType<Velocity>();
player.removeAllComponents();
```

`getComponent` and `hasComponent` default to the empty name. There is also an `addComponent` that copies an existing component instance.

A component chooses its systems. `registeredSystems` is the list of system functions that should see this entity. `systemGroup` is the name of the group on the scene that owns those systems. It defaults to `"clientGroup"`. Both fields are protected, so the derived constructor sets them.

When the component is added, Bleky inserts the entity into each of those systems, in a bucket keyed by this component's type. The group must already exist on the scene. If that function is not in the group yet, the group inserts it after the anchor, so put the first system in yourself before any component does this. The same entity can be linked by several components. It is removed from a system only when the last component that registered that system is gone.

Only an active entity is placed where systems will read it. An inactive one is remembered and moved across when `activate` runs. Adding a component that registers systems also requires the entity to already be able to reach a scene, because the link is made immediately.

Do not call `destroy()` on a component. That override only logs a warning. Removal goes through `removeComponent`.

## Systems

A system is a function pointer:

```cpp
void (*)(bleky::EntityMap &)
```

`EntityMap` maps a component `std::type_index` to the entities linked under that type. `BLEKY_ENTITIES` reads one bucket. It uses `operator[]`, so a type that nobody linked yields an empty list and inserts that empty list into the map.

```cpp
void move(bleky::EntityMap &entities) {
    for (bleky::Entity *entity : BLEKY_ENTITIES(entities, Position)) {
        auto &position = entity->getComponent<Position>();
        auto &velocity = entity->getComponent<Velocity>();
        float dt = bleky::App::Get().deltaTime;
        position.x += velocity.dx * dt;
        position.y += velocity.dy * dt;
    }
}
```

The bucket is `Position` only if a `Position` component registered `move`. Registering `move` from `Velocity` would put the entity under `Velocity` instead. The system can still `getComponent` any other component on that entity.

Systems are ordered by groups on the scene. A group has a name and a priority from 0 to 100 (`BLEKY_MAX_PRIORITY`). The default priority is 50. Higher priority runs first. `executeAll` walks the groups in that order, and each group runs its systems from front to back.

```cpp
auto &group = scene.addGroup("clientGroup");
group.addSystem(move, false);
group.addSystem(collide);
```

A new group has no systems, and its insertion anchor is the end of the list. The first `addSystem` therefore passes `false`, which inserts at the anchor. That system becomes the anchor. The next `addSystem` uses the default and inserts after the anchor, so the call order above runs `move`, then `collide`.

`hasGroup` and `getGroup` look a group up by name. Inside a group, `hasSystem`, `getCountOf`, and `getSystems` look up a function. The same function may be inserted more than once. `removeSystem` drops every entry of that function when the scene sweeps at the end of its run.

The anchor can be moved with `setAnchor` on a system entry or an index, `setAnchorOnLast`, or `setAnchorOnNth` / `setAnchorOnLast` for one function. Later insertions happen relative to wherever the anchor is.

`Position` in the example belongs to the default group, so its constructor only has to name the function:

```cpp
struct Position : BLEKY_COMPONENT(Position) {
    float x = 0.f;
    float y = 0.f;

    Position(const std::string &name, bleky::Entity *owner, float x, float y)
        : BLEKY_COMPONENT(Position)(name, owner), x(x), y(y) {
        registeredSystems.push_back(move);
    }
};
```

Create the group before adding that component. Otherwise the link looks up `"clientGroup"` and fails.

## Resources

A resource is shared data, not per-entity state. The type inherits `BLEKY_RESOURCE(YourType)`, for the same cloning reason as components. The app and each scene have separate stores.

```cpp
struct Texture : BLEKY_RESOURCE(Texture) {
    std::string path;

    Texture(const std::string &name, std::string path)
        : BLEKY_RESOURCE(Texture)(name, nullptr), path(std::move(path)) {}
};

auto &hero = scene.fetchResource<Texture>("hero", "hero.png");
```

`fetchResource` returns the existing object of that type and name, and in that case it ignores the extra arguments. If there is none, it constructs one. The derived constructor receives the name plus those arguments. It initializes the `Resource` base with that name and a null manager. The store attaches the manager itself after construction. Fetching the same name again returns the same object.

## Events

The app and each scene have a separate bus. `subscribe` registers a `std::function<void(const T &)>`. `emit` copies the event into a queue. Nothing is delivered inside `emit`.

```cpp
struct Hit { bleky::Entity *target; };

scene.subscribe<Hit>([](const Hit &hit) {
    hit.target->activate(false);
});

scene.emit(Hit{&player});
```

The app delivers its queue at the start of the frame, before the scene runs. The scene delivers its queue at the start of its own run, before systems. An event emitted from a subscriber, or from a system, waits for the next delivery. Subscribers therefore do not see events that were emitted while they were running.

## Removal

`remove` on a parent and `removeComponent` on an entity start destruction immediately. A component's destruction unlinks the entity from that component's systems once no component on the entity still registers the system. An entity's destruction destroys its components and its children. A scene's destruction clears the active-scene pointer if it pointed at that scene, and records the scene so the app erases it after the frame.

The scene sweep at the end of `run` erases names that `Item` recorded, and it erases systems passed to `removeSystem`. `Entity` and `Component` replace `onDestroy` and do not call `Item`'s version, so that sweep does not erase the entity or the component from its manager. `Scene` records its own name, and the app does erase it.

`clearRemoved` is part of that sweep. Scene code calls it. Other code does not.

## Ref

`bleky::cmp::Ref` is a built-in component that stores a pointer to an item. `to` sets it. `get<T>` casts it back, and the caller has to name the real type. Both are function templates, and their definitions are in `src/components/Ref.cpp`, so a call from another translation unit does not link.

## A whole frame

```cpp
#include <bleky/ECS.hpp>

struct Velocity : BLEKY_COMPONENT(Velocity) {
    float dx = 0.f;
    float dy = 0.f;

    Velocity(const std::string &name, bleky::Entity *owner, float dx, float dy)
        : BLEKY_COMPONENT(Velocity)(name, owner), dx(dx), dy(dy) {}
};

void move(bleky::EntityMap &entities);

struct Position : BLEKY_COMPONENT(Position) {
    float x = 0.f;
    float y = 0.f;

    Position(const std::string &name, bleky::Entity *owner, float x, float y)
        : BLEKY_COMPONENT(Position)(name, owner), x(x), y(y) {
        registeredSystems.push_back(move);
    }
};

void move(bleky::EntityMap &entities) {
    float dt = bleky::App::Get().deltaTime;
    for (bleky::Entity *entity : BLEKY_ENTITIES(entities, Position)) {
        auto &position = entity->getComponent<Position>();
        auto &velocity = entity->getComponent<Velocity>();
        position.x += velocity.dx * dt;
        position.y += velocity.dy * dt;
    }
}

int main() {
    auto &app = bleky::App::Get();
    auto &scene = app.add("main");
    scene.addGroup("clientGroup").addSystem(move, false);

    auto &player = scene.add("player");
    player.addComponent<Position>(0.f, 0.f);
    player.addComponent<Velocity>(1.f, 2.f);

    app.run();
}
```
