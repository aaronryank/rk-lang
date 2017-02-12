### void error(int die, char \*fmt, ...)

  Prints `Error error_count+1: fmt ...\n`<br/>
  Increments error\_count.
  If `die` is `true`, call `clean_exit(-1)`.
