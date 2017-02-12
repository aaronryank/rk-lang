### void read\_loop(FILE \*src, FILE \*dest)

  Read through `src` until `rk:start` is read, ignoring all data.<br/>
  Continue reading, sending strings to `rk_parse`. Stops reading when `EOF` is read.
