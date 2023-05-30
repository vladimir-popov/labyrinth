# Short naming convention

 - The "CameCase" is not used at all.
 - The abbreviations are acceptable.
 - The functions should be named only in snake_case.
 - The variables  should be named in snake_case, or as short abbreviation:
   ```c
   int row_lenght;
   // or:
   int rlen;
   ```
 - Enums should be named in snake_case and always should be used with `enum` key word:
   ```c
   enum my_enum
   ```
 - The type aliases should be named in Snake_Case with capital first letters.
 - Structures should be named in snake_case and always have type alias. In code, structures
   should be referenced only by their type aliases:
   ```c
   typedef struct {
    ...
   } My_Mtruct;
   // or:
   struct another_struct;
   typedef struct another_struct Another_Struct;
   ```
