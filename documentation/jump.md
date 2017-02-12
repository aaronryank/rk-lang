### void add\_jump(FILE \*src, char \*name)

  Add a jump named `name` with `src`'s current position.

### void jump(FILE \*src, char \*jump\_name)

  Find jump named `jump_name` in `jumps`.<br/>
  Call `fseek` on `src` with position in specified jump.

### int existing\_jump(char \*name)

  See if a jump named `name` exists.<br/>
  Return index if so, -1 otherwise.
