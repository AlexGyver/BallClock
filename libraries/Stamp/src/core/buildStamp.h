#pragma once
// https://stackoverflow.com/a/58314096

#define _BS_Y2K_DIFF 946684800ul

#define _BS_YEARS (__DATE__[10] - '0' + (__DATE__[9] - '0') * 10)

#define _BS_DAY_OF_MONTH ((__DATE__[5] - '0') + (((__DATE__[4] > '0')? __DATE__[4] - '0': 0) * 10) - 1)

#define _BS_DAY_OF_YEAR ((_BS_DAY_OF_MONTH) +        \
(  (__DATE__[0] == 'J' && __DATE__[1] == 'a') ?   0: \
   (__DATE__[0] == 'F'                      ) ?  31: \
   (__DATE__[0] == 'M' && __DATE__[2] == 'r') ?  59: \
   (__DATE__[0] == 'A' && __DATE__[1] == 'p') ?  90: \
   (__DATE__[0] == 'M'                      ) ? 120: \
   (__DATE__[0] == 'J' && __DATE__[2] == 'n') ? 151: \
   (__DATE__[0] == 'J'                      ) ? 181: \
   (__DATE__[0] == 'A'                      ) ? 212: \
   (__DATE__[0] == 'S'                      ) ? 243: \
   (__DATE__[0] == 'O'                      ) ? 273: \
   (__DATE__[0] == 'N'                      ) ? 304: \
                                              334  ))

#define _BS_LEAP_DAYS (_BS_YEARS / 4 + ((_BS_YEARS % 4 == 0 && _BS_DAY_OF_YEAR > 58) ? 1 : 0) )

#define BUILD_STAMP_LOCAL ( (_BS_YEARS * 365 + _BS_LEAP_DAYS + _BS_DAY_OF_YEAR ) * 86400ul   \
                            + ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0') * 3600ul        \
                            + ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0') * 60ul          \
                            + ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0') + _BS_Y2K_DIFF )

//