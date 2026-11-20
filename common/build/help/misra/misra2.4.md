## Rule 2.4: A project should not contain unused tag declarations.
---

VIOLATION:
---

```c
typedef struct Books_tagname
{
    char title[50];
    char author[50];
    char subject[100];
    int book_id;
} Book;

int main()
{
    Book book;                // unused tag name
    strcpy( book.title, "C Programming");
    strcpy( book.author, "Nuha Ali");
    strcpy( book.subject, "C Programming Tutorial");
    book.book_id = 6495407;
    printf( "Book title : ", book.title);
}
```

SOLUTION:
---
```c
typedef struct Books_tagname
{
    char title[50];
    char author[50];
    char subject[100];
    int book_id;
} Book;

int main()
{
    Books_tagname book; /* Compliant */
    strcpy( book.title, "C Programming");
    strcpy( book.author, "Nuha Ali");
    strcpy( book.subject, "C Programming Tutorial");
    book.book_id = 6495407;
    printf( "Book title : ", book.title);
}
```

[Home page of Misra](./misra.md)