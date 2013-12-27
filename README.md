Welcome to the **Yarp** project!

**Some notes from the (only) contributor**

At this stage, the code really consists of some scaffolding - only the basics. This also means that - barring some unit-tests - there is currently not much to look at (more on this below). However, as this project develops, more and more features will see the light of day.

Currently, development is (unit-) test-driven and so evolution will happen likewise. This means that while there is little controversy over "design" issues (see an outline of some abstractions and tooling decisions below) right now, things may change in the future, as concepts like an "engine" (or "entity driver" ?) or similar "higher-level/server-side" logic evolves. ATM, bottom-up suits me just fine; other motivated contributors may follow this approach by:

* implementing a new feature and writing a testcase alongside to check whether it works as it should
* upgrading a feature and extending/updating any existing test code accordingly
* start a concept prototype/design discussion (on a branch/in this wiki ?) about what a missing feature could look like

Existing funcionality has been grouped into modules (directories) reflecting the intermediate project structure. In alphabetical order, these are:

* *chance*: common functionality related to randomness
* *chance/dice*: simulation of dice rolls
* *character*: common characteristics of player- and non-player- entities
* *character/monster*: characteristics of adversaries
* *character/player*: characteristics of player characters (PCs)
* *client*: (graphical) UI
* *combat*: (round-based) melee (includes concepts for damage, different forms of attack, ...)
* *common*: some concepts (includes schemas for terrain, environment, climate and tools, ...)
* *engine*: logic to handle entities and level state
* *graphics*: glue code/data realizing surfaces, tiles, windows, cursors and other elements
* *item*: characteristics of objects (e.g. tools, weapons and armor, ...)
* *magic*: characteristics of magical objects and attributes (i.e. spells and special abilities)
* *map*: structures, related algorithms and path-finding logic of area maps
* *net*: common logic of the client/server architecture
* *net/client*: client-side networking code
* *net/protocol*: low-level specifics of networked interactions
* *net/server*: server-side networking code
* *net/stream*: low-level implementation of the pipes-and-filters pattern
* *sound*: glue code/data realizing audio playback
* *test_u*: unit-test code
* *tools*: additional tooling
* *Yarp*: meta-module, facilitates high-level project work

**Overview of existing functionality**

The implemented unit test program names are self-describing:

* chance_dice: simulate rolling dice
* chance_rangeToRoll: convert a requested range of values (e.g. 2-12) to an (approximated) appropriate dice roll (e.g. 2d6)
* monster_parser: load/dump monster library
* character_generator[_gui]: generate a player character (text-based / UI)
* character_parser: load/store a player character to/from a file
* rpg_client_gui: client GUI (GTK/glade & SDL)
* combat_simulator: simulate auto-combat (ATM text-based only)
* graphics_parser: load graphics library
* SDL_gui: SDL library integration unit-test
* item_parser: load/dump item library
* magic_parser: load/dump magic library
* map_generator: generate area maps
* map_parser: load/store area maps
* map_vision[_ui]: line-of-sight (LOS) unit-test (text-based / UI)
* path_finder[_ui]: path-finding (A*) unit-test (text-based / UI)
* net_client: network client stub
* net_server: network server stub
* IRC_client_gui: network client UI (GTK/glade), currently implements the IRC protocol (incomplete)
* sound_parser: load/dump sound library

The current client UI can load/display an area map using tiles in an isometric ("2.5D") projection using a defined style for walls, floor and doors. The screen can be scrolled with the mouse or the cursor keys. Using the cursor keys, a player character can be moved around the map. Doors can be opened by clicking on them when the player is within range. Monsters are spawned regularly and will attack the player, basic combat has been implemented.

**Some (early) design decisions**

* the (incomplete) ruleset is based on the d20 system reference (see [[http://www.d20srd.org/index.htm]])
* adapted some concepts and ripped some 2D-art/soundwork from the *vulture* nethack GUI (see [[http://www.darkarts.co.za/vulture]])

**Some implementation decisions**

* C/C++ as primary language
* GNU g++ as primary compiler
* libc/libstdc++/STL to implement basic types/algorithms
* ACE to implement low-level patterns and OS abstraction (see [[http://www.cs.wustl.edu/~schmidt/ACE.html]])
* Xerces/XSD/XML is used as schema, entity model and bindings (see [[http://www.codesynthesis.com/products/xsd]])
* SDL (currently 1.2) is used to implement low-level graphics representation (see [[http://www.libsdl.org]])
* GLib/GTK+/libGlade is used to implement the current client prototype (see [[http://library.gnome.org/devel/references]])

**Other technical decisions**

* Linux as primary development/target platform, Win32 platform support
* configuration management through CMake and (!) Autotools; KDevelop4/Anjuta/QtCreator project files are available
  but may not be consistent
* version management through git (see [[http://www.git.org]])
* online project hosting on github (see [[http://www.github.org/esohns/Yarp]])

** Some TODOs (to be specified)**
* support networked (client/server ?) multi-user functionality with protocols
* support SDL 2.0 (see [[http://www.libsdl.org]])
* support OpenGL in client UI (see [[http://www.darkarts.co.za/noegnud]])
* support 3D graphical client UI
