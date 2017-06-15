# Hierarchical Scene Component Visibility Manager
In Unreal Engine 4, the visibility of Scene Components is not maintained hierarchically. Let's say you have an actor structure like this:

- Root  (`bVisible = true`)
  - Mesh  (`bVisible = false`)
    - SubMesh (`bVisible = true`)
    
You will be able to see *Root* and *SubMesh*, but not *Mesh*.
If you set the visibility of *Root* to `false`, you can only see *SubMesh*. You can also specify the parameter `bPropagateToChildren` to `true`, which sets the visibility of all decendants down through the attachment tree as well, then you will see non of these three components.

However, if you want to show the tree again while leaving *Mesh* still invisible, you'll have to manage the visibilities manually and carefully, because setting visibility of *Root* to `true` and propagating it to all the children will make *Mesh* also visible. While this is a totally reasonable design without any overhead, sometimes it could be a pain in the ass to maintain this kind of visibility hierarchy. That's why I made this hierarchical visibility manager.

## Usage
1. This Visibility Manager consists of simply one header files. Drop it into the public source folder of your project (PROJECT_NAME/Source/PROJECT_NAME/Public).
2. Open the header file, change `YOURPROJECT_API` to your project name (capped, with no spaces) plus `_API`. You can copy-paste from other header files in your project if you are not sure.
3. Construct an instance of `Visibility Manager` in your blueprint. You'll only need one per world, so it's generally a good idea to do this in your `GameState`'s `BeginPlay` event, and store the instance in a variable.
4. Use the instance's `SetVisibility` and `SetActorVisibility` methods whenever you want to change the visibility of a Scene Component or an Actor. It's recommended to create a `GetVisibilityManager` method in a blueprint library so you can easily access it everywhere.

## Implementation Notes
This class employs a `TMap` to store hierarchical visibility state of components, but it doesn't bother to release for destroyed objects, so it does create a memory leak. However, since the memory footprint used for each component is very tiny, this shouldn't be a problem for typical use.
