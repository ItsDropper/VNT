# 12 — Build a Text Adventure

## Goal

Make a small game, then compare it with the larger game in the repository.

## Step 1: type a playable game with state

Create `vnx/game/my_first_adventure.vnx`:

~~~text
set has_key "no"
label room
say "THE LOCKED ROOM"
say "Choose: 1) window  2) door  3) desk"
input choice "> "
when choice == "1" jump window
when choice == "2" jump door
when choice == "3" jump desk
say "That is not a valid choice."
jump room
label desk
say "You find a brass key."
set has_key "yes"
jump room
label window
say "The window opens onto a safe ledge. You escape."
end
label door
when has_key == "yes" jump unlocked_door
say "The door is locked. Search the desk for a key."
jump room
label unlocked_door
say "The key turns. You escape through the door."
say "ENDING: ESCAPED"
end
~~~

Run the interpreter and enter `1`; then run it again and enter `2` without visiting the desk. The door should be locked. Run it a third time, enter `3` to find the key, then enter `2`; the door should open. Also test an invalid answer. This demonstrates state persisting across labels.

## Step 2: trace the state change

Before visiting the desk, `has_key` contains `"no"`. The desk route changes it to `"yes"` and jumps back to `room`. When the player later chooses the door, the conditional jump sends the player to `unlocked_door`.

## Step 3: inspect the full game

Open [The Last Signal](../../../../vnx/game/last_signal.vnx). It uses output, input, variables, comparisons, labels, and jumps to create multiple rooms and endings. The story is in the .vnx file, not hard-coded into the interpreter.

The existing endings are `ENDING: HOMEWARD`, `ENDING: CONTAINMENT`, and `ENDING: THE SIGNAL`. The CI workflow contains scripted routes to test endings.

## Exercise

Add a secret label and a state variable. Make an ending reachable only after a specific choice, then write down the exact input sequence that reaches it.

## Checkpoint

Can the game run on the same interpreter without changing `interpreter.vnt`? If yes, story logic is separate from the language engine.
