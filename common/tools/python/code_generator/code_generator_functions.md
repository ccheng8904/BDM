# Code Generator Functions
Code Generator provides user number of functions that can be used in code generator expressions to generate code. For a detailed information of code generator functions see their definitions in [code_generator.py](code_generator.py).
<br>


## `print()`
Prints given input to command-line console, e.g.:
```bash
<$print("Hello world!")>
```
Output of code example is: `Hello world!`
<br>


## `$()`
Expands the value of the given variable, e.g.:
```bash
<$define(str, "Hello")>
<$define(num, 0)>

<$print($(str))>
<$print($(num))>
```
Output of code example is:
<br>`Hello`
<br>`0`
<br>


## `define()`
Defines a variable with name and value given as function arguments `define(var_name, var_value)`, e.g.:
```bash
<$define(var, 0)>
<$define(str, "Hello world")
```
<br>


## `undef()`
Undefines a variable given to have no value assigned, e.g.:
```bash
<$undef(var)>
```
<br>


## `ifdef()` / `ifndef()`
If given variable is defined / not defined, conditionally do either this or that in the other case. Action for the false case is not mandatory.
`if[n]def(var, action_if_true, [action_if_false])`, e.g.:
```bash
<$ifdef(var, "DEFINED", "NOT DEFINED")>
<$ifndef(var, define(var, "Hello"))>
<$ifdef(var, "DEFINED")>
```
Output of code example is:
<br>`NOT DEFINED`
<br>`DEFINED`
<br>


## `ifeq()` / `ifneq()`
If two variables are equal / not equal, conditionally do either this or that in the other case. Action for the false case is not mandatory.
`if[n]eq(var1, var2, action_if_true, [action_if_false])`, e.g.:
```bash
<$define(str1, "Hello")>
<$define(str2, "Hello")>
<$ifeq(str1, str2, "Equal")>

<$define(str2, "Helloo")>
<$ifneq(str1, str2, "Not equal", "Equal")>
```
Output of code example is:
<br>`Equal`
<br>`Not equal`
<br>


## `len()`
Returns number of items in list or dictionary, e.g.:
```bash
<$define(list, [{"name":"Year", "days":365}, {"name":"Week", "days":7}, {"name":"Month", "days":30}])>

<$print("Number of list's items is:")>
<$print(len(list))>
```
Output of code example is:
<br>`Number of list's items is:`
<br>`3`
<br>


## `subst()`
Substitutes a `pattern` in a `string` with specified `replacement_string`: `subst(pattern, replacement_string, string)`, e.g.:
```bash
<$define(my_str,"Hi, I am from Slovakia.")>
<$subst(" ", "-", my_str)>
```
Result string will be: `Hi,-I-am-from-Slovakia.`
<br>


## `toupper()`
Converts letters in given string to upper-case, e.g.:
```bash
<$define(my_str,"Hi, I am from Slovakia.")>
<$toupper(my_str)>
```
Result string will be: `HI, I AM FROM SLOVAKIA.`
<br>


## `calc()`
Performs basic mathematical operations (+ , - , * , / , %) with given variables (either of type string or integer), e.g.:
```bash
<$define(num1, 10)>
<$define(num2, 20)>

<$print(calc($(num1) + $(num2) - 1))>
```
Output of code example is: `29`
<br>


## `inc()`
Increments given integer with increment value (if not specified, by default increment value equals one) `inc(int_name, increment_value)`, e.g.:
```bash
<$define(num, 5)>

<$inc(num, 10)>
<$print(num)>

<$inc(num)>
<$print(num)>
```
Output of code example is:
<br>`15`
<br>`16`
<br>


