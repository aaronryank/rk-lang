## COMPUTE.C

### void compute.set(char \*keyword)

  Set `compute.in` to 1, defining that the source file is reading information to be computed.<br/>
  Set `compute.keyword`, defining what the language does after computation.<br/>
  Always succeeds.

### int compute.breakout(char \*keyword)

  Checks for a keyword that stops the language from reading information to be computed.<br/>
  Returns 1 if the keyword stops computation.<br/>
  Returns 0 otherwise.

### void compute.remove(int idx, int count)

  Removes `count` elements from `compute.op` starting at index `idx`.<br/>
  Always succeeds. Well, not really, but it keeps quiet when it fails.

### void compute.strmanip(int idx)

  Call if `compute.op[idx]` is '.' (string manipulation).<br/>
  Set `compute.op[idx-1]` to `compute.op[idx-1]`'s index stored in `compute.op[idx+1]`.<br/>
  Fails if `compute.op[idx-1]` is not a variable.

### int compute.compute()

  Compute the value of `compute.op`, clearing the array and setting it to the value. <br/>
  Called by `compute.logic` and `compute.assign`.

### void compute.logic()

  Evaluate `compute.op` by calling `compute.compute`. <br/>
  If true, continue parsing file as normal.<br/>
  If false, break from the logic statement/loop. If in while loop, decrement while loop count.

### int compute.add(char \*keyword)

  Call `compute.breakout` on `keyword`.<br/>
  If return value is 1, assign/evaluate `compute.op`.<br/>
  Otherwise, add `keyword` to `compute.op[compute.idx]`.

### void compute.assign()

  Evaluate `compute.op` by calling `compute.compute`.<br/>
  Assign `var_list[last.var_idx]` to computed value.

### void compute.reset()

  Reset all values in `struct compute` to 0.

### int getcmp(char \*val)

  Return computation index of `val`.<br/>
  e.g. `val == "x"`, return `MULT` (9). `val == "/"`, return `DIV` (10).

### void mod\_op(int (\*op)[], int idx, int range)

  `(*op)[idx-1]` shall be `pre` (an integer value), `(*op)[idx]` shall be `cur` (an operator), and `(*op)[idx+1]` shall be `post` (integer value). <br/>
  set `pre` to `pre cur post`.
