### void wait\_for\_character(FILE \*f, int delim, int print)

  Read `f` character by character until `delim` is reached.<br/>
  If `print` is `true`, print each character as they are read.

### void break\_from(FILE \*f, char \*open, char \*close)

  Used to break out of loops/if-statements.<br/>
  `open` specifies the opening statement (e.g. `if`), and `close` specifies the closing statement (e.g. `fi`).<br/>
  Calls itself if it hasn't broken from the loop/statement.
