# INI editor in C

## Usage

- r  <filename> <category> <key>             - Reads the value of a key under a category
- w  <filename> <category> <key> <new_value> - Updates the value of a key under a category
- lk <filename> <category>                   - Lists all keys and values under a category (or all categories if none is given)
- lc <filename>                              - Lists all categories in the file

## Alias

- r, read
- w, write

## Examples

Consider this ini file ```test.ini```

```
[Hardware]
Monitor1=LG
Monitor2=LG
Mouse=Lamzu Thorn
Printer=Ricoh
```

To add a Key and value issue this command:

```
./cini write test.ini CategoryNew NewKey NewValue
```

The changed ini ```test.ini``` looks like this now:

```
[Hardware]
Monitor1=LG
Monitor2=LG
Mouse=Lamzu Thorn
Printer=Ricoh

[CategoryNew]
NewKey=NewValue
```


