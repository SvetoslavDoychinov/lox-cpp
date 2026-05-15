# What is this project?

This is a C++ implementation of a tree walk interpreter based on the book [Crafting Interpreters](https://craftinginterpreters.com/classes.html) by Robert Nystrom.
The project is purely educational and for fun. It has no real application as there are plenty better programming languages. I've been working on this project for a while now in my spare time and as it's nearing completion and in a fully
functional, hopefully bug free state I decided to upload it.

> **WARNING** The project is still WIP. I haven't implemented OOP (the last 2 sections from the tree walk interpreter part of the book).

# Changes I've made to the original

Other than my implementation being in a different language I've made some other changes to Lox too.
Some of the changes I've made but not all include:

1. Completed numerous challenges from the book
2. Made ```print``` a builtin function rather than a statement
3. Added ```break``` and ```continue``` statements for ```for ``` and ```while``` loops.
4. Disallowed unassigned variables from being used, i.e. can't have a variable expression if you have only declared the variable but not assigned it.
5. Made block statements a mandatory part of all control flow statements.
6. I've made other changes too but at this point I don't remember them all. Maybe I will list them once the project is finished.
