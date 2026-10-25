# 12 — Build a Text Adventure

A text adventure is an excellent first substantial program because it uses state, input, branching, and repeated decisions without needing graphics.

## Model the game

Start with a small map of locations and a list of game-state variables. For example:

- current room;
- whether the player found a key;
- whether power is restored;
- whether the player has collected an item;
- whether the game has ended.

Write the state transitions down before implementing the dialogue. A room should not only print text; player decisions should change state and affect later options.

## Separate story from engine

The interpreter is the engine: it parses and executes language constructs. The adventure is a program written in the language. Do not hard-code each room into the interpreter; that would make the game logic part of the language implementation.

## Make endings reachable

For every ending, write a route through the game. Also test impossible routes: for example, an exit that requires power should remain closed until the player restores it.

A simple state table helps:

| State | Action | Expected change |
|---|---|---|
| No key | Search security | Key becomes available |
| Has key | Activate generator | Power becomes active |
| No power | Try airlock | Exit remains locked |
| Power active | Try airlock | Escape ending is reachable |

Adapt this table to the exact rules of your game.

## Exercise

Add one room, one state variable, and one decision that affects a later room. Then add a test proving the decision matters.

## Checkpoint

Can the same interpreter run a second, unrelated source program without any changes to the interpreter itself?
