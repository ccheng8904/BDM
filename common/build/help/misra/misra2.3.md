## Rule 2.3: A project should not contain unused type declarations.
---

VIOLATION:
---

```c
int function (void)
{
    typedef int* INTPTR; // unused type declaration
    int* a, b;
}
```

SOLUTION:
---
```c
int function (void)
{
    typedef int* INTPTR;
    INTPTR a,b;		/* Compliant */
}
```

[Home page of Misra](./misra.md)
