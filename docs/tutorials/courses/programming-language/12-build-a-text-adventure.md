# 12 — Build a Text Adventure

## Goal

Make a small game, then compare it with the larger game in the repository.

## Step 1: type the first game

Create `vnx/game/my_first_adventure.vnx`:

~~~text
say "THE LOCKED ROOM"
say "Choose: 1) window  2) door"
input choice "> "
when choice == "1" jump window
when choice == "2" jump door
say "That is not a valid choice."
end
label window
say "The window opens onto a safe ledge. You escape."
end
label door
say "The door leads into a dark corridor."
say "ENDING: THE CORRIDOR"
end
~~~

Run the interpreter and enter `1`. Run it again and enter `2`. Test an invalid answer too.

## Step 2: add state

Add this near the beginning:

~~~text
set has_key "no"
~~~

Create a `label desk` section that says you found a key, sets `has_key` to `"yes"`, then jumps back to the menu. Add a `3) desk` option and a condition that jumps to `desk` when choice is `"3"`.

Now make a locked-door route. If `has_key` equals `"yes"`, jump to an unlocked-door label; otherwise print that it is locked. Test both routes.

## Step 3: inspect the full game

Open [The Last Signal](../../../../vnx/game/last_signal.vnx). It uses output, input, variables, comparisons, labels, and jumps to create multiple rooms and endings. The story is in the .vnx file, not hard-coded into the interpreter.

The existing endings are `ENDING: HOMEWARD`, `ENDING: CONTAINMENT`, and `ENDING: THE SIGNAL`. The CI workflow contains scripted routes to test endings.

## Exercise

Add a secret label and a state variable. Make an ending reachable only after a specific choice, then write down the exact input sequence that reaches it.

## Checkpoint

Can the game run on the same interpreter without changing `interpreter.vnt`? If yes, story logic is separate from the language engine.
