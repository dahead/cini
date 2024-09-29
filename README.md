# INI editor in C

## Usage

- r/read <filename> <category> <key>             - Reads the value of a key under a category
- w/write <filename> <category> <key> <new_value> - Updates the value of a key under a category

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


