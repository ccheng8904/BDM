## Rule 2.2: There shall be no dead code.
---

VIOLATION:
---

```c
int add (int a , int b)
{
    int result;
    result = a * b; // dead code
    return a + b;
}
```

SOLUTION:
---
```c
int add (int a , int b)
{
    int result;
    result = a + b;
    return result;
}
```

[Home page of Misra](./misra.md)