## `dec()`
Converts given hexadecimal number or string (if it's possible) to decimal format `dec(var_name)`, e.g.:
```bash
<$print(dec(0x400))>
<$print(dec("0x400"))>
```
Output of code example is:
<br>`1024`
<br>`1024`
<br>


## `hex()`
Converts given decimal number or string (if it's possible) to hexadecimal format `hex(var_name)`, e.g.:
```bash
<$print(hex(255))>
<$print(hex("255"))>
```
Output of code example is:
<br>`0xff`
<br>`0xff`
<br>


## `int()`
Converts given string (if it's possible) to integer `int(string_name)`, e.g.:
```bash
<$print(int("1000"))>
<$print(int("alphabet"))>
```
Output of code example is:
<br>`1000`
<br>`error: cannot convert "alphabet" to number`
<br>


## `str()`
Converts given integer to string and cuts it according to given borders `str(int_name, str_start, str_end)`, e.g.:
```bash
<$define(num, 100000)>
<$print(str(num, 0, 3))>
```
Output of code example is: `100`
<br>


## `macro()`
Creates a macro string according to given recipe and arguments `macro(recipe, arg1, arg2, ...)`, e.g.:
```bash
<$define(num1, 10)>
<$define(num2, 20)>

<$macro("$1 + $2 = $3", num1, num2, calc($(num1) + $(num2)))>
```
Output of code example is: `10 + 20 = 30`
<br>


## `macro_start()`
Defines a macro with a name given as function argument `macro_start(macro_name)` so that macro can be used further in the code (it can be called by using `macro()` function). The end of macro definition is marked by `macro_end` constant. Between `macro_start(macro_name)` and `macro_end` a recipe for macro has to be written, e.g.:
```bash
<$macro_start(day_enum)>
$1 has $2 days.
<$macro_end>

<$macro(day_enum, "Year", 365)>
```
Output of code example is: `Year has 365 days.`
<br>


## `foreach()`
Loops through a list from python dictionary, for each `item` in `list` execute specified `action` - `foreach(item, list, action)`, e.g.:
```bash
<$define(my_list, [{"name":"Year", "days":365}, {"name":"Week", "days":7}, {"name":"Month", "days":30}])>

<$foreach(item, my_list, macro("$1 has $2 days.\n", item.name, item.days))>
```
Output of code example is:
<br>`Year has 365 days.`
<br>`Week has 7 days.`
<br>`Month has 30 days.`
<br>


## `sort()` / `rsort()`
Arranges a list or dictionary into ascending or descending order (`sort() / rsort()`) according to one of its items `[r]sort(list, item)`, e.g.:
```bash
<$define(my_list, [{"name":"Year", "days":365}, {"name":"Week", "days":7}, {"name":"Month", "days":30}])>

// sort by "days"
<$foreach(item, sort(my_list, "days"), macro("$1 has $2 days.\n", item.name, item.days))>
// reverse sort by "days"
<$foreach(item, rsort(my_list, "days"), macro("$1 has $2 days.\n", item.name, item.days))>
```
Output of code example is:
<br>`Week has 7 days.`
<br>`Month has 30 days.`
<br>`Year has 365 days.`

<br>`Year has 365 days.`
<br>`Month has 30 days.`
<br>`Week has 7 days.`
<br>


## `table()`
Converts string tag to string value taken from a defined table - `table(table_name, string_tag)`, e.g.:
```bash
<$define(my_list, [{"name":"Year", "days":365}, {"name":"Week", "days":7}, {"name":"Month", "days":30}])>
// table definition
<$define(tbl_days,{"Week":"der Woche","Month":"der Monat","Year":"das Jahr"})>

<$foreach(item, my_list, macro("$1 is "$2" in German.\n", item.name, table(tbl_days, item.name)))>
```
Output of code example is:
<br>`Year is "das Jahr" in German.`
<br>`Week is "der Woche" in German.`
<br>`Month is "der Monat" in German.`
<br>


## `enable()` / `disable()`
Enables or supresses the output of code generator to file (it works similar like a compiler switch), e.g.:
```bash
<$define(str1, "Hello before disabling output")>
<$define(str2, "Hello after output is disabled")>
<$define(str3, "Hello after enabling output")>

<$macro("$1", str1)>
<$disable()>
<$macro("$(str2)")>
<$enable()>
<$macro("$(str3)")>
```
Output of code example is:
<br>`Hello before disabling output`
<br>`Hello after enabling output`
<br>


## `exec()`
Wraps and executes multiple functions or recipes (given as function arguments) inside itself - `exec(recipe1, recipe2, ...)`, e.g.:
```bash
<$define(num, 1000)>
<$ifdef(num, exec("DEFINED\n", macro("Number $1 is in hexadecimal format: $2", num, hex(num))))>
```
Output of code example is:
<br>`DEFINED`
<br>`Number 1000 is in hexadecimal format: 0x3e8`
<br>


## `include()`
This function will include custom python script with name of script as parameter and execute '__main__' function.
Example of such script with name "custom_script.py" is:
```python
from cg_tools import *

@register_function("custom_function")
def function_custom(self, args, unused):
    inStr = self.get_arg(args, 0, default="", type_check=str)
    print(inStr)
    return inStr

def __main__():
    print("This line will be printed in console")
    return "This line will be placed to output file"
```

In template file the script can be loaded and custom function can be executed:
```bash
<$include("custom_script")>
<$custom_function("This line will be printed to console and added to output file")>
```

Console output when processing template file will print following lines:
<br>`This line will be printed in console`
<br>`This line will be printed to console and added to output file`
<br>

Output file will contain following lines:
<br>`This line will be placed to output file`
<br>`This line will be printed to console and added to output file`
<br>

## `load_csv()`
This function will load CSV file and place data into dictionary.
Example csv file:
```csv
name, days
"Year", 365
"Week", 7
"Month", 30
```

Then in template file the csv can be loaded:
```bash
<$load_csv(day_table,"example.csv")>
// Loop trough entries:
<$foreach(entry, day_table, macro("$1 has $2 days\n", entry.name, entry.days))>
```

Output of code example is:
<br>`// Loop trough entries`
<br>`Year has 365 days`
<br>`Week has 7 days`
<br>`Month has 30 days`
<br>

## `camelCase()` / `CamelCase()` / `snake_case()` / `SNAKE_CASE()`
Case change functions are designed to change naming of variables, classes, functions, etc. to be compliant with given naming convention. All of those functions split input argument into array of strings and then combine all items back to string, applying desired case formatting.
```bash
<$snake_case("My_LEDString123")>
<$SNAKE_CASE("My_LEDString123")>
<$camelCase("My_LEDString123")>
<$CamelCase("My_LEDString123")>
```

Output of code example is:
<br>`my_led_string_123`
<br>`MY_LED_STRING_123`
<br>`myLedString123`
<br>`MyLedString123`
<br>

[Back to Code Generator homepage](../../../build/help/code_generator.md)
