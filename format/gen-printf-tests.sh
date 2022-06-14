#!/usr/bin/env bash

curl -LG 'https://github.com/BartMassey/printf-tests/raw/master/printf-tests.txt' \
  | awk '
/^ / {
  nfield = patsplit($0, fields, /"[^"]*"|[^" ]+/)

  if (fields[2] == "?") {
    next
  }

  argstr = fields[4]
  for ( i = 5; i <= nfield; ++i ) {
    argstr = argstr ", " fields[i]
  }

  # disable tests without arguments
  if (!argstr) {
    next
  }

  printf("/*%4s */ EXPECT_EQ(%22s, %16s_fmt ( %s ));\n",
         fields[1], fields[2], fields[3], argstr)
}
'
