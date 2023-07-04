# Code convention

External libraries should not be used in this project.

Code should be formatted according to [GNU Coding Standards](https://www.gnu.org/prep/standards/standards.html).

## Short naming convention

 - The "CamelCase" is not used at all.
 - Abbreviations are acceptable.
 - Functions should be named only in snake_case.
 - Variables should be named in snake_case, or as a short abbreviation:
   ```c
   int row_lenght;
   // or:
   int rlen;
   ```
 - Enums should be named in snake_case and always should be used with `enum` key word:
   ```c
   enum my_enum
   ```
 - Structures should be named in snake_case and always have type alias. In code, structures
   should be referenced only by their type aliases.
 - Type aliases for **domain** entities should be named in Snake_Case with capital first letters.
   ```c
   typedef struct {
    ...
   } My_Mtruct;
   // or:
   struct another_struct;
   typedef struct another_struct Another_Struct;
   ```
 - Type aliases for util structures may be an abbreviation:
   ```c
   typedef struct {
    ...
   } u8str;
   ```
