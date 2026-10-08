# Bleky

**Bleky is a simple ECS designed around the spirit of Rule of Zero, minimizing the amount of boilerplate code users need to write for copying, moving, and resource management. The goal is to let developers focus on designing clean, simple classes whose properties naturally provide the behavior they need.**

Most Bleky components can be default-constructed when desired. Objects created this way are not attached to a manager and therefore remain independent from the managed ECS world. This makes them useful for isolated objects, experimentation, temporary copies, or other situations where an element should exist without participating in the ECS.

Entities can also be active or inactive. An inactive entity remains part of the ECS but is ignored by the systems it is associated with, allowing its processing to be temporarily disabled without removing it from the world.

Systems do not need to be manually attached to entities. Instead, when an entity receives a component, the component itself determines which system it belongs to, allowing the appropriate systems to be associated automatically.

Elements can be created directly through their respective managers using their add methods, or existing elements can be attached to a manager afterward. Once attached, they become part of the managed ECS world and can participate throughout the entire ecosystem of entities, components, and systems.

Bleky also provides system groups with priorities, allowing modules to place their systems into the appropriate execution containers without accidentally mixing unrelated systems. Within each group, systems can be positioned precisely using anchors: a system can be inserted at a specific position, at the beginning, at the end, or relative to another system. By default, the insertion anchor advances automatically as systems are added, making sequential system registration straightforward while still allowing precise control when needed.

**Bleky's goal is to keep ECS code lightweight and predictable: let developers define simple objects and their behavior, while the framework handles ownership, relationships, registration, and system organization.**
