This is an automatic translation, may be incorrect in some places. See sources and examples!

# Stamp
Library for storage and manipulation over time
- more convenient to use than built -in Time.h
- Faster transformation algorithms are used than in Time.h
- keeps the date and time of 4 bytes of the variable (seconds from 01.01.2000)
- transforms a year, month, day, hours, minutes, seconds, day of the week, day of the year
- can count and maintain time on the basis of Millis ()
- Parsing of lines
- works ~ until 2106

## compatibility
Compatible with all arduino platforms (used arduino functions)

## Content
- [documentation] (#reference)
- [Example] (# Example)
- [versions] (#varsions)
- [installation] (# Install)
- [bugs and feedback] (#fedback)

<a id="reference"> </a>

## Documentation
### Timezone
`` `CPP
VOID setstampzone (int Zone);// Install the global time zone in hours or minutes
int Getstampzone ();// Get a global time zone
`` `

### Datime
Saves date and time in separate variables:
`` `CPP
uint16_t year;// year
uint8_t months;// month (1 .. 12)
uint8_t day;// Day of the month (1 .. 28-31)
Uint8_t Hour;// hour (0 .. 23)
uint8_t minute;// minute (0 .. 59)
uint8_t second;// second (0 .. 59)
uint8_t Weekday;// Day of the week (1 Mon .. 7 Sun) ISO 8601
uint16_t soarday;// Day of the year (1 .. 365-366)

// The indicated ranges are true for all other functions of the library!
`` `

`` `CPP
// ============ Design ===============
Datime;
Datime (UNIX);// Unix Time
Datime (year, month, day);// year, month, day
Datime (Hour, Minute, Second);// hour, minute, second
Datime (year, month, day, hor, minute, second);
Datime (Consta Char* str);// Line of the type yyyy-mm-ddthh: mm: ss

// ============= Installation =====================
VOID SET (UNIX);// Unix Time
VOID SET (Year, Month, Day);// year, month, day
VOID SET (Hour, Minute, Second);// hour, minute, second
VOID SET (year, Month, Day, Hour, Minute, Second);

// ============== Parsing ====================
Bool Parsedate (Consta Char* S);// from the line of the type yyyy-mm-dd
Bool Parsetime (Consta Char* S);// from the line of the species hh: mm: ss
Bool Parse (Const Char* S);// from the line of the type YYYY-MM-DD or hh: mm: ss or yyyy-mm-ddthh: mm: ss
Bool Parsehttp (Consta Char* S);// from the line of species <day_week>, dd <Month> yyyy hh: mm: ss

// ============== EXPORT ========================
uint32_t toseconds ();// Bring time per second (excluding the date)
uint32_t getunix ();// Bend into unix seconds

// ============= In the line ======================
// Bring the date in the format "DD.mm.yyy"
Char* DATETOChar (Char* BUF);
String Datetostring ();

// Bring the time in the format "hh: mm: ss"
Char* Timetochar (Char* Buf);
String Timetosting ();

// Determine in DD.mm.yyyy hh: mm: ss, div - division
Char* toChar (char* buf, char div = '');
String Tostring (Char Div = '');

// ============= add ===================
VOID AddSeconds (Uint32_T S);// Add seconds
VOID Addminutes (Uint32_T M);// Add minutes
VOID Addhours (Uint32_T H);// Add a watch
VOID Adddays (Uint32_T D);// Add days

VOID NEXTSECOND ();// Next second
VOID NEXTMINUTE ();// The next michicks (XX: XX: 00)
VOID NEXTHOUR ();// Next hour (XX: 00: 00)
VOID NEXTDAY ();// The next day (00:00:00)
VOID NEXTMONTH ();// Next month (1st number 00:00:00)

VOID updatedys ();// update Weekday and Yearday based on the current date (after manual change)

// ============== Engorithm ======================
// The time transformation algorithm is set before connecting the library
// for research and just so that it was
#define unix_alg unix_alg_0 // ~ 402us and ~ 94b flash (avr)
#define unix_alg unix_alg_1 // ~ 298us and ~ 138b flash (avr)
#define unix_alg unix_alg_2 // ~ 216us and ~ 584b flash (avr)
#define unix_alg unix_alg_3 // ~ 297us and ~ 178b flash (avr)
#define unix_alg unix_alg_time_t // ~ 246us and ~ 842b flash (avr)
`` `

> All time adds in Datime are performed directly, without converting in UNIX, i.e.Pretty fast.

### Stamp
Keeps the date and time in one variable `uint32_t` - unix time

`` `CPP
// ============ Design ===============
Stamp;
Stamp (uint32_t unix);// from UNIX
Stamp (Datime & DT);// from Datime
Stamp (year, month, day);// year, month, day
Stamp (Hour, Minute, Second);// hour, minute, second
Stamp (const char* str);// from the line of the type YYYY-MM-DD or hh: mm: ss or yyyy-mm-ddthh: mm: ss
Stamp (year, month, day, hor, minute, second);

// =========== Institute =================
VOID SET (Datime & DT);// Install from Datime
VOID SET (Year, Month, Day);// year, month, day
VOID SET (Hour, Minute, Second);// hour, minute, second
VOID SET (year, Month, Day, Hour, Minute, Second);

Bool Parse (Const Char* S);// from the line of the type YYYY-MM-DD or hh: mm: ss or yyyy-mm-ddthh: mm: ss
Bool Parsehttp (Consta Char* S);// from the line of species <day_week>, dd <Month> yyyy hh: mm: ss

// ============== EXPORT ========================
Datime Get ();// Export to Datime format
Void Get (Datime & DT);// Export to the Datime type variable
uint32_t toseconds ();// Get seconds
uint32_t Tominutes ();// Get minutes
uint32_t tohours ();// Get a clock
uint32_t Todays ();// Get a day

// ============= In a line ======================
// Bring the date in the format "DD.mm.yyy"
Char* DATETOChar (Char* BUF);
String Datetostring ();

// Bring the time in the format "hh: mm: ss"
Char* Timetochar (Char* Buf);
String Timetosting ();

// Determine in DD.mm.yyyy hh: mm: ss, div - division
Char* toChar (char* buf, char div = '');
String Tostring (Char Div = '');
`` `

### Stampticker
The same `Stamp`, but saves the time on the basis of` millis () `after synchronization.Signaling once per second, you can connect the handler.It has a ticker, you need to call it in `look ()`.

`` `CPP
// Install the current unix, additionally milliseconds of synchronization
Stampticker (uint32_t unix, uint16_t ms = 0);

// Install the current unix, additionally milliseconds of synchronization
VOID update (uint32_t unix, uint16_t ms = 0);

VOID Attach (F);// Connect the function-processor of the operation (type VOID F ())
VOID Detach ();// Disconnect the function-cutter

Bool Ready ();// Returns True every second
Bool Synced ();// Time synchronized
uint16_t ms ();// Get milliseconds of the current second
uint32_t calcunix ();// Calculate and get the current unix

// ticker, call in LOOP.Unix updates once a second.Will return True every second taking into account MS synchronization
Bool Tick ();
`` `

### StampSync
The same `Stamp`, but saves the time on the basis of` millis () `after synchronization.It works without a ticer.

`` `CPP
// Install the current unix, additionally milliseconds of synchronization
StampSync (uint32_t unix, uint16_t ms = 0);

// Install the current unix, additionally milliseconds of synchronization
VOID update (uint32_t unix, uint16_t ms = 0);

Bool Synced ();// Time synchronized
uint32_t getunix ();// Get the current unix
uint16_t ms ();// Get milliseconds of the current second
`` `### utilities
Set of functions for working with time and date

`` `CPP
// Time per second
uint32_t Stamputils :: Timetoseconds (Uint8_t Hours, Uint8_t Minutes, Uint8_T Seconds);

// leap year
Bool Stamputils :: isleap (uint16_t year);

// days in the month without accounting for the year (February 28)
uint8_t Stamputils :: Daysinmonth (Uint8_t Month);

// days in the month taking into account the leap year
uint8_t Stamputils :: Daysinmonth (Uint8_t Month, Uint16_t Year);

// Days of the year to the month (Jan 0, Feb 31, March 59/60 ...)
uint16_t Stamputils :: Daystomonth (Uint8_t Month, Uint16_t Year);

// Date on the day of the current year (starting with 1)
uint16_t Stamputils :: DateToyearday (Uint8_t Day, Uint8_t Month, Uint16_t Year);

// Date on the day of the week (PN 1 .. Sun 7)
uint8_t Stamputils :: DateTeWeekday (Uint8_t Day, Uint8_t Month, Uint16_T Year);

// Date in the number of days from 01.01.2000 (starting with 0)
uint16_t Stamputils :: DateTodays2000 (Uint8_t Day, Uint8_t Month, Uint16_T Year);

// Date in UNIX time, zone in minutes
uint32_t Stamputils :: DateTouNix (Uint8_t Day, Uint8_t Month, Uint16_t Year, Uint8_t Hour, Uint8_t Minute, Uint8_T Seconds, Int16_T Zone = 0);

`` `

<a id="EXAMPLE"> </a>

## Example
`` `CPP
Stamp S (2023, 5, 8, 16, 26, 0);
Serial.println (S.Getunix ());
Serial.println (S.Tostring ());

Datime D = S.Get ();
Serial.println (D.Year);
Serial.println (D.MONTH);
Serial.println (D.Day);

D.YEAR = 2022;// changed the year
S.Set (D);// updated Stamp
Serial.println (S.Tostring ());
`` `

<a id="versions"> </a>
## versions
- V1.0
- V1.1 - added many new tools and capabilities

<a id="install"> </a>
## Installation
- The library can be found by the name ** Stamp ** and installed through the library manager in:
    - Arduino ide
    - Arduino ide v2
    - Platformio
- [download library] (https://github.com/gyverlibs/stamp/archive/refs/heads/main.zip). Zip archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/ *
    - (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!

<a id="feedback"> </a>
## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code