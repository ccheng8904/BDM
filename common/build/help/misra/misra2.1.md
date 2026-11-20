## Rule 2.1: A project shall not contain unreachable code.
---

VIOLATION:
---

```c
int x=10;
while(x>5)
{
    continue;
    x++;   /*Non-compliant*/
}
```

SOLUTION:
---
```c
int x=10;
while(x>11)
{
    x++;
    continue; /*Compliant*/
}
```

[Home page of Misra](./misra.md)
