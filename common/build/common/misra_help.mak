#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PP PED FW
#*******************************************************************************
#  Description      Misra Rules help make file
#*******************************************************************************

Rules2.1_l1 = Rule 2.1: A project shall not contain unreachable code.
Rules2.1_l2 = VIOLATION:                     SOLUTION
Rules2.1_l3 = ******************************************************************************************************
Rules2.1_l4 = int x=10;                      int x=10;
Rules2.1_l5 = while(x>5)                     while(x>11)
Rules2.1_l6 = {                              {
Rules2.1_l7 =    continue;                      x++;
Rules2.1_l8 =    x++;   /*Non-compliant */      continue; /*Compliant*/
Rules2.1_l9 = }                          	  }
Rules2.1_lines = 1 2 3 4 5 6 7 8 9
Rules2.1_output = $(foreach line_num, $(Rules2.1_lines), $(info $(Rules2.1_l)$(line)))

Rules2.2 = "Rule 2.2: There shall be no dead code.\n
Rules2.2+= \n
Rules2.2+= VIOLATION:                                SOLUTION\n
Rules2.2+= ******************************************************************************************************\n
Rules2.2+= int add (int a , int b)               int add (int a , int b)               \n
Rules2.2+= {                                     {                                  \n
Rules2.2+=   int result;                            int result ;                 \n
Rules2.2+=   result = a * b; // dead code           result = a + b;            \n
Rules2.2+=   return a + b;                          return result;                      \n
Rules2.2+= }                                     }\n"


Rules2.3= "Rule 2.3: A project should not contain unused type declarations.\n
Rules2.3+= Violation:											Solution\n
Rules2.3+= ******************************************************************************************************************************\n
Rules2.3+=int function (void)									int function (void)  \n
Rules2.3+={											{							\n
Rules2.3+=	typedef int* INTPTR;// unused type declaration						typedef int* INTPTR; \n
Rules2.3+=      int* a, b;	     									INTPTR a,b;		/Compliant*/  \n 
Rules2.3+=}											} \n"


Rules2.4= "Rule 2.4: A project should not contain unused tag declarations.\n
Rules2.4+= Violation:													Solution\n
Rules2.4+=*************************************************************************************************************************************\n
Rules2.4+=typedef struct Books_tagname									typedef struct Books_tagname  \n												
Rules2.4+={																			{								\n
Rules2.4+=   char title[50];											char title[50];			\n
Rules2.4+=   char author[50];											char author[50];				\n
Rules2.4+=   char subject[100];											char subject[100];				\n
Rules2.4+=   int book_id;												int book_id;				\n
Rules2.4+=} Book;											} Book;					\n
Rules2.4+=int main()												int main()					\n
Rules2.4+={												{								\n
Rules2.4+=   Book book;                // unused tag name								Books tagname book; \*Compliant*/ \n 
Rules2.4+=   strcpy( book.title, \"C Programming\");									strcpy( book.title, \"C Programming\");\n
Rules2.4+=   strcpy( book.author, \"Nuha Ali\");									strcpy( book.author, \"Nuha Ali\"); \n
Rules2.4+=   strcpy( book.subject, \"C Programming Tutorial\");						   	strcpy( book.subject, \"C Programming Tutorial\");\n	
Rules2.4+=   book.book_id = 6495407;								    book.book_id = 6495407;     \n
Rules2.4+=   printf( \"Book title : \", book.title);								    printf( \"Book title : \", book.title);   \n
Rules2.4+=}												}\n"


Rules2.5= "Rule 2.5:A project should not contain unused macro declarations\n
Rules2.5+= Violation:														Solution\n
Rules2.5+=**************************************************************************************************************************************\n
Rules2.5+=\#define PI 3.14              // unused macro declaration 		\t\t\t	\#define PI 3.14					\n
Rules2.5+=float calc_area(float area)										float calc_area(float area)			\n
Rules2.5+={													{							\n
Rules2.5+=    float area;												   float area;				\n
Rules2.5+=    area = 3.14 * radius * radius;										  area = 3.14 * radius * radius;	\n	
Rules2.5+=    return area;												return area;	\n
Rules2.5+=}													}\n"


Rules2.6="Rule 2.6:A function should not contain unused label declarations\n
Rules2.6+=Violation:										Solution\n																											
Rules2.6+=************************************************************************************************************************\n
Rules2.6+=void f1 (int a) 									void f1 (int a) 			\n
Rules2.6+=  if (a <=0) 										if (a<=0)\n 
Rules2.6+=  {											{								\n
Rules2.6+=    	a++;										  	  a++;					\n		
Rules2.6+=  }								       }					\n
Rules2.6+=L1:                  /* Non-compliant */           					  if (a==0)			\n
Rules2.6+=  for (int i = 0; i < a; i++) 							        { goto L1;			\n
Rules2.6+=  {    											}	                    \n
Rules2.6+=  	 i=i+1;										       \t L1:  /* Compliant */ \n
Rules2.6+=	}											  for (int i=0; i<a; i++) \n
Rules2.6+=												  \t \t \t \t \t \t \t \t \t \t \t{ \n \t \t \t \t \t \t \t \t \t \t \t \t i=i+1;\n
Rules2.6+=
Rules2.6+=																																  \t \t \t \t \t \t \t \t \t \t \t} \n"


Rules2.7=" Rule 2.7: There should be no unused parameters in functions\n
Rules2.7+= Violation:																	Solution\n
Rules2.7+=****************************************************************************************************************************************************\n
Rules2.7+=int func(int a, int b)													\t	int func(int a, int b)\n
Rules2.7+={																	{							\n
Rules2.7+=    int c;																		int c;					\n
Rules2.7+=    c = a*a;                /*Non-Compliant*/													c=a*b;   	/* Compliant */ \n
Rules2.7+=    return c;														             	       return c;			\n
Rules2.7+=}																	}\n"					


Rules3.1=" Rule 3.1: The characters sequences /* and // shall not be used within a comment.\n
Rules3.1+=Violation:												Solution\n
Rules3.1+=*************************************************************************************************************************************************\n
Rules3.1+={													{\n
Rules3.1+=		int func(int a, int b)											  int func(int a, int b)\n
Rules3.1+=    int c;												 	int c;	\n
Rules3.1+=    /* 												       	// c is a result variable \n
Rules3.1+=    // c is a result variable  							  			/* Compliant */			\n
Rules3.1+=    */                          							  			c= a*b;\n
Rules3.1+=    c = a*b;               								  		return c;\n
Rules3.1+=    return c;												}\n
Rules3.1+=}	\n"																							


Rules3.2=" Rule 3.2: Line splicing shall not be used in // comments.\n
Rules3.2+=Violation:												Solution\n
Rules3.2+=***************************************************************************************************************************************\n
Rules3.2+=void function (void)										void function (void) \n
Rules3.2+={												{\n
Rules3.2+=	/* Non-compliant */										 /* Compliant */ \n
Rules3.2+=	int x = 15; // initialization \										int x=15; \\  \n
Rules3.2+=	if ( x>10 )												initialization \n
Rules3.2+=			   														\t\t\t\t\t\t\t\t\t\t\t\t\t if (x<10)\n
Rules3.2+=		x = x + 100; 									 			  x= x + 100;	  \n \t\t\t}
Rules3.2+= 					\t\t\t\t\t\t\t\t 	}\n"


Rules4.1=" Rule 4.1: Octal and hexadecimal escape sequences shall be terminated.\n
Rules4.1+=Violation:													Solution\n
Rules4.1+=*****************************************************************************************************************************************\n
Rules4.1+=void function()																												\t\t\\t\t\t\t\t\t\t\t\t\tvoid function()\n
Rules4.1+={												{\n
Rules4.1+=    char *c2 = \"\142n\";             /* Non-compliant */					\t\tchar *c2 = "\142\n";\n
Rules4.1+=																	  \t\t\t\t\t\t\t\t\t\t\t\t/* Terminated with another escape sequence. */\n
Rules4.1+=    char *s1 = "\x41g"; 		/* Non-compliant */							char *s1 = "\x\41\g";\n
Rules4.1+=}														/* Terminated with another escape sequence. */\n
Rules4.1+=																\t\t\t\t\t\t\t\t\t\t\t\t}/n"


Rules4.2= "Rule 4.2: Trigraphs should not be used.\n
Rules4.2+= Violation:																Solution\n
Rules4.2+=************************************************************************************************************************************************************************************************************\n
Rules4.2+={															{\n
Rules4.2+= const char str[]=\"(Date should be in the form ??-??-??)\"; /* Non-compliant*/ 							const char str[] = \"(Date should be in the form "?" "?-?" "?-?" "?")\";\n
Rules4.2+=																			      \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t// adjacent string literals concatenated at compile time\n
Rules4.2+= Evaluates to \"(Date should be in the form ~~]\"								  			const char str2[] = \"(Date should be in the form ?-?-?)\";\n
Rules4.2+=}															}\n"


Rules5.1= "Rule 5.1:External identifiers shall be distinct.\n
Rules5.1+= Violation:                                  \n
Rules5.1+= ****************************************************************************************************** \n
Rules5.1+= /*      123456789012345678901234567890123456789012345678901234567890123********* Characters */ \n
Rules5.1+= void function() \n
Rules5.1+= { \n
Rules5.1+= \t int32_t C99_______________________________________Variable_5_1_ext_non_a = 1; \n
Rules5.1+= \t int32_t C99_______________________________________Variable_5_1_ext_non_b = 1;   /* Non-compliant*/ \n
Rules5.1+= } \n

Rules5.1+= Solution:                                   \n
Rules5.1+= ****************************************************************************************************** \n
Rules5.1+= /*      123456789012345678901234567890123456789012345678901234567890123********* Characters */ \n
Rules5.1+= void function() \n
Rules5.1+= { \n
Rules5.1+= \t int32_t C99_____________________________________Variable_5_1_ext_non_a = 1; \n
Rules5.1+= \t int32_t C99_____________________________________Variable_5_1_ext_non_b = 1;   /* Non-compliant*/ \n
Rules5.1+= } \n"


Rules5.2= "Rule 5.2: Identifiers declared in the same scope and name space shall be distinct.\n
Rules5.2+= Violation:                                  \n
Rules5.2+= ****************************************************************************************************** \n
Rules5.2+= void function() \n
Rules5.2+= { \n
Rules5.2+= /*             123456789012345678901234567890123456789012345678901234567890123********* Characters */ \n
Rules5.2+= static int32_t C________________________________________Variable_5_2_stat_non_a; \n
Rules5.2+= static int32_t C________________________________________Variable_5_2_stat_non_b;  /* Non-compliant */ \n
Rules5.2+= } \n

Rules5.2+= Solution:                                  \n
Rules5.2+= ****************************************************************************************************** \n
Rules5.2+= void function() \n
Rules5.2+= { \n
Rules5.2+= /*             123456789012345678901234567890123456789012345678901234567890123********* Characters */ \n
Rules5.2+= static int32_t C______________________________________Variable_5_2_stat_non_a; \n
Rules5.2+= static int32_t C______________________________________Variable_5_2_stat_non_b;  /* Non-compliant */ \n
Rules5.2+= } \n"


Rules5.3= "Rule 5.3: An identifier declared in an inner scope shall not hide an identifier declared in an outer scope.\n
Rules5.3+= Violation:                                  Solution:\n
Rules5.3+= ******************************************************************************************************\n
Rules5.3+= void  MISRA5_3_func1(void )                 void  MISRA5_3_func1(void) \n
Rules5.3+= {                                           { \n
Rules5.3+= int16_t i , var;                             int16_t i , var; \n
Rules5.3+= {                                            { \n
Rules5.3+=  int16_t i; /* Non-compliant*/                int16_t j; /* compliant*/ \n
Rules5.3+=  i = 3;                                       j = 3; \n
Rules5.3+=  var += i;                                    var += j; \n
Rules5.3+=  }                                           } \n
Rules5.3+=  i = 1;                                      i = 1; \n
Rules5.3+=  var += i;                                   var += i; \n
Rules5.3+=}                                            } \n"
														

Rules5.4= "Rule 5.4: Macro identifiers shall be distinct.\n
Rules5.4+= Violation:                                  \n
Rules5.4+= ******************************************************************************************************\n
Rules5.4+= /*      123456789012345678901234567890123456789012345678901234567890123********* Characters */ \n
Rules5.4+= \#define C99________________________________________Macro_5_4_block_non_a 2 \n
Rules5.4+= \#define C99________________________________________Macro_5_4_block_non_b	3 /* Non-compliant */ \n

Rules5.4+= Solution:                                  \n
Rules5.4+= ******************************************************************************************************\n
Rules5.4+= /*      123456789012345678901234567890123456789012345678901234567890123********* Characters */ \n
Rules5.4+= define C99____________________________________Macro_5_4_block_non_a 2 \n
Rules5.4+= define C99____________________________________Macro_5_4_block_non_b	3 /* Compliant */ \n"


Rules5.5= "Rule 5.5: Identifiers shall be distinct from macro names.\n
Rules5.5+= Violation:                                  Solution:\n
Rules5.5+= ******************************************************************************************************\n
Rules5.5+= define Sum(x, y) ( ( x ) + ( y ) )          define Sum_macro(x, y) ( ( x ) + ( y ) ) \n
Rules5.5+= void MISRA5_5_func1(void)                   void MISRA5_5_func1(void) \n
Rules5.5+= {                                           { \n
Rules5.5+= int16_t Sum = 2; /* Non-compliant */         int16_t Sum = 2; /*compliant */  \n
Rules5.5+= }                                           } \n"


Rules5.6="Rule 5.6: A typedef name shall be a unique identifier.\n
Rules5.6+=Violation:                                     Solution:\n
Rules5.6+= ******************************************************************************************************\n
Rules5.6+= typedef int32_t mass;                          typedef int32_t mass;\n
Rules5.6+= void MISRA5_6_func1 (void )                    void MISRA5_6_func1 (void ) \n
Rules5.6+= {                                              { \n
Rules5.6+=  int32_t mass=1; /*Non-compliant */             mass var =1; /*Compliant */ \n
Rules5.6+=  int32_t var;                                   var = var+1; \n
Rules5.6+=  var += mass;                                  } \n
Rules5.6+= }\n"


Rules5.7="Rule 5.7:A tag name shall be a unique identifier.\n
Rules5.7+= Violation:                                     Solution:\n
Rules5.7+= ******************************************************************************************************\n
Rules5.7+= static void MISRA5_7_func1(void)              static void MISRA5_7_func1(void) \n
Rules5.7+= {                                             { \n
Rules5.7+=  struct stag                                   struct stag    \n
Rules5.7+=  {                                             { \n
Rules5.7+= 	uint16_t staga;                                uint16_t staga; \n
Rules5.7+= 	uint16_t stagb;                                uint16_t stagb; \n
Rules5.7+=  };                                            }; \n

Rules5.7+= struct stag a1={0,0};                           union deer \n
Rules5.7+= union stag a2={0,0};/*Non-compliant*/           { \n
Rules5.7+= }                                                uint16_t deer1; \n
Rules5.7+=                          				       \t\t\t\t\t\t uint16_t deer2; \n
Rules5.7+=                          						\t\t\t\t\t\t } \n
Rules5.7+=                              					\t\t\t\t\t\t struct stag a1={0,0}; \n
Rules5.7+=                      							\t\t\t\t\t\t union deer a2={0,0};/*compliant*/ \n
Rules5.7+=                       							\t\t\t\t\t\t}    \n"


Rules5.8="Rule 5.8:Identifiers that define objects or functions with external linkage shall be unique\n
Rules5.8+= Violation:                                     Solution:\n
Rules5.8+= ******************************************************************************************************\n
Rules5.8+= /* file1.c */                                  /* file1.c */ \n
Rules5.8+= int32_t count;                                 int32_t count; \n
Rules5.8+= void foo(void)                                 void foo(void) \n
Rules5.8+= {                                              { \n
Rules5.8+=  \t int16_t index;                               int16_t index; \n
Rules5.8+= }                                              } \n
Rules5.8+= /*file2.c*/                                    /*file2.c*/ \n
Rules5.8+= static foo(void)    /*Non-compliant*/          static boo(void)      /*compliant*/ \n
Rules5.8+= {                                              { \n
Rules5.8+=  \t   int16_t count;  /*Non-compliant*/              int16_t varcount; /*compliant*/ \n
Rules5.8+=  \t   int32_t index;                                 int32_t index; \n
Rules5.8+= }                                              } \n"


Rules5.9="Rule 5.9:Identifiers that define objects or functions with internal linkage should be unique.\n
Rules5.9+= Violation:                                     Solution:\n
Rules5.9+= ******************************************************************************************************\n
Rules5.9+= static int32_t var1 = 7;                       static int32_t var1 = 7; \n
Rules5.9+= void MISRA5_9_func1 ( void )                   void MISRA5_9_func1 ( void ) \n
Rules5.9+= {                                              { \n
Rules5.9+= 	int16_t var1= 3;                                int16_t var1= 3; \n
Rules5.9+=  int16_t var2= 17;                               int16_t var2= 17; \n
Rules5.9+=  var1 = var1 + var2;                             var1 = var1 + var2; \n
Rules5.9+= }                                              }\n"


Rules6.1="Rule 6.1:Bit-fields shall only be declared with an appropriate type\n
Rules6.1+= Violation:                                                 Solution: \n
Rules6.1+= ******************************************************************************************************\n
Rules6.1+= typedef unsigned int UINT_16;                             typedef unsigned int UINT_16; \n
Rules6.1+= struct MISRA_6_1_struct                                   struct MISRA_6_1_struct \n
Rules6.1+= {                                                         { \n
Rules6.1+=  int  b1:2;  /* Non-compliant */                            unsigned int b1:2;  /* Compliant */ \n
Rules6.1+=  signed long b2:2;  /* Non-compliant  */                    UINT_16 b2:2  ;  /* Compliant */  \n
Rules6.1+=  unsigned long b3:2;  /* Non-compliant  */                 }; \n
Rules6.1+= }; \n"


Rules6.2="Rule 6.2:Single-bit named bit fields shall not be of a signed type\n
Rules6.2+= Violation:                                                 Solution: \n
Rules6.2+= ******************************************************************************************************\n
Rules6.2+= typedef unsigned int UINT_16;                             typedef unsigned int UINT_16; \n
Rules6.2+= struct MISRA_6_2_struct                                   struct MISRA_6_2_struct \n
Rules6.2+= {                                                         { \n
Rules6.2+= int  b1:1;  /* Non-compliant */                            unsigned int b1:1;  /* Compliant */ \n
Rules6.2+= signed long b2:1;  /* Non-compliant  */                    UINT_16 b2:1;  /* Compliant */  \n
Rules6.2+= unsigned long b3:1;  /* Non-compliant  */                 }; \n
Rules6.2+= }; \n"


Rules7.1="Rule 7.1: Octal constants shall not be used.\n
Rules7.1+= Violation:                                                 Solution: \n
Rules7.1+= ******************************************************************************************************\n
Rules7.1+= static void MISRA7_1_func1 (void)                          static void MISRA7_1_func1 (void) \n
Rules7.1+= {                                                          { \n
Rules7.1+= uint16_t MISRA_7_1_code[ 10 ]={0};                          uint16_t MISRA_7_1_code[ 10 ]={0}; \n
Rules7.1+= MISRA_7_1_code[3] = 052; /* Non-Compliant */                MISRA_7_1_code[1]=109; /* Compliant */ \n
Rules7.1+= MISRA_7_1_code[4] = 071; /* Non-Compliant */                MISRA_7_1_code[2]=100; /* Compliant */ \n
Rules7.1+= }                                                          }\n"


Rules7.2="Rule 7.2:A “u” or “U” suffix shall be applied to all integer constants that are represented in an unsigned type.\n
Rules7.2+= Violation:                                                      Solution: \n
Rules7.2+= ******************************************************************************************************\n
Rules7.2+= static void MISRA7_2_func1 (void)                               static void MISRA7_2_func1 (void) \n
Rules7.2+= {                                                               { \n
Rules7.2+= uint16_t  MISRA_7_2_constant1 = 0x8000;   /*Non-Compliant*/     uint16_t  MISRA_7_2_constant1 = 0x8000u;   /* Compliant */ \n
Rules7.2+= uint16_t  MISRA_7_2_constant2 = 0x8001;  /*Non-Compliant */     uint16_t  MISRA_7_2_constant2 = 0x8001u;  /* Compliant */ \n
Rules7.2+=}                                                               } \n"


Rules7.3="Rule 7.3:The lowercase character “l” shall not be used in a literal suffix.\n
Rules7.3+= Violation:                                                      Solution: \n
Rules7.3+= ******************************************************************************************************\n
Rules7.3+= static void MISRA7_3_func1 (void)                               static void MISRA7_3_func1 (void) \n
Rules7.3+= {                                                               { \n
Rules7.3+= const int64_t MISRA_7_3_a = 0l;  /* Non-compliant */             const int64_t MISRA_7_3_a = 0L;  /* Compliant */ \n
Rules7.3+= const uint64_t MISRA_7_3_b = 0lU; /* Non-compliant */            const uint64_t MISRA_7_3_b = 0LU; /*Compliant */ \n
Rules7.3+= }                                                               } \n"


Rules7.4="Rule 7.4:A string literal shall not be assigned to an object unless the object's type is pointer to const-qualified char.\n
Rules7.4+= Violation:                                                      Solution: \n
Rules7.4+= ******************************************************************************************************\n
Rules7.4+= static void MISRA7_4_func1 (void)                               static void MISRA7_4_func1 (void) \n
Rules7.4+= {                                                               { \n
Rules7.4+= 	char *MISRA_7_4_s = "string"; /* Non-compliant */            	   const volatile char *MISRA_7_4_s = "string"; /* Compliant */ \n
Rules7.4+= }                                                               } \n"


Rules8.1= "Rule 8.1: Types shall be explicitly specified.  \n
Rules8.1+=  \n
Rules8.1+= Violation:                                                      Solution: \n
Rules8.1+= ******************************************************************************************************\n
Rules8.1+= 	  static x1 = 1;   /* Non-compliant */                            static int16_t x1 = 1 /* Compliant */ \n  
Rules8.1+= static void MISRA8_1_func1(void)                                static void MISRA8_1_func1(void)  \n   
Rules8.1+= {                                                               { \n
Rules8.1+= 	\t const y1 = 1;   /* Non-compliant */                             const int16_t y1 = 1;  /* compliant */ \n    }                            
Rules8.1+=                                       \t\t\t\t    }  \n"


Rules8.2= "Rule 8.2: Function types shall be in prototype form with named parameters.  \n
Rules8.2+=  \n
Rules8.2+= Violation:                                                      Solution: \n
Rules8.2+= ********************************************************************************************************************************\n
Rules8.2+= static void MISRA8_2_fun2(int16_t); /* Non-compliant */         static void MISRA8_2_fun2(int16_t m);   /*compliant*/ \n  
Rules8.2+= static void MISRA8_2_fun2(int16_t m)                            static void MISRA8_2_fun2(int16_t m)  \n   
Rules8.2+= {                                                               { \n
Rules8.2+=  \tMISRA8_int16_volatile_helper = m;                             MISRA8_int16_volatile_helper = m; \n     }                           
Rules8.2+=                                                               \t\t\t\t    } \n"


Rules8.3= "Rule 8.3: All declarations of an object or function shall use the same names and type qualifiers.\n
Rules8.3+=  \n
Rules8.3+= Violation:                                                      Solution: \n
Rules8.3+= ***********************************************************************************************************************************\n
Rules8.3+= extern void MISRA8_3_fun2( int *const j);                       extern void MISRA8_3_fun2( int *const j);  \n  
Rules8.3+= void MISRA8_3_fun2 ( int * j ) /* Non-compliant */              void MISRA8_3_fun2(int *const j)  /*compliant*/  \n   
Rules8.3+= {                                                               { \n
Rules8.3+=  MISRA8_int32_volatile_helper = *j;                                MISRA8_int32_volatile_helper = *j; \n   }                               
Rules8.3+=                                                              \t\t\t\t   } \n"


Rules8.4= "Rule 8.4: A compatible declaration shall be visible when an object or function with external linkage is defined. \n
Rules8.4+=  \n
Rules8.4+=Violation:                                                      Solution: \n
Rules8.4+=*********************************************************************************************************************************\n
Rules8.4+=extern uint16_t MISRA_8_4_speed = 6000u; /* Non-compliant */    extern uint16_t MISRA_8_4_speed;  \n  
Rules8.4+=                                                               \t\t\t\t\t\t\t\tuint16_t MISRA_8_4_speed = 6000u; /*compliant*/  \n "


Rules8.5= "Rule 8.5: An external object or function shall be declared once in one and only one file. \n
Rules8.5+= \n
Rules8.5+= Violation:                                                      Solution: \n
Rules8.5+= ****************************************************************************************************************************\n
Rules8.5+= /* Misra8.h */                                                  /* Misra8.h */ \n  
Rules8.5+= extern int16_t MISRA_8_5_a; /* Non-compliant */                 extern int16_t MISRA_8_5_b;	 \n   
Rules8.5+= extern int16_t MISRA_8_5_b;                                                                     \n
Rules8.5+= /* Misra8b.h */                                                /* Misra8b.h */                 \n   extern int16_t MISRA_8_5_a;                              
Rules8.5+=                                     \t  extern int16_t MISRA_8_5_a; /* compliant */ \n
Rules8.5+= /* Misra8b.c */                                                 /* Misra8b.c */ \n
Rules8.5+= int16_t MISRA_8_5_a = 1;                                        int16_t MISRA_8_5_a = 1; \n"


Rules8.6= "Rule 8.6: An identifier with external linkage shall have exactly one external definition. \n
Rules8.6+= \n
Rules8.6+= Violation:                                                      Solution: \n
Rules8.6+= *********************************************************************************************************************\n
Rules8.6+= /* Misra8.h */                                                  /* Misra8.h */ \n
Rules8.6+= extern int16_t MISRA_8_6_b;                                     extern int16_t MISRA_8_6_b;  \n   
Rules8.6+= /* Misra8.c */                                                  /* Misra8.c */ 		 \n  int16_t MISRA_8_6_b = 50;                       
Rules8.6+=					           		\t\tint16_t MISRA_8_6_b = 50;  /* compliant */   \n
Rules8.6+= /*Misra8b.c */                                                  /*Misra8b.c */                                \n
Rules8.6+= int16_t MISRA_8_6_b = 80; /* Non-compliant */                                                                \n"


Rules8.7+= "Rule 8.7: Functions and objects should not be defined with external linkage if they are referenced in only one translation unit. \n
Rules8.7+= \n
Rules8.7+= Violation:                                                      Solution: \n
Rules8.7+= ******************************************************************************************************\n
Rules8.7+= /*Misra8.c */                                                   /*Misra8.c */\n
Rules8.7+= int16_t MISRA_8_7_a = 1;                                        int16_t MISRA_8_7_a = 1;\n
Rules8.7+= void MISRA8_7_func1(void)                                       void MISRA8_7_func1(void)\n    {                                 
Rules8.7+=                        	  											\t\t\t\t{\n
Rules8.7+=  MISRA8_int16_volatile_helper += MISRA_8_7_a;                    MISRA8_int16_volatile_helper += MISRA_8_7_a;\n
Rules8.7+= }                                                               }\n
Rules8.7+= /*Misra8.h*/                                                    /*Misra8.h*/\n
Rules8.7+= extern void MISRA8_7_func1 (void);                              void MISRA8_7_func1 (void);\n
Rules8.7+= extern int16_t MISRA_8_7_a; /* Non Complaint */                 int16_t MISRA_8_7_a; /* Complaint */  \n"


Rules8.8+= "Rule 8.8: The static storage class specifier shall be used in all declarations of objects and functions that have internal linkage. \n
Rules8.8+= \n
Rules8.8+= Violation:                                                      Solution: \n
Rules8.8+= ***********************************************************************************************************************************\n
Rules8.8+=  static void MISRA8_8_func1 ( void );                            static void MISRA8_8_func1 ( void );  \n  
Rules8.8+= void MISRA8_8_func1 ( void ) 	/* Non-compliant */          static void MISRA8_8_func1( void ) /* compliant */  \n   
Rules8.8+= {                                                               { \n
Rules8.8+= 		 MISRA8_int32_volatile_helper +=  MISRA_8_8_x;                     MISRA8_int32_volatile_helper +=  MISRA_8_8_x;  \n    }                         
Rules8.8+=                                      																	\t\t\t\t\t   }"


Rules8.9= "Rule 8.9: An object should be defined at block scope if its identifier only appears in a single function. \n
Rules8.9+= \n
Rules8.9+= Violation:                                                      Solution: \n
Rules8.9+= **********************************************************************************************************************************\n
Rules8.9+= int32_t l;   /* Non-compliant */                                static void MISRA8_1_func1(void)     \n  
Rules8.9+= static void MISRA8_9_func3 ( void )                             int32_t l;  /* compliant */          \n   
Rules8.9+= {                                                               { \n
Rules8.9+=   for ( l = 0; l < 3; ++l )                                       for ( l = 0; l < 3; ++l ) \n
Rules8.9+=   {                                                                 { \n
Rules8.9+=     MISRA8_uint16_volatile_helper +=  2u;                               MISRA8_uint16_volatile_helper +=  2u;    \n    }                             
Rules8.9+=                                                                 \t\t\t\t   } \n
Rules8.9+= }                                                               } \n"


Rules8.10= "Rule 8.10: An inline function shall be declared with the static storage class. \n
Rules8.10+= \n
Rules8.10+= Violation:                                                      Solution: \n
Rules8.10+= *********************************************************************************************************************************\n
Rules8.10+= extern inline void MISRA8_10_func(void);                        extern inline void MISRA8_10_func(void);\n  
Rules8.10+= inline void MISRA8_10_func1 (void)  /*Non-compliant*/           static inline void MISRA8_10_func1 (void)  /*compliant*/ \n   
Rules8.10+= {                                                               { \n
Rules8.10+=  MISRA8_uint16_volatile_helper +=  2u;                             MISRA8_uint16_volatile_helper +=  2u; \n    }                            
Rules8.10+=                                                               \t\t\t\t   }\n"


Rules8.11= "Rule 8.11: When an array with external linkage is declared, its size should be explicitly specified. \n
Rules8.11+= \n
Rules8.11+= Violation:                                                      Solution: \n
Rules8.11+= ******************************************************************************************************\n
Rules8.11+= int32_t array2[ ];  /* Non-compliant */                         int32_t array2[20]; /* Compliant */ \n " 


Rules8.12= "Rule 8.12: Within an enumerator list, the value of an implicitly-specified enumeration constant shall be unique.\n
Rules8.12+= \n
Rules8.12+= Violation:                                                      Solution: \n
Rules8.12+= ******************************************************************************************************\n
Rules8.12+= enum colour1{ red = 3, blue, green, yellow = 5 };               enum colour1{ red = 3, blue, green = 5, yellow = 5 }; \n		
Rules8.12+= /* Non-compliant */                                             /* compliant */   \n "                    


Rules8.13= "Rule 8.13: A pointer should point to a const-qualified type whenever possible. \n
Rules8.13+= \n
Rules8.13+= Violation:                                                      Solution: \n
Rules8.13+= *******************************************************************************************************************************\n
Rules8.13+= static int16_t MISRA8_13_func1( int16_t *p )                    static int16_t MISRA8_13_func1( const int16_t *p ) \n
Rules8.13+= /* Non-compliant */                                             /* compliant */      \n         
Rules8.13+= {                                                               { \n
Rules8.13+=   \t\treturn *p;                                                      return *p; \n          }                      
Rules8.13+=                                                                \t\t\t\t\t  }  \n"


Rules8.14= "Rule 8.14: The restrict type qualifier shall not be used. \n
Rules8.14+= \n
Rules8.14+= Violation:                                                                                Solution: \n
Rules8.14+= *********************************************************************************************************************************************\n
Rules8.14+= static void MISRA8_14_func2(void * restrict p, void const * restrict q, uint16_t n )      static void MISRA8_14_func2(void *p, void const *q, uint16_t n ) \n  
Rules8.14+= /* Non-compliant */                                                                        /*compliant */  \n
Rules8.14+= {                                                                                            { \n
Rules8.14+=   memcpy ( p, q, n );                                                                          memcpy ( p, q, n );  /* compliant */ \n }                                
Rules8.14+=                                                                                            \t\t\t\t\t\t\t      } \n"


Rules9.1= "Rule 9.1: The value of an object with automatic storage duration shall not be read before it has been set. \n
Rules9.1+= \n
Rules9.1+= Violation:                                                      Solution: \n
Rules9.1+= **********************************************************************************************************************\n
Rules9.1+= static void MISRA9_1_func2 ( void )                             static void MISRA9_1_func2 ( void ) \n
Rules9.1+= uint16_t u;                                                     uint16_t u = 5u;     \n  if ( u == 3U )             
Rules9.1+=                                                  \t\t\t\t\t if ( u == 3U )             \n
Rules9.1+= {                                                               {            \n
Rules9.1+=  /* Non-compliant */                                               /* compliant */ \n  }                               
Rules9.1+=                                                              \t\t\t\t }  \n"


Rules9.2= "Rule 9.2: The initializer for an aggregate or union shall be enclosed in braces. \n
Rules9.2+= \n
Rules9.2+= Violation:                                                      				Solution: \n
Rules9.2+= *****************************************************************************************************************************************\n
Rules9.2+= static void MISRA9_2_func1 (void)                             				static void MISRA9_1_func2 ( void ) \n
Rules9.2+= {                                                            			{                                   \n              
Rules9.2+=   int16_t MISRA9_2_y1[3][2]={1,2,0,0,5,6};                 		 int16_t MISRA9_2_y1[3][2]={{1,2},{0},{5,6}}; \n
Rules9.2+=   /* Non-compliant */                                       		\t\t/* compliant */  \n                                                                                     
Rules9.2+= MISRA9_int16_volatile_helper = MISRA9_2_y1[1][1];  \n	MISRA9_int16_volatile_helper = MISRA9_2_y1[1][1];  \n
Rules9.2+= }                                                            			 } \n"


Rules9.3= "Rule 9.3: Arrays shall not be partially initialized. \n
Rules9.3+= \n
Rules9.3+= Violation:                                                      Solution: \n
Rules9.3+= ********************************************************************************************************************************************\n
Rules9.3+= static void MISRA9_3_func1 (void)                             static void MISRA9_3_func1 (void) \n
Rules9.3+= {                                                             {                                   \n     int16_t y[ 3 ] = { 0, 1 }; /* Non-compliant */  
Rules9.3+= 												   \t\t  int16_t y[ 3 ] = { 0, 1, 2 };	/*compliant */ \n   }       \t\t\t\t\t\t\t} \n"                                                                                                                 
Rules9.3+= 


Rules9.4= "Rule 9.4: An element of an object shall not be initialized more than once. \n
Rules9.4+= \n
Rules9.4+= Violation:                                                            Solution: \n
Rules9.4+= **********************************************************************************************************************************************\n
Rules9.4+= static void MISRA9_4_func1 (void)                                     static void MISRA9_4_func1 (void) \n
Rules9.4+= {                                                                     {   \n   
Rules9.4+= int16_t a3[5] = {[0]=-5, [1]=-4, [2]=-3, [2]=-2, [3]=-2, [4]=1 ;\n      \t\t\t\t\t\t\t\t\t int16_t a3[5] = {[0]=-5, [1]=-4, [2]=-3, [3]=-2, [4]=1 ;  \n     /* Non-compliant */                                                                              
Rules9.4+=    	/* compliant */  \n
Rules9.4+= MISRA9_int16_volatile_helper = a3[3];                                  MISRA9_int16_volatile_helper = a3[3]; \n
Rules9.4+= }                                                                     } \n"


Rules9.5= "Rule 9.5: Where designated initializers are used to initialize an array object the size of the array shall be specified explicitly. \n
Rules9.5+= \n
Rules9.5+= Violation:                                                            Solution: \n
Rules9.5+= **********************************************************************************************************************************************\n
Rules9.5+= static void MISRA9_5_func1 (void)                                     static void MISRA9_5_func1 (void) \n
Rules9.5+= {                                                                     {                                   \n int16_t a1[ ] = { [ 0 ] = 1 };/* Non-compliant */          
Rules9.5+= 											        	\t\tint16_t a1[1] = { [ 0 ] = 1 };  /* compliant */  \n MISRA9_int16_volatile_helper = a1[0];    			 	MISRA9_int16_volatile_helper = a1[0]; \n  }    \t\t\t\t\t\t\t\t\t}                                                                                                                     
Rules9.5+=   \n"


Rules10.1= "Rule 10.1: Operands shall not be of an inappropriate essential type. \n
Rules10.1+= \n
Rules10.1+= Violation:                                                            Solution: \n
Rules10.1+= **********************************************************************************************************************************************\n
Rules10.1+= static void MISRA10_1_func1 ( _Bool bla )                             static void MISRA10_1_func1 ( _Bool bla )\n
Rules10.1+= {                                                                     {    \n
Rules10.1+= /*Rationale 1  - non compliant examples*/                                 /* Compliant Examples*/   \n float32_t f32a = 1.0f, f32b = 1.0f, f32c;             
Rules10.1+=                              \t\t\t float32_t f32a = 1.0f, f32b = 1.0f, f32c; \n f32c  = f32a & 2U;   \t\t\t\t\t\t\t\t f32c = f32a + f32b;  f32c = f32a << 2;                                                                                                                        
Rules10.1+=                                                     \t\t\t\t\t\t\t  _Bool blb = f32a > 0.0;    \n  }                                            
Rules10.1+=                       \t\t\t}  \n"


Rules10.2= "Rule 10.2: Expressions of essentially character type shall not be used inappropriately in addition and subtraction operations. \n
Rules10.2+= \n
Rules10.2+= Violation:                                                            Solution: \n
Rules10.2+= **********************************************************************************************************************************************\n
Rules10.2+= static void MISRA10_2_func1(void)	                                  static void MISRA10_2_func1(void) \n
Rules10.2+= {                                                                     {                                         \n enum enuma { a1 } ena = a1;             
Rules10.2+=  	                	\t\t\t\tenum enuma { a1 } ena = a1;                                            \n  char cha = '0';   \t\t\t\t\t\t\tchar cha = '0';                                                                                                                  
Rules10.2+=     \n
Rules10.2+= int32_t s32a = 1;                                                      int8_t s8a = 1;    \n
Rules10.2+= s32a = cha + ':'; 	/*Non Complaint*/                                  s8a = (int8_t) (cha - '0');  /*Complaint*/   \n  s32a = cha - ena;   /*Non Complaint*/   \t\t\t\t cha = '0' - s8a;   /*Complaint*/                                              
Rules10.2+=     \n 
Rules10.2+=}                                                                      }\n "


Rules10.3= "Rule 10.3: The value of an expression shall not be assigned to an object with a narrower essential type or of a different essential type category. \n
Rules10.3+= \n
Rules10.3+= Violation:                                                            Solution: \n
Rules10.3+= **********************************************************************************************************************************************\n
Rules10.3+= static void MISRA10_3_func1 (void)                                    static void MISRA10_3_func1 (void) \n   {     
Rules10.3+=                                                                    \t\t\t\t\t\t\t\t {                                \n      
Rules10.3+= _Bool bla = 0;    /*Non Complaint*/                                     _Bool bla   = (bool) 0;  /*Complaint*/  \n     \tuint8_t u8a = 1;                                                                                                                                  
Rules10.3+=   \n                                                      \t\t\t\tuint8_t u8a = 1;\n          u8a = 'a';                                             
Rules10.3+=                                                             \t\tu8b = 2;   /*Complaint*/      \n
Rules10.3+= }                                                                     }     \n"               


Rules10.4= "Rule 10.4: Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type category. \n
Rules10.4+= \n
Rules10.4+= Violation:                                                            Solution: \n
Rules10.4+= **********************************************************************************************************************************************\n
Rules10.4+= static void MISRA10_4_func1(void)                                     static void MISRA10_4_func1(void) \n
Rules10.4+= {                                                                     {            \n
Rules10.4+=  int8_t s8a = 1;                                                        int8_t s8a = 1;           \n char cha = 'a';\n  \t\t\t\t\t\t\t\t\tchar cha = 'a';                                                                                                                      
Rules10.4+=                                  \n uint8_t u8a = 1;                              
Rules10.4+=                                                        \t\t\t uint8_t u8a = 1;    \n s8a += u8a;   /*Non Complaint*/            
Rules10.4+=                                        \t\t\t\t\t cha += s8a;  /*Complaint*/         \n
Rules10.4+=  u8a += cha;   /*Non Complaint*/                                        cha -= u8a; /*Complaint*/      \n
Rules10.4+= }                                                                     }  \n"


Rules10.5="Rule 10.5: The value of an expression should not be cast to an inappropriate essential type.\n
Rules10.5+=Violation:												Solution\n
Rules10.5+= **********************************************************************************************************************************************\n
Rules10.5+=static void MISRA10_5_func1(void)\t\t\t\t\t\t\t\tstatic void MISRA10_5_func1(void) \n 
Rules10.5+={\t\t\t\t\t\t\t\t\t\t\t\t{\n
Rules10.5+=int 8_t s8a= 1;										int8_t s8a = 1;\n
Rules10.5+=char chb = 'a';										char chb = 'a';\n
Rules10.5+=s8a=(int8_t) false; /*Non-Compliant*/							\ts8a = (int8_t) 2U; /*Compliant*/\n
Rules10.5+=chb=(char) false;  /*Non-Compliant*/								chb= (char) enb; /*Compliant*/ \n}		
Rules10.5+=											\t\t\t\t\t\t\t\t\t\t}

Rules10.6="Rule 10.6: Expressions of essentially character type shall not be used inappropriately in addition and subtraction operations.\n
Rules10.6+=Violation:	\n******************************************************************************************************\n
Rules10.6+=static void MISRA10_6_func1 (void)		\n
Rules10.6+={	\n int16_t s16a=1, s16b = 1;\nu32a = u16a + u16b; /*Non-Compliant*/	\n int32_t s32a = s16a +s16b; /* Non-Compliant*/	\n}																																			 																																														
Rules10.6+=\n
Rules10.6+= Solution\n 
Rules10.6+=************************************************************************************************************\n
Rules10.6+= static void MISRA10_6_func1 (void) \n
Rules10.6+= { \n
Rules10.6+= int16_t u16a = 1, u16b = 1 , u16c; \n
Rules10.6+= int16_t s16a = 1, s16b = 1; \n
Rules10.6+= u16c = u16a + u16b; \n
Rules10.6+= int32_t s16a = s16a + s16b; \n
Rules10.6+=}\n"

Rules10.7= "Rule 10.7: If a composite expression is used as one operand of an operator in which the usual arithmetic conversions are performed then the other operand shall not have wider essential type. \n
Rules10.7+= \n
Rules10.7+= Violation: \n **********************************************************************************************************************************************                                 
Rules10.7+= \n
Rules10.7+= static void MISRA10_7_func1 (void)                                        \n
Rules10.7+= {                                                                         \n
Rules10.7+=  uint16_t u16a = 1, u16b = 1, u16c = 1;                                    \n  uint32_t u32a = 1, u32b = 1, u32c = 1;                                                                                                                              
Rules10.7+=  \t\t\t\t\t\t\t\t\t\t                                    \n  u32a = (u16a + u16b) + u32c;  /*Non Complaint*/                                           
Rules10.7+=             \n u32b = u32a * ( u16a + u16b ); /*Non Complaint*/                              
Rules10.7+=           \n } 
Rules10.7+= \n                                                                     

Rules10.7+=\nSolution
Rules10.7+= \n
Rules10.7+=*********************************************************************************************************************************************\n
Rules10.7+=static void MISRA10_7_func1 (void)\n
Rules10.7+={\n
Rules10.7+= uint16_t u16a = 1, u16b = 1, u16c = 1;\n
Rules10.7+= uint32_t u32a = 1, u32b = 1, u32c = 1;\n
Rules10.7+= u32a = u16a + u16b + u32c; /* Compliant*/\n
Rules10.7+= u32b = u32a * u16a + u16b; /* Compliant*/\n
Rules10.7+=}\n"


Rules10.8= "Rule 10.8: The value of a composite expression shall not be cast to a different essential type category or a wider essential type. \n
Rules10.8+= \n
Rules10.8+= Violation:                                                            Solution: \n
Rules10.8+= **********************************************************************************************************************************************\n
Rules10.8+= static void MISRA10_8_func1 (void)                                    static void MISRA10_8_func1 (void)      \n
Rules10.8+= {                                                                     {                                 \n    
Rules10.8+=  uint16_t u16a =1, u16b = 1;                                           uint16_t u16a =1, u16b = 1;         \n     uint32_t u32a = 1, u32b = 1;                                                                                                                          
Rules10.8+=                                            \t\t\t\t\t\t\t\tuint32_t u32a = 1, u32b = 1;         \n   int32_t s32a = 1, s32b = 1;                                                
Rules10.8+=                               int32_t s32a = 1, s32b = 1;          \n   u16a = ( uint16_t ) ( s32a + s32b );                 
Rules10.8+=         	\t\t u16a = ( uint16_t ) ( u32a + u32b );  \n
Rules10.8+=   /* Non -compliant - different essential type category */             \t\t/* Complaint */                          \n  u32a = ( uint32_t ) ( u16a + u16b );         
Rules10.8+=                              \t\t         u32a = ( uint16_t ) ( u16a + u16b );   \n
Rules10.8+=  /* Non-compliant - cast to wider essential type */                    \t\t/* Compliant */    \n  }                                                                   
Rules10.8+=                                                                      } \n"


Rules11.1= "Rule 11.1: Conversions shall not be performed between a pointer to a function and any other type. \n
Rules11.1+= \n
Rules11.1+= Exception: \n
Rules11.1+= 1.	A null pointer constant may be converted into a pointer to function. \n
Rules11.1+= 2.	A pointer to a function may be converted into void.  \n
Rules11.1+= 3.	A function type may be implicitly converted into a pointer to that function type. \n
Rules11.1+= Violation:                                                                  Solution: \n
Rules11.1+= **********************************************************************************************************************************\n
Rules11.1+= typedef void ( *fp16 ) ( int16_t n );                                       typedef void ( *fp16 ) ( int16_t n );             \n
Rules11.1+= typedef void ( *fp32 ) ( int32_t n );                                       typedef fp16 ( *pfp16 ) ( void );                 \n
Rules11.1+= typedef fp16 ( *pfp16 ) ( void );                                                                                             \n static void MISRA11_1_func1 (void)                                                                                                                               
Rules11.1+=                                          \t\t\t\t\t\t\t\tstatic void MISRA11_1_func1 (void)                \n {                                           
Rules11.1+=                                     \t\t\t\t   {                                                 \n      fp16 fp1 = NULL;              
Rules11.1+=                                                                 \t\t\t\t\t\tfp16 fp1 = NULL;                              \n
Rules11.1+=  fp32 fp2 = ( fp32 ) fp1;                                                       /* Compliant - exception 1 */                 \n
Rules11.1+=  /* Non-compliant - function pointer into different function pointer */         pfp16 pfp1;                                   \n
Rules11.1+=  fp16 fp3 = ( fp16 ) 0x8000;                                                    ( void ) ( *pfp1 ( ) );                       \n
Rules11.1+=  /* Non-compliant - integer into function pointer */                           /* Compliant - exception 2 - cast function pointer into void */ \n
Rules11.1+= }                                                                           }                                                 \n"


Rules11.2= "Rule 11.2: Conversions shall not be performed between a pointer to an incomplete type and any other type. \n
Rules11.2+= \n
Rules11.2+= Exception: \n
Rules11.2+= 1. A null pointer constant may be converted into a pointer to an incomplete type. \n
Rules11.2+= 2. A pointer to an incomplete type may be converted into void.  \n
Rules11.2+= Violation:                                                                  Solution: \n
Rules11.2+= ******************************************************************************************************************************\n
Rules11.2+= static void MISRA11_2_func2 (void)                                          static void MISRA11_2_func2 (void)              \n
Rules11.2+= {                                                                       {                                               \n
Rules11.2+=   struct s;                                                                   struct s;                                     \n   struct t;                                                                                                                            
Rules11.2+=                                                                     \t\t\t\t\t\t\t\t\t\t\t struct t;                                     \n    /* Incomplete types */                                                
Rules11.2+=                                                        /* Incomplete types */                        \n struct s const *sp = NULL;             
Rules11.2+=                                                   \t\t\t\t     struct s const *sp = NULL;                    \n 
Rules11.2+=   int16_t const *ip;                                                                                                        \n
Rules11.2+=   ip = ( int16_t * ) sp;                                                      sp = NULL;                                    \n
Rules11.2+=   /* Non-compliant */                                                           /* Compliant - exception 1 */               \n
Rules11.2+=   sp = ( struct s * ) 1234;                                                   struct s *f ( void );                         \n
Rules11.2+=   /* Non-compliant */                                                         ( void ) f ( );                               \n
Rules11.2+=   \t                                                                          /* Compliant - exception 2 */                 \n  }   
Rules11.2+=                                                                           \t\t\t\t\t\t\t\t\t }                                               \n"


Rules11.3= "Rule 11.3: A cast shall not be performed between a pointer to object type and a pointer to a different object type. \n
Rules11.3+= \n
Rules11.3+= Violation:                                                                  Solution: \n
Rules11.3+= *******************************************************************************************************************************\n
Rules11.3+= static void MISRA11_3_func1 (void)                                         static void MISRA11_3_func1 (void)                  \n
Rules11.3+= {                                                                        {                                                   \n
Rules11.3+=   uint8_t varp1 = 1;                                                         const int16_t *p = NULL;                          \n uint8_t *p1 = &varp1;                                                                                                                              
Rules11.3+=                                                        \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t    const volatile int16_t *q;                        \n uint32_t *p2;                                             
Rules11.3+=                                                                                                                  \n p2 = ( uint32_t * ) p1;        
Rules11.3+=                                                       \t\t\t\t\t\tq = ( const volatile int16_t * ) p;               \n 
Rules11.3+=   /* Non-compliant - possible incompatible alignment */                       /* Compliant */                                  \n   *p2 += *p1;                       
Rules11.3+=                                                                   \t\t\t\t\t\tboo = ( q != NULL );                             \n
Rules11.3+=   boo = ( p2 != NULL );                                                           boo = ( p != NULL );                             \n
Rules11.3+= }                                                                          }                                                   \n"


Rules11.4= "Rule 11.4: A conversion should not be performed between a pointer to object and an integer type. \n
Rules11.4+= \n
Rules11.4+= Violation:                                                                  Solution: \n
Rules11.4+= ****************************************************************************************************************************\n
Rules11.4+= static void MISRA11_4_func1 (void)                                          Casting between a pointer and an               \n
Rules11.4+= {                                                                           integer type should be avoided where possible. \n 
Rules11.4+=   uint8_t *PORTA = ( uint8_t * ) 0x0002;                                                                                   \n       /* Non-compliant */                                                                                                                                                         
Rules11.4+=                                                                                              \n               int32_t addr = ( int32_t ) &p;\n    
Rules11.4+=    /* Non-compliant */                                                                                                     \n 
Rules11.4+= }                                                                                                                          \n"


Rules11.5= "Rule 11.5: A conversion should not be performed from pointer to void into pointer to object. \n
Rules11.5+= \n
Rules11.5+= Violation:   \n  ***************************************************************************************************************************                                                            
Rules11.5+= \n
Rules11.5+= static void MISRA11_5_func1 (void)                                                                         \n
Rules11.5+= {                                                                                                                                           \n 
Rules11.5+=   uint32_t *p32 = NULL;                                                                                               \n  void *p;	                                                                                                                                
Rules11.5+=  		                                                                                                                         \n   uint16_t *p16;                                            
Rules11.5+=                                                                                                             \n          p16 = p;       
Rules11.5+=                                                                                                                      \n 
Rules11.5+=   /* Non-compliant */                                                          \n p32 = ( uint32_t * ) p;   
Rules11.5+=                                                                                               \n
Rules11.5+=   /* Non-compliant */                                                                                                      \n
Rules11.5+=   Bool boo = ( p32 != NULL );                                                                                                                \n  } \n                                                               
                                                                                                                                          

Rules11.5+=Solution\n
Rules11.5+=**************************************************************************************************************************\n
Rules11.5+=static void MISRA_5_func1 (void)\n
Rules11.5+={
Rules11.5+= uint32_t *p32 =NULL;\n
Rules11.5+= void *p;\n
Rules11.5+= uint16_t *p16;\n
Rules11.5+= p = p32;\n
Rules11.5+=/* Compliant- pointer to uint32_t into pointer to void */\n
Rules11.5+= p =(void*) p16; \n
Rules11.5+=/ * Compliant*/\n
Rules11.5+=}\n"

Rules11.6= "Rule 11.6: A cast shall not be performed between pointer to void and an arithmetic type.. \n
Rules11.6+= \n
Rules11.6+= Violation:                                                                  Solution: \n
Rules11.6+= ******************************************************************************************************************************\n
Rules11.6+= static void MISRA11_6_func1 (void)                                        static void MISRA11_6_func1 (void)    \n
Rules11.6+= {                                                                         { \n 
Rules11.6+=   void * p;                                                                  void const * r;                 \n       void const * q = NULL;\n  uint32_t u, u32a = 1;                                                                                                                      
Rules11.6+=                                                                   \n void const * r;                         
Rules11.6+=                                                                                               \n    uint32_t u, u32a = 1;              
Rules11.6+=                                                                                         \n 
Rules11.6+=   p = ( void * ) 0x1234u;                                                    r = ( void * ) 0;               \n                               
Rules11.6+=                                 \t\t\t\t\t\t /* Compliant */                 \n
Rules11.6+=   q = ( void * ) 1024.0f;                                                                                    \n
Rules11.6+=   /* Non-compliant - undefined */                                                                            \n
Rules11.6+=   u = ( uint32_t ) p;                                                                                        \n
Rules11.6+=   /* Non-compliant - implementation-defined */                                                               \n }            
Rules11.6+=                                                                          \t\t\t\t\t\t\t\t   }                                 \n"


Rules11.7= "Rule 11.7: A cast shall not be performed between pointer to void and an arithmetic type.. \n
Rules11.7+= \n
Rules11.7+= Violation:                                                                  Solution: \n
Rules11.7+= *********************************************************************************************************************************\n
Rules11.7+= static void MISRA11_7_func1 (void)                                          conversion between pointer to object              \n
Rules11.7+= {                                                                           and a non-integer arithmetic type should          \n 
Rules11.7+=   int16_t * p = NULL;                                                         be avoided where possible.                      \n  float32_t f;                                                                                                                              
Rules11.7+=                                                                                                                  \n  char cha ;                               
Rules11.7+=                                                                                                                     \n
Rules11.7+=   f = ( float32_t ) p;                                                                                                        \n  /* Non-compliant */                
Rules11.7+=                                                                                                            \n 
Rules11.7+=   p = ( int16_t * ) f;                                                                                                        \n   /* Non-compliant  */                             
Rules11.7+=                                                                                                          \n
Rules11.7+=   cha = ( char ) p ;                                                                                                          \n
Rules11.7+=   /* Non-compliant  */                                                                                                        \n              
Rules11.7+= }                                                                                                                             \n"
    
	
Rules11.8= "Rule 11.8: A cast shall not be performed between pointer to void and an arithmetic type.. \n
Rules11.8+= \n
Rules11.8+= Violation:                                                                  Solution: \n
Rules11.8+= *****************************************************************************************************************************\n
Rules11.8+= static void MISRA11_8_func1 (void)                                          static void MISRA11_8_func1 (void)                        \n
Rules11.8+= {                                                                            {                                                        \n 
Rules11.8+=   uint16_t x;                                                                    uint16_t const *pi;\nuint16_t const * const   cpi  =  &x ;  \t\t\t\t\t\tuint16_t const * const   cpi  =  &x;                                                                                                                             
Rules11.8+=                  \n uint16_t * const  *pcpi = NULL;             
Rules11.8+=                                                   					                                  \n
Rules11.8+=   uint16_t * const	*ppi;                                                                                                             \n    uint16_t const *pi;              
Rules11.8+=                                                                                                                    \n  
Rules11.8+=   uint16_t const *qi;                                                                                                                 \n  qi = (uint16_t *)pci;                             
Rules11.8+=                                                             \t\t\t\t\tpi = cpi;                                            \n
Rules11.8+=   /* Non-compliant  */                                                           /* Compliant - no conversion, no cast required */    \n
Rules11.8+=   ri = (uint16_t *)pvi;                                                                                                               \n
Rules11.8+=   /* Non-compliant  */                                                                                                                \n
Rules11.8+=   ppi = (uint16_t * *)pcpi;                                                                                                           \n                                                            \n             
Rules11.8+=   /* Non-compliant  */                                                                                                                \n
Rules11.8+= }                                                                            }                                                        \n"
	
	
Rules11.9= "Rule 11.9: The macro NULL shall be the only permitted form of integer null pointer constant. \n
Rules11.9+= \n
Rules11.9+= Violation:                                                                  Solution: \n
Rules11.9+= *********************************************************************************************************************\n
Rules11.9+= static void MISRA11_9_func1 (uint8_t const *p)                              static void MISRA11_9_func2 (void)                 \n
Rules11.9+= {                                                                           {                                                  \n 
Rules11.9+=   int32_t const *p1 = 0;                                                       int32_t const *p2 = ( void * ) 0;               \n   /* Non-compliant */                                                                                                                             
Rules11.9+=                                                           \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t /* Compliant */                                 \n  _Bool boo = ( p1 == MY_NULL_1 );                                   
Rules11.9+=                                               \t\t _Bool boo = ( p2 == MY_NULL_2 );                \n
Rules11.9+=   /* Non-compliant */                                                         \t /* Compliant */                                 \n                    
Rules11.9+=                                                                                           \n }                   
Rules11.9+=       																		\t\t\t\t\t\t\t     }\n"


Rules12.1=" Rule 12.1: The precedence of operators within expressions should be made explicit.   \n
Rules12.1+= \n
Rules12.1+= Violation:                                                         \t\t\t\t\t\t\t Solution: \n
Rules12.1+= ***********************************************************************************************************************************************************\n
Rules12.1+=static void MISRA12_1_func1 (void) 									\t\tstatic void MISRA12_1_func1 (void) \n	{							
Rules12.1+=																	\t\t\t\t\t\t{\n
Rules12.1+=	uint32_t arr2[1] = { 1 };												{	struct a \n
Rules12.1+=	uint8_t x = 1, y = 1;														int16_t n;\n
Rules12.1+=	arr2[0] = sizeof x + y; 												};\n
Rules12.1+= /* Non-compliant - write either sizeof ( x ) + y or sizeof ( x + y ) */	\t\t\t\t\t\tstruct a *pa[] = { NULL };\n
Rules12.1+=}													\t\t\tint16_t arr1[2] = { 1, 1 };	\n														
Rules12.1+=																			\t\tint16_t const *pj = &arr1[0];\n
Rules12.1+=																			\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tint16_t j = pa[0]->n;	\n																		
Rules12.1+=																			\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t/* Compliant - no need to write ( pa[ 0 ] )->n */	\n			
Rules12.1+=																			\t\t\t\t\t\t\t\t\t\t\t\t\twhile( *pj++ == 1 ) \n
Rules12.1+=																			\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t/* Compliant - no need to write *( pj++ ) */\n
Rules12.1+=																				\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tMISRA12_int16_volatile_helper += j;\n
Rules12.1+=																			\t\t\t\t\t\t\t\t\t\t\t\t\t}\n"



Rules12.2= "Rule 12.2: The right hand operand of a shift operator shall lie in the range zero to one less than the width in  \n
Rules12.2+=            bits of the essential type of the left hand operand.    \n
Rules12.2+= \n
Rules12.2+= Violation:                                                             Solution: \n
Rules12.2+= *********************************************************************************************************************\n
Rules12.2+= static void MISRA12_2_func1 (void)                                     static void MISRA12_2_func1 (void)                      \n
Rules12.2+= {                                                                      {                                                       \n 
Rules12.2+=   uint16_t u16a = 1, u16b = 1;                                            uint16_t u16a = 1, u16b = 1;                         \n  uint32_t u32a = 1, u32b = 1;                                                                                                                              
Rules12.2+=                                             			                                  \n uint64_t u64a;    \t\t\t\t\t\t\t uint64_t u64a;                            
Rules12.2+=                                                                                                  \n
Rules12.2+=   u32a = u32a << 32u; /*Non-compliant */		                  	 u32a = u32a << 31u; /* Compliant */                  \nMISRA12_uint32_volatile_helper = u32a;                 
Rules12.2+=                                   \t\t  MISRA12_uint32_volatile_helper = u32a;               \n
Rules12.2+=   u16a = u16a << 16u; /*Non-compliant */                                  u16a = u16a << 15u; /* Compliant */                  \n
Rules12.2+=   MISRA12_uint16_volatile_helper = u16a;                                  MISRA12_uint16_volatile_helper = u16a;               \n
Rules12.2+= }                                                                      }\n"


Rules12.3= "Rule 12.3: The comma operator should not be used   \n
Rules12.3+= \n
Rules12.3+= Violation:                                                             Solution: \n
Rules12.3+= ******************************************************************************************************************\n
Rules12.3+= static void MISRA12_3_func2 (void)                                     static void MISRA12_3_func2 (void)                      \n
Rules12.3+= {                                                                      {                                                       \n 
Rules12.3+=   MISRA12_3_func1 ( ( 1, 2 ), 3 );                                        MISRA12_3_func1 ( 2 , 3 );                           \n /* Non-compliant - how many parameters? */                                                                                                                              
Rules12.3+=                                \t\t\t\t\t\t\t /* Compliant */                                      \n }                                            
Rules12.3+=                                                                       \t\t\t\t}\n"


Rules12.4= "Rule 12.4: Evaluation of constant expressions should not lead to unsigned integer wrap-around   \n
Rules12.4+= \n
Rules12.4+= Violation:                                                             Solution: \n
Rules12.4+= *************************************************************************************************************\n
Rules12.4+= bool_t b;                                                                                                                      \n
Rules12.4+= static void MISRA12_4_func1 (void)                                     static void MISRA12_4_func1 (void)                      \n
Rules12.4+= {                                                                      {                                                       \n 
Rules12.4+=   uint16_t y =  b ? 0u : ( 0u - 1u );                                        uint16_t y = ( 0u == 0u ) ? 0u : ( 0u - 1u );     \n /* Non-compliant */                                                                                                                              
Rules12.4+=                                                          \t\t\t\t\t\t\t\t\t\t  /* Compliant */                                   \n }                                            
Rules12.4+=                                                                       \t\t\t\t}\n"


Rules12.5= "Rule 12.5: The sizeof operator shall not have an operand which is a function parameter declared as “array of type”.   \n
Rules12.5+= \n
Rules12.5+= Violation:                                                             Solution: \n
Rules12.5+= **************************************************************************************************************\n
Rules12.5+= static int16_t MISRA12_int16_helper_A[] = { 1, 2, 3, 4, 5 };           static int16_t MISRA12_int16_helper_A[] = { 1, 2, 3, 4, 5 };           \n
Rules12.5+= static void MISRA12_5_func1 (int16_t A[4])                             static void MISRA12_5_func1 ( int16_t A[ 4 ] )                         \n
Rules12.5+= {                                                                      {                                                                      \n 
Rules12.5+=   uint32_t numElements = sizeof ( A ) / sizeof ( int16_t );              uint32_t numElements_A = sizeof ( MISRA12_int16_helper_A )           \n                                                          \n 
Rules12.5+=   /* Non - Compliant as it always gives                                                         / sizeof ( MISRA12_int16_helper_A[ 0 ] );     \n  the same answer, irrespective of the                                                                                                          
Rules12.5+=                                     \t\t\t\t\t\t\t\t\t\t/* Compliant - as numElements_A will be                              \n    number of members that appear to be                                           
Rules12.5+=                                        given the expected value 5 */                                    \n
Rules12.5+=     in the array (2 in this case),because                                                                                                    \n
Rules12.5+=     A has type int16_t * and not                                        }                                                                    \n
Rules12.5+=     int16_t[4]. As sizeof ( int16_t *) is                                                                                                    \n        
Rules12.5+=     often the same as sizeof( int16_t),                                                                                                      \n numElements is likely to always have                                
Rules12.5+=                                                                                                         \n                       
Rules12.5+=     the value 1.*/                                                                                                                           \n        
Rules12.5+= }\n"


Rules13.1= "Rule 13.1: Initializer lists shall not contain persistent side effects. \n
Rules13.1+= \n
Rules13.1+= Violation:                                                                  Solution: \n
Rules13.1+= ************************************************************************************************************************\n
Rules13.1+= static volatile uint16_t vv1 = 1;                                           static volatile uint16_t vv1 = 1;                    \n
Rules13.1+= uint16_t u16a = 1;                                                          uint16_t u16a = 1;                                   \n 
Rules13.1+= static void MISRA13_1_func1 ( void )                                        static void MISRA13_1_func2(uint16_t x, uint16_t y ) \n {      \t\t\t\t\t\t\t\t\t{                                                                                                                         
Rules13.1+=                                                                                                                              \n uint16_t a[ 2 ] = { vv1, 0 };                                               
Rules13.1+=                                                  uint16_t a[ 2 ] = { x + y, x - y };              \n
Rules13.1+=   /* Non-compliant - volatile access is persistent side effect */              /* Compliant - no side effects */                \n                 
Rules13.1+=                                                                                 \t\t\t\t\t\t\t\t\tvv1 = 3;                                        \n
Rules13.1+=   u16a = a[1];                                                                  u16a = a[0];                                    \n
Rules13.1+=  }                                                                          }                                                   \n"


Rules13.2= "Rule 13.2: The value of an expression and its persistent side effects shall be the same under all permitted evaluation orders. \n
Rules13.2+= \n
Rules13.2+= Violation:                                                                  Solution: \n
Rules13.2+= ************************************************************************************************************************\n
Rules13.2+= static void MISRA13_2_func1 (void)                                          static void MISRA13_2_func3 (void)                 \n
Rules13.2+= {                                                                           {                                                  \n 
Rules13.2+=   int16_t i = 1;                                                                volatile uint8_t PORT = 1;                      \n   uint16_t a[3] = { 1, 1, 1 }, b[3] =    { 2, 2, 2 };                                                                                                                             
Rules13.2+=                            \t\t\t\t\t\t\t\t PORT = PORT & 0x80u;                            \n  \#define COPY_ELEMENT( index ) ( a[( index )] = b[( index )] )                                                   \n                                               
Rules13.2+=                   \t\t\t\t\t/* Compliant - PORT is read and modified */    \n                                    \n
Rules13.2+=      COPY_ELEMENT ( i++ );                                                                                                     \n   MISRA13_uint16_volatile_helper = a[1];                 
Rules13.2+=                                                                                          \n
Rules13.2+= }                                                                            }                                                   \n"


Rules13.3= "Rule 13.3: A full expression containing an increment (++) or decrement (--) operator should have no other potential side \n
Rules13.3+= effects other than that caused by the increment or decrement operator. \n
Rules13.3+= \n
Rules13.3+= Violation:                                                                  Solution: \n
Rules13.3+= ****************************************************************************************************************\n
Rules13.3+= static void MISRA13_3_func1 (void)                                          static void MISRA13_3_func3 (void)     \n
Rules13.3+= {                                                                        {                                      \n
Rules13.3+=     u32a = u32b++;                                                             uint16_t z = 1, i = 0;              \n
Rules13.3+=     /* Non-compliant */                                                        uint16_t a[3] = { 1, 1, 1 };        \n
Rules13.3+=    	                                                                           uint16_t *p = &a[0];                \n
Rules13.3+=     u32a = ++u32b + u32c--;                                                    z++;  /* Compliant */               \n
Rules13.3+=    /* Non-compliant */                                                         a[ i ]++;  /* Compliant */          \n }                              
Rules13.3+=                                                                            \t\t\t\t\t   }                                      \n"


Rules13.4= "Rule 13.4: The result of an assignment operator should not be used. \n
Rules13.4+= \n
Rules13.4+= Violation:                                                                  Solution: \n
Rules13.4+= ***********************************************************************************************************************\n
Rules13.4+= static void MISRA13_3_func1 (void)                                          static void MISRA13_3_func3 (void)                 \n
Rules13.4+= {                                                                         {                                                  \n
Rules13.4+=     u32a = u32b++;                                                             uint16_t z = 1, i = 0;                          \n
Rules13.4+=     /* Non-compliant */                                                        uint16_t a[3] = { 1, 1, 1 };                    \n
Rules13.4+=    	                                                                           uint16_t *p = &a[0];                            \n
Rules13.4+=     u32a = ++u32b + u32c--;                                                    z++;  /* Compliant */                           \n
Rules13.4+=    /* Non-compliant */                                                         a[ i ]++;  /* Compliant */                      \n }                           
Rules13.4+=                                                                            \t\t\t\t\t\t  }                                                  \n"


Rules13.5= "Rule 13.5: The right hand operand of a logical && or || operator shall not contain persistent side effects. \n
Rules13.5+= \n
Rules13.5+= Violation:                                                                  Solution: \n
Rules13.5+= ******************************************************************************************************************\n
Rules13.5+= static uint16_t MISRA13_5_func2 ( uint16_t y )                              static uint16_t MISRA13_5_func1 ( uint16_t y )                    \n
Rules13.5+= {                                                                           {                                                                 \n
Rules13.5+=     static uint16_t temp2 = 0;                                                     uint16_t temp1 = y;                                        \n
Rules13.5+=     /* This side effect is persistent                                            /* These side effects are not                                                                                          \n
Rules13.5+=    	as lifetime of temp is ended when returned */                                 persistent as seen by the caller */                         \n
Rules13.5+=     temp2 = y + temp2;                                                            temp1 = y + 0x8080U;                                        \n
Rules13.5+=     return temp2;                                                                 return temp1;                                              \n                            
Rules13.5+= }                                                                            }                                                                \n
Rules13.5+= static void MISRA13_5_func3 ( _Bool b )                                     static void MISRA13_5_func3 ( _Bool b )                           \n
Rules13.5+= {                                                                           {
Rules13.5+=    uint16_t a = 1;                                                                 uint16_t a = 1;                                             \n
Rules13.5+=    /* Non-compliant - MISRA13_5_func2 ( ) has a persistent                         /* Compliant - MISRA13_5_func1 ( ) has                      \n
Rules13.5+=    side effect - but Rule 13.5 is switched off  */                                   no persistent side effects */                             \n
Rules13.5+=    if( b && ( a == MISRA13_5_func2( 1u ) ) )                                       if ( b &&( a == MISRA13_5_func1 ( 1u ) ))                   \n
Rules13.5+=   { }                                                                              {}                                                          \n
Rules13.5+= }                                                                           }                                                                  \n"


Rules13.6= "Rule 13.6: The operand of the sizeof operator shall not contain any expression which has potential side effects. \n
Rules13.6+= \n
Rules13.6+= Violation:                                                                  Solution: \n
Rules13.6+= **********************************************************************************************************************************************\n
Rules13.6+= static void MISRA13_6_func1 (void)                                          static void MISRA13_6_func1  (void)                               \n
Rules13.6+= {                                                                         {                                                                  \n
Rules13.6+=     volatile uint32_t i = 1;                                                   volatile uint32_t i = 1;                                        \n
Rules13.6+=     uint32_t j = 1;                                                            uint32_t j = 1;                                                 \n
Rules13.6+=    	uint32_t s;                                                                uint32_t s;                                                     \n
Rules13.6+=     s = sizeof ( j++ ); 	                                                     s = sizeof ( j );   /* Compliant */                             \n
Rules13.6+=    /* Non-compliant */                                                         s = sizeof ( i );  /* Compliant */                               \n }                           
Rules13.6+=   																			\t\t\t\t\t\t  }\n"


Rules14.1= "Rule 14.1: A loop counter shall not have essentially floating type \n
Rules14.1+= \n
Rules14.1+= Violation:                                                                  Solution: \n
Rules14.1+= ************************************************************************************************************************\n
Rules14.1+= static void MISRA14_1_func1 (void)                                          static void MISRA14_1_func2 (void)                               \n
Rules14.1+= {                                                                           {                                                                \n
Rules14.1+=     uint32_t counter = 0u;                                                     float32_t f;                                                  \n
Rules14.1+=     for(float32_t f = 0.0f; f < 1.0f; f+= 0.001f )                             for ( uint32_t counter = 0u; counter < 1000u; ++counter )     \n
Rules14.1+=    	/* Non-compliant - value of counter is unlikely */                         /* Compliant - integer loop counter guarantees */             \n
Rules14.1+=     { 	                                                                       {                                                             \n
Rules14.1+=        /* to be 1 000 at the end of the loop */                                     /* 1 000 iterations */                                   \n                            
Rules14.1+=         ++counter;                               		f = ( float32_t ) counter * 0.001f;                      \n
Rules14.1+=     }                                                                          }                                                             \n
Rules14.1+=     MISRA14_uint32_volatile_helper = counter;                                  MISRA14_float32_volatile_helper = f;                          \n
Rules14.1+=   }                                                                         }                                                                \n"


Rules14.2= "Rule 14.2: A for loop shall be well-formed. \n
Rules14.2+= \n
Rules14.2+= Violation:                                                                  Solution: \n
Rules14.2+= **********************************************************************************************************************************************\n
Rules14.2+=static void MISRA14_2_func1 (void)											static void MISRA14_2_func1 (void)\n
Rules14.2+={												\t\t{\n
Rules14.2+=   bool_t flag = false;													bool_t flag = false;\n
Rules14.2+= for (int16_t i = 0;(i < 5)&& !flag; i++)										for (int16_t i = 0;(i < 5)&& !flag; i++)\n 
Rules14.2+=	{															{\n
Rules14.2+=	   if ( i < 5 )														 if ( i < 5 )\n
Rules14.2+=	   {							 	     						 \t{	\n
Rules14.2+=	   }																				    \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tflag = true;\n 
Rules14.2+=	i = i + 3; 																				\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t/* Compliant - allows early \n
Rules14.2+=  /* Non-compliant - altering the loop 										\ttermination of loop */\n
Rules14.2+=    counter inside for loop body is not allowed*/  									 }\n
Rules14.2+=	}												\t\t\t MISRA14_uint16_volatile_helper += 2u;\n
Rules14.2+=  MISRA14_uint16_volatile_helper += 2u;											}\n
Rules14.2+=}										\t\t\t}\n"



Rules14.3= "Rule 14.3: Controlling expressions shall not be invariant. \n
Rules14.3+= \n
Rules14.3+= Violation:                                                                  Solution: \n
Rules14.3+= **********************************************************************************************************************************************\n
Rules14.3+= static void MISRA14_3_func2 (void)                                          static void MISRA14_3_func2(void)                              \n
Rules14.3+= {                                                                           {                                                                \n
Rules14.3+=     const uint16_t numcyl = 4u;                                                const volatile uint16_t numcyl = 4u;                          \n
Rules14.3+=     if ( numcyl == 4u )                                                        if ( numcyl_cal == 4u )                                       \n
Rules14.3+=    	{                                                                          {                                                             \n
Rules14.3+=         /* Non-compliant - compiler is permitted                                 /* Compliant - compiler assumes numcyl may be changed by an \n
Rules14.3+=        to assume that numcyl always has value 4 */                                external method even though the program cannot modify its value */  \n }                           
Rules14.3+=                                                                              \t\t\t\t\t\t    }                                                             \n
Rules14.3+=  }                                                                          }  \n"                                                              
                                                                        

Rules14.4= "Rule 14.4: Controlling expressions shall not be invariant. \n
Rules14.4+= \n
Rules14.4+= Violation:                                                                  Solution: \n
Rules14.4+= **********************************************************************************************************************************************\n
Rules14.4+= static void MISRA14_4_func2 (void)                                          static void MISRA14_4_func2 (void)                              \n
Rules14.4+= {                                                                           {                                                                \n
Rules14.4+=     int32_t i = 0;                                                              int32_t i = 0;                                               \n
Rules14.4+=     if ( i )                                                                    if ( i != 0 )                                                \n
Rules14.4+=    	/* Non-compliant */                                                         /* Compliant */                                              \n
Rules14.4+=     {                                                                           {                                                             \n
Rules14.4+=     }                                                                           }                                                             \n
Rules14.4+=  }                                                                          }                                                                 \n"
	

Rules15.1= "Rule 15.1: The goto statement should not be used \n
Rules15.1+= \n
Rules15.1+= Violation:                                                                  Solution: \n
Rules15.1+= **************************************************************************************************************\n
Rules15.1+= static void MISRA15_2_func2 ( void )                                        static void MISRA15_1_func1 ( void )                              \n
Rules15.1+= {                                                                           {                                                                 \n	
Rules15.1+= 	int32_t j = 0;                                                              int32_t j = 0;                                                \n
Rules15.1+= L1:                                                                             /* Compliant */                                               \n
Rules15.1+= 	++J;                                                                        for(j; j<=10;j++);                                            \n
Rules15.1+= 	if ( 10 == j )                                                              if ( 10 == j )                                                \n
Rules15.1+=     {                                                                           {                                                             \n
Rules15.1+=     }                                                                           }                                                             \n
Rules15.1+=     goto L1; /* Non-compliant */                                                j++;                                                          \n
Rules15.1+=     ++j;                                                                        MISRA15_int32_volatile_helper = j;                            \n
Rules15.1+= }                                                                           }                                                                 \n"


Rules15.2= "Rule 15.2: The goto statement shall jump to a label declared later in the same function \n
Rules15.2+= \n
Rules15.2+= Violation:                                                                  Solution: \n
Rules15.2+= *******************************************************************************************************************\n
Rules15.2+= static void MISRA15_2_func2 ( void )                                        static void MISRA15_2_func2 ( void )                              \n
Rules15.2+= {                                                                           {                                                                 \n
Rules15.2+= 	int32_t j = 0;                                                              int32_t j = 0;                                                \n 
Rules15.2+= L1:                                                                             ++j;                                                          \n
Rules15.2+= 	++j;                                                                                                                                      \n
Rules15.2+=     if ( 10 == j )                                                              if ( 10 == j )                                                \n
Rules15.2+=     {                                                                           {                                                             \n 
Rules15.2+=     }                                                                               goto L2; /* Compliant */                                  \n
Rules15.2+= 	goto L1; /* Non-compliant */                                                }                                                             \n
Rules15.2+=     ++j;                                                                        ++j;                                                          \n
Rules15.2+=     MISRA15_int32_volatile_helper = j;                                          MISRA15_int32_volatile_helper = j;                            \n 
Rules15.2+= }                                                                           }                                                                 \n" 


Rules15.3= "Rule 15.3: Any label referenced by a goto statement shall be declared in the same block, or in any block enclosing the goto statement \n
Rules15.3+= \n
Rules15.3+= Violation:                                                                  Solution: \n
Rules15.3+= **********************************************************************************************************************************************\n
Rules15.3+= static void MISRA15_3_func1 ( int32_t a )                                   static void MISRA15_3_func1 ( int32_t a )                         \n 
Rules15.3+= {                                                                           {                                                                 \n  
Rules15.3+= 	if ( a <= 0 )                                                               if ( a <= 0 )                                                 \n
Rules15.3+= 	{                                                                           {                                                             \n
Rules15.3+=		                                                                               goto L1; /* Compliant */                                   \n
Rules15.3+=     }                                                                           }                                                             \n
Rules15.3+= 	goto L2;	/* Non-compliant */                                         L1:                                                               \n
Rules15.3+= 	if ( a > 0 )                                                                if ( a > 0 )                                                  \n
Rules15.3+= 	{                                                                           {                                                             \n
Rules15.3+= 	L2:                                                                         }                                                             \n
Rules15.3+= 	  ;                                                                                                                                       \n
Rules15.3+=     }	                                                                                                                                      \n        
Rules15.3+= 	MISRA15_int32_volatile_helper += 2;                                         MISRA15_int32_volatile_helper += 2;                           \n
Rules15.3+= }                                                                           }                                                                 \n
Rules15.3+= \n
Rules15.3+= static void MISRA15_3_func2 ( int32_t a )                                   static void MISRA15_3_func2 ( int32_t a )                         \n
Rules15.3+= {                                                                           {                                                                 \n
Rules15.3+= 	if ( a <= 0 )                                                               if ( a <= 0 )                                                 \n
Rules15.3+= 	{                                                                           {                                                             \n
Rules15.3+= 		goto L2; /* Non-compliant */                                            }                                                             \n
Rules15.3+= 	}                                                                           goto L1; /* Compliant */                                      \n
Rules15.3+= 	if ( a > 0 )                                                            L1:                                                               \n
Rules15.3+= 	{                                                                           if ( a > 0 )                                                  \n
Rules15.3+=     L2:                                                                         {                                                             \n
Rules15.3+=        ;                                                                        }                                                             \n
Rules15.3+= 	MISRA15_int32_volatile_helper += 2;                                         MISRA15_int32_volatile_helper += 2;                           \n
Rules15.3+= }                                                                           }                                                                 \n
Rules15.3+= \n
Rules15.3+= static void MISRA15_3_func3 ( int32_t y )                                   static void MISRA15_3_func3 ( int32_t y )                         \n 
Rules15.3+= {                                                                           {                                                                 \n 
Rules15.3+= 	int32_t x = 1;                                                              int32_t x = 1;                                                \n
Rules15.3+= 	switch ( x )                                                                switch ( x )                                                  \n
Rules15.3+= 	{                                                                           {                                                             \n  
Rules15.3+= 		case 0:                                                                     case 0:                                                   \n 
Rules15.3+= 			if ( x == y )                                                               if ( x == y )                                         \n
Rules15.3+= 			{                                                                           {                                                     \n  
Rules15.3+= 				goto L1; /* Non-compliant */                                                goto L1; /* compliant */                          \n
Rules15.3+= 			}                                                                           }                                                     \n
Rules15.3+=             y = x;                                                                      y = x;                                                \n
Rules15.3+=             ++y;                                                                                                                              \n break;                    
Rules15.3+= 			                                                                 \t\t\t\t\t\t L1:                                                       \n
Rules15.3+= 		case 1:                                                                         ++y;                                                  \n
Rules15.3+= 			y = x;                                                                      break;                                                \n
Rules15.3+= 	L1:                                                                             case 1:                                                   \n
Rules15.3+= 			++x;                                                                            y = x;                                            \n 
Rules15.3+= 			break;                                                                          ++x;                                              \n
Rules15.3+= 		default:                                                                            break;                                            \n
Rules15.3+= 			break;                                                                  default:                                                  \n  
Rules15.3+= 	}                                                                                       break;                                            \n   
Rules15.3+= 	MISRA15_int32_volatile_helper = x + y;                                      } MISRA15_int32_volatile_helper = x + y;                      \n
Rules15.3+= }                                                                            }                                                                \n "


Rules15.4= "Rule 15.4: There should be no more than one break or goto statement used to terminate any iteration statement \n
Rules15.4+= \n
Rules15.4+= Violation:                                                                  Solution: \n
Rules15.4+= *************************************************************************************************************\n
Rules15.4+= static bool_t MISRA15_4_func1 ( int32_t z )                                 static bool_t MISRA15_4_func1 ( int32_t z )                       \n
Rules15.4+= {                                                                           {                                                                 \n
Rules15.4+= 	MISRA15_int32_volatile_helper = z;                                          MISRA15_int32_volatile_helper = z;                            \n
Rules15.4+= 	return true;                                                                return true;                                                  \n
Rules15.4+= }                                                                           }                                                                 \n
Rules15.4+= static void MISRA15_4_func3 (void)                                          static void MISRA15_4_func2 (void)                                \n
Rules15.4+= {                                                                           {                                                                 \n
Rules15.4+=     int32_t x;                                                                  int32_t x, y;                                                 \n 
Rules15.4+=     for ( x = 0; x < 4; ++x ) /*Non-compliant,Rule 15.4 is switched off*/       for ( x = 0; x < 4; ++4 )                                     \n
Rules15.4+=     {                                                                           {                                                             \n
Rules15.4+=         if ( MISRA15_4_func1 ( x ) )                                                if ( MISRA15_4_func1 ( x ) )                              \n
Rules15.4+=         {                                                                           {                                                         \n 
Rules15.4+= 			break;                                                                      break;                                                \n
Rules15.4+=         }                                                                           }                                                         \n   
Rules15.4+=         else if ( MISRA15_4_func1 ( x ) )                                           for ( y = 0; y < x; ++y )                                 \n 
Rules15.4+=         {                                                                           {                                                         \n
Rules15.4+= 			goto EXIT;                                                                  if ( MISRA15_4_func1 ( 4 - y ) )			          \n  
Rules15.4+=         }                                                                               {                                                     \n
Rules15.4+=         else                                                                                break;                                            \n
Rules15.4+= 		{                                                                               }                                                     \n 
Rules15.4+= 			if ( MISRA15_4_func1 ( x ) )                                                for ( y = 0; y < x; ++y )                             \n 
Rules15.4+= 			{                                                                           {                                                     \n
Rules15.4+= 				MISRA15_int32_volatile_helper += 2;                                         if ( MISRA15_4_func1 ( 4 - y ) )                  \n
Rules15.4+= 			}                                                                               {	                                              \n
Rules15.4+= 		}                                                                                       break; /* Compliant */                        \n
Rules15.4+= 	}                                                                                       }                                                 \n 
Rules15.4+=     EXIT:                                                                               }                                                     \n   
Rules15.4+= 		;                                                                           }                                                         \n  
Rules15.4+= }                                                                                   MISRA15_int32_volatile_helper += 2; }                     \n static uint32_t MISRA15_4_func4 (void)  
Rules15.4+= \n 	\t\t\t\t\t\t\t\t\tstatic uint32_t MISRA15_4_func4 (void)																		        
Rules15.4+=                            \n
Rules15.4+= {                                                                           {                                                                 \n
Rules15.4+= 	return (uint32_t) MISRA15_int32_volatile_helper;                            return (uint32_t) MISRA15_int32_volatile_helper;              \n
Rules15.4+= }                                                                           }                                                                 \n
Rules15.4+= static void MISRA15_4_func5 (void)                                          static void MISRA15_4_func6 (void)                                \n
Rules15.4+= {                                                                           {                                                                 \n
Rules15.4+= 	uint32_t x = 1, y = 1, z;                                                   uint32_t x = 1, y = 1, z;                                     \n
Rules15.4+= 	while ( x != 0u )	/*Non-compliant, Rule 15.4 is switched off*/            while ( x != 0u ) /*compliant- but Rule 15.4 is switched off*/\n 
Rules15.4+= 	{                                                                           {                                                             \n 
Rules15.4+= 		x = MISRA15_4_func4();                                                      x = MISRA15_4_func4();                                    \n   
Rules15.4+= 		if ( x == 1u )                                                              if ( x == 1u )                                            \n    
Rules15.4+= 		{                                                                           {                                                         \n  
Rules15.4+= 			break;                                                                      break;                                                \n
Rules15.4+= 		}                                                                           }                                                         \n
Rules15.4+= 		while ( y != 0u )                                                           while ( y != 0u )		                                  \n
Rules15.4+= 		{                                                                           {                                                         \n
Rules15.4+= 			y = MISRA15_4_func4();                                                      y = MISRA15_4_func4();                                \n   
Rules15.4+= 			if ( y == 1u )                                                              if ( y == 1u )                                        \n 
Rules15.4+= 			{                                                                           {                                                     \n  
Rules15.4+= 				goto L1;                                                                    break;                                            \n
Rules15.4+= 			}                                                                           }                                                     \n  
Rules15.4+= 		}                                                                           }                                                         \n 
Rules15.4+= 	}                                                                           }                                                             \n     
Rules15.4+= 	L1:	                                                                        z = x + y;                                                    \n  
Rules15.4+= 	z = x + y;                                                                                                                                \n
Rules15.4+= 	MISRA15_int32_volatile_helper = (int32_t) z;  }                             MISRA15_int32_volatile_helper = (int32_t) z;                  \n
Rules15.4+= }                                                                           }                                                                 \n"           


Rules15.5= "Rule 15.5: A function should have a single point of exit at the end \n
Rules15.5+= \n
Rules15.5+= Violation:                                                                  Solution: \n
Rules15.5+= ********************************************************************************************************************\n
Rules15.5+= static bool_t MISRA15_5_func1 ( uint16_t n, char const *p )                 static bool_t MISRA15_5_func1 ( uint16_t n, char const *p )       \n
Rules15.5+= {                                                                           {                                                                 \n
Rules15.5+= 	if ( n > 5u )                                                               if ( n > 5u )                                                 \n
Rules15.5+= 	{                                                                           {                                                             \n
Rules15.5+= 	   return false;	/*Non-compliant, Rule 15.4 is switched off*/                 }                                                             \n
Rules15.5+= 	}                                                                                                                                         \n
Rules15.5+= 	if ( p == NULL )                                                            if ( p == NULL )                                              \n
Rules15.5+= 	{                                                                           {                                                             \n
Rules15.5+=         return false;	/*Non-compliant, Rule 15.4 is switched off*/                 }                                                             \n
Rules15.5+= 	}                                                                                                                                         \n
Rules15.5+= 	return true;                                                                return true; /* Compliant */                                  \n
Rules15.5+= }                                                                           }  \n"                                                             
				 

Rules15.6= "Rule 15.6: The body of an iteration-statement or a selection-statement shall be a compound-statement 
Rules15.6+= \n
Rules15.6+= Violation:                                                                  Solution: \n
Rules15.6+= **********************************************************************************************************************************************\n
Rules15.6+= static void MISRA15_6_func1(void)                                           static void MISRA15_6_func1(void)                                 \n
Rules15.6+= {                                                                           {                                                                 \n
Rules15.6+= 	MISRA15_int32_volatile_helper += 2;                                         MISRA15_int32_volatile_helper += 2;                           \n
Rules15.6+= }                                                                           }                                                                 \n
Rules15.6+= static void MISRA15_6_func2(void)                                           static void MISRA15_6_func2(void)                                 \n 
Rules15.6+= {                                                                           {                                                                 \n
Rules15.6+= 	MISRA15_int32_volatile_helper += 2;                                         MISRA15_int32_volatile_helper += 2;                           \n
Rules15.6+= }                                                                           }                                                                 \n
Rules15.6+= static void MISRA15_6_func3 (void)                                          static void MISRA15_6_func5 ( int32_t i )                         \n
Rules15.6+= {                                                                           {                                                                 \n   
Rules15.6+= 	int32_t i = 0;                                                              if ( i < 3 ) { MISRA15_6_func1 ( ); }                         \n                         
Rules15.6+= 	while ( i < 2 )                                                             else if ( i < 2 )	/* Compliant by exception                 \n
Rules15.6+= 		MISRA15_6_func1(); 		/* Non-compliant */                                                - uses of else if is allowed */            \n
Rules15.6+= 		MISRA15_6_func2();		/* MISRA15_6_func2() should                 { MISRA15_6_func2 ( ); }                                      \n  
Rules15.6+= 								have been added to the loop body */             else { ; }                                                    \n
Rules15.6+= }                                                                           }                                                                 \n
Rules15.6+= \n
Rules15.6+= static void MISRA15_6_func4 ( int32_t i )                                   static void MISRA15_6_func4 ( int32_t i )                         \n
Rules15.6+= {                                                                           {                                                                 \n
Rules15.6+=     if ( i < 3 )                                                                if ( i < 3 )                                                  \n
Rules15.6+=         if ( i < 2 )                /* Non-compliant */                         {   if ( i < 2 ) { MISRA15_6_func1 ( ); }                     \n
Rules15.6+=            MISRA15_6_func1 ( );     /* Non-compliant */                             else {                                                    \n
Rules15.6+=     else                                                                                 MISRA15_6_func2 ( ); } /* Compliant -                \n
Rules15.6+=         MISRA15_6_func2 ( ); 		/* Non-compliant - MISRA15_6_func2()                              MISRA15_6_func2() should actually       \n
Rules15.6+= 					appears to be the else statement */                                               be the else statement */                \n
Rules15.6+= 									/* to the first if */                       }                                                             \n
Rules15.6+= }                                                                           }	                                                              \n"


Rules15.7= "Rule 15.7: All if … else if constructs shall be terminated with an else statement 
Rules15.7+= \n
Rules15.7+= Violation:                                                                  Solution: \n
Rules15.7+= **********************************************************************************************************************************************\n
Rules15.7+= static void MISRA15_7_func1 ( int32_t i )                                   static void MISRA15_7_func1 ( int32_t i )                         \n
Rules15.7+= {                                                                           {                                                                 \n
Rules15.7+=     if ( i < 3 )                                                                if ( i < 3 )                                                  \n 
Rules15.7+=     {                                                                           {                                                             \n
Rules15.7+=         MISRA15_int32_volatile_helper += 2;                                         MISRA15_int32_volatile_helper += 2;                       \n
Rules15.7+=     }                                                                           }                                                             \n
Rules15.7+= 	else if ( i < 2 )                                                           else if ( i < 2 )                                             \n
Rules15.7+= 	{                                                                           {                                                             \n
Rules15.7+= 		MISRA15_int32_volatile_helper += 2;                                         MISRA15_int32_volatile_helper += 2;                       \n
Rules15.7+= 	}                                                                           }                                                             \n
Rules15.7+= 	/* Non-compliant - else statement is missing */                             else { ; }                                                    \n
Rules15.7+= }                                                                           }                                                                 \n"


Rules16.1= "Rule 16.1: All switch statements shall be well formed.
Rules16.1+=\n
Rules16.1+= Violation:                                                                  Solution: \n
Rules16.1+=**********************************************************************************************************************************************\n
Rules16.1+= static void MISRA16_1_func1( int32_t y)                                    static void MISRA16_1_func1( int32_t y)                \n
Rules16.1+={                                                                            {                                                     \n
Rules16.1+=   int32_t x=1;                                                                  int32_t x=1;                                      \n  switch(x)                                                 
Rules16.1+=                                                                       \t\t\t switch(x)                                         \n
Rules16.1+=   {                                                                             {                                                 \n   
Rules16.1+=       case 0:                                                                        case 0:                                      \n
Rules16.1+=              if(x==y)                                                                       if(x==y)                              \n
Rules16.1+=              {                                                                              {                                     \n
Rules16.1+=                goto L16;                                                                        goto L16;                            \n
Rules16.1+=              }                                                                              }                                     \n
Rules16.1+=               break;                                                           \t\t  L16:                     /*Compliant*/             \n
Rules16.1+=                                                                                              \t\t\t\t\t\t\t\t\t\t    ++x;                                \n
Rules16.1+=  L16:                          /*Non-Compliant*/                      \t\t    break;                              \n
Rules16.1+=               ++x;                                                                  \t\t default:                                     \n
Rules16.1+=               break;                                                                             break;                              \n
Rules16.1+=       default:                                                                                                                    \n
Rules16.1+=               break;                                                             \t\t}                                                \n
Rules16.1+= }                                                                                   MISRA16_int32_volatile_helper= x+y;           \n            
Rules16.1+=  MISRA16_int32_volatile_helper= x+y;                                          }                                                   \n
Rules16.1+= }                                                                                                                                 \n"


Rules16.2= "Rule 16.2: A switch label shall only be used when the most closely enclosing compound statement is the body of the switch statement.
Rules16.2+=\n
Rules16.2+= Violation:                                                                    Solution: \n     
Rules16.2+=****************************************************************************************************************\n
Rules16.2+=        static void MISRA16_1_func1 ( int32_t i )                                       static void MISRA16_1_func1 (int32_t i)    \n
Rules16.2+= {                                                                             {                                                   \n
Rules16.2+= 	int32_t x =1;                                                                     int32_t x =1;                               \nswitch ( x )
Rules16.2+=                                                                    \t\t\t\t\t\t\t\t\t\t switch ( x )                                      \n
Rules16.2+= 	{                                                                               {                                             \n
Rules16.2+= 		case 1: 		                                                                case 1:                                   \n
Rules16.2+= 			if ( i < 3 )                                                                       ;                                  \n
Rules16.2+= 			{                                                                           case 2:                                   \n
Rules16.2+= 		case 2: 			/* Non-compliant \t\t\t\t x=1;    /* Compliant*/              \n
Rules16.2+= 			x = 1;                                                                            break;                              \n
Rules16.2+= 			}                                                                           default:                                  \n
Rules16.2+= 			break;                                                                            break;                              \n
Rules16.2+= 	default:                                                                                                                      \n
Rules16.2+= 			break;                                                                   }                                            \n
Rules16.2+= 	}                                                                          }                                                  \n"


Rules16.3= "Rule 16.3: An unconditional break statement shall terminate every switch-clause. 
Rules16.3+=\n
Rules16.3+= Violation:                                                                      Solution: \n
Rules16.3+=**********************************************************************************************************************************************\n
Rules16.3+=static void MISRA16_3_func1 ( int32_t a, int32_t b )                             static void MISRA16_3_func1 ( int32_t a, int32_t b ) \n
Rules16.3+={                                                                                {                                                  \n
Rules16.3+= int 32_t x=1;                                                                     int 32_t x=1;                                    \n
Rules16.3+= switch ( x )                                                                      switch ( x )                                     \n
Rules16.3+={                                                                                {                                                  \n
Rules16.3+= case 0:                                                                           case 0:                                          \n
Rules16.3+=        break;                                                                            \tbreak;                                    \n
Rules16.3+= case 1:                                                                           case 1:                                          \n
Rules16.3+= case 4:                                                                           case 4:                                          \n
Rules16.3+=            a=b             /* Non- Compliant - break omitted*/                            \t  a=b;                                     \n
Rules16.3+= case 5:                                                                                  break;                                    \n
Rules16.3+=            if ( a==b )                                                            \t   case 5:                                          \n
Rules16.3+={                                                                                  if ( a==b )                               \n
Rules16.3+=        ++a;                                                                       \t{                                                \n
Rules16.3+=        break;  /* Non-Compliant - conditional break*/                                  \t ++a;                                      \n
Rules16.3+=}                                                                                  }                                                \n
Rules16.3+= default :                                                                          default:                                        \n
Rules16.3+=                ;      /*Non-compliant*/                                                   \t\t break;                                  \n
Rules16.3+=}                                                                                  }                                                \n
Rules16.3+= MISRA16_int32_volatile_helper =a;                                                  MISRA16_int32_volatile_helper =a;               \n
Rules16.3+=}                                                                                  }                                                \n"


Rules16.4="Rule 16.4: Every switch statement shall have a default label.
Rules16.4+=\n
Rules16.4+= Violation:                                                                         Solution: \n
Rules16.4+=**********************************************************************************************************************************************\n
Rules16.4+= static void MISRA16_4_func1 (void)                                                 static void MISRA16_4_func1 (void)             \n
Rules16.4+= {                                                                                  {                                              \n
Rules16.4+=	int16_t x = 1;                                                                       int16_t x = 1;                               \n
Rules16.4+=      switch( x )                                                                          switch( x )                             \n
Rules16.4+=      {                                                                                    {                                       \n
Rules16.4+=         case 0:                                                                               case 0:                             \n
Rules16.4+=                ++x;                                                                                  \t++x;                         \n
Rules16.4+=                break;                                                                                \tbreak;                       \n
Rules16.4+=          case 1:                                                                              case 1:                             \n
Rules16.4+=          case 2:                                                                              case 2:                             \n
Rules16.4+=                 break;                                                                               \tbreak;                       \n
Rules16.4+=     /* Non-compliant - default label is required */                                    default:                            \n
Rules16.4+=	}                                                                                                                                 \n
Rules16.4+= MISRA16_int32_volatile_helper = x;                                                      errorflag=1;                \n
Rules16.4+=	}                                                                                       break;                      \n
Rules16.4+=                                                                                            }                                      \n
Rules16.4+=                                                                                              \t\t\t\t\t\t\t\t\t\t\t MISRA16_int32_volatile_helper = x;  \n
Rules16.4+=                                                                                               \t\t\t\t\t\t\t\t\t\t\tMISRA16_int32-volatile_helper =errorflag; \n
Rules16.4+=                                                                                      	\t\t\t\t\t\t\t\t\t\t    }                                         \n   
Rules16.4+=                                                                                         \t\t\t\t\t\t\t\t\t\t\t/* Compliant - default label is  present*/ \n "


Rules16.5="Rule 16.5: A default label shall appear as either the first or the last switch label of a switch statement.
Rules16.5+=\n
Rules16.5+= Violation:                                                                            Solution: \n
Rules16.5+=**********************************************************************************************************************************************\n
Rules16.5+= switch ( x )                                                                           switch ( x )           \n        {                                                          
Rules16.5+=                                                                                        \t\t\t{ \n
Rules16.5+= 	 case 0:                                                                                 default: /* Compliant-default is the first label */ \n
Rules16.5+=	           ++x;                                                                          \t\t case 0:   \n
Rules16.5+=          break;                                                                                      ++x; \n
Rules16.5+=     default:  /* Non-compliant -default label is mixed with the case labels*/                   break; \n
Rules16.5+=          x=0;                                                                             \tcase 1: \n
Rules16.5+=      break;                                                                               \tcase 2: \n
Rules16.5+=      case 1:                                                                                     break;	\n
Rules16.5+=      case 2:                                                                                 } \n
Rules16.5+=            break;            \n
Rules16.5+=     \n
Rules16.5+=	}\n"


Rules16.6="Rule 16.6: Every switch statement shall have at least two switch-clauses. \n 
Rules16.6+=Violation:						Solution\n
Rules16.6+=***************************************************************************************************************\n
Rules16.6+= static void MISRA16_6_func1 (void)             				switch(z)\n
Rules16.6+= {                                                                                 {\n
Rules16.6+=	int32_t x = 0, y = 0, z = 0;                                                         case 1:\n
Rules16.6+=                                                                                                z=2;\n
Rules16.6+=	switch ( x )                                                                            break;\n
Rules16.6+=	{                                                                                    default:     /* Compliant*/ \n      
Rules16.6+=		default: 		/* Non-compliant - switch is redundant */                z=0; \n
Rules16.6+=			x = 0;                                                                                break;\n
Rules16.6+=			break;                                                                     \n
Rules16.6+=    }                                                                                    MISRA16_int32_volatile_helper=x;\n
Rules16.6+=	switch ( y )                                                                         MISRA16_int32_volatile_helper=y;\n
Rules16.6+=	{                                                                                    MISRA16_int32_volatile_helper=z;\n
Rules16.6+=		case 1:                                                                         }\n
Rules16.6+=		default: 		/* Non-compliant - switch is redundant */ \n
Rules16.6+=			y = 0;\n
Rules16.6+=			break;\n
Rules16.6+=	}\n"


Rules16.7="Rule 16.7: A switch expression shall not have essentially Boolean type. \n 
Rules16.7+= Violation:                                                                              Solution: \n
Rules16.7+= **********************************************************************************************************************************************\n
Rules16.7+=static void MISRA16_7_func1 (void)                                                       static void MISRA16_7_func1 (void) \n
Rules16.7+={                                                                                        {  \n
Rules16.7+=	int32_t x = 0, y = 0, z = 0;                                                              int32_t x = 0, y = 0, z = 0; \n
Rules16.7+=	switch ( x == 0 ) /* Non-compliant - essentially Boolean */                               switch(x) \n
Rules16.7+=	{                                                                                         { \n
Rules16.7+=		case 0:                                                                                   case 0:	\n           
Rules16.7+=			y = x;                                                                                     y = x; \n
Rules16.7+=			break;                                                                                     break;  \n                    
Rules16.7+=		default:                                                                                 default: \n
Rules16.7+=			y = z;                                                                                      y = z; \n 
Rules16.7+=			break;                                                                                      break; \n
Rules16.7+=	}                                                                                          }    \n       MISRA16_int32_volatile_helper = y;                          
Rules16.7+=                                                         \t\t\t  MISRA16_int32_volatile_helper = y; \n
Rules16.7+=}                                                                                          } \n"


Rules17.1="Rule 17.1: The features of <stdarg.h> shall not be used. \n
Rules17.1+=\n
Rules17.1+= Violation:                                                                                 \t\tSolution: \n
Rules17.1+=**********************************************************************************************************************************************\n
Rules17.1+=\#if defined(INDICATE_COMPILE_ERROR_MISRA17)								The features of <stdarg.h> are not defined and hence \n
Rules17.1+=static void MISRA17_1_func1 ( va_list ap ) /* Non-compliant */							should not be used. \n
Rules17.1+={ \n
Rules17.1+=	int32_t y; \n
Rules17.1+=	y = va_arg ( ap, int32_t ); 		/* Non-compliant */\n
Rules17.1+=}\n
Rules17.1+=static void MISRA17_1_func2 ( uint16_t n, ... )	\n
Rules17.1+={ \n
Rules17.1+=	uint32_t x;	\n
Rules17.1+=	va_list ap; 								/* Non-compliant */\n
Rules17.1+=	va_start ( ap, n ); 			/* Non-compliant */ \n
Rules17.1+=	x = va_arg ( ap, uint32_t ); 		/* Non-compliant */ \n
Rules17.1+=	MISRA17_1_func1 ( ap ); \n
Rules17.1+=	/* undefined - ap is indeterminate because va_arg used in MISRA17_1_func1 ( ) */ \n
Rules17.1+=	x = va_arg ( ap, uint32_t );		/* Non-compliant */ \n
Rules17.1+=	/* undefined - returns without using va_end ( ) */ \n
Rules17.1+=}\n
Rules17.1+=static void MISRA17_1_func3 ( void )  \n
Rules17.1+={\n
Rules17.1+=	/* undefined - uint32_t:double type mismatch when f uses va_arg ( ) */ \n
Rules17.1+=	MISRA17_1_func2 ( 1, 2.0, 3.0 );	\n
Rules17.1+=} \n
Rules17.1+=\# //INDICATE_COMPILE_ERROR_MISRA17" \n
	

Rules17.2="Rule 17.2: Functions shall not call themselves, either directly or indirectly.\n
Rules17.2+=Violation:														Solution\n
Rules17.2+=**********************************************************************************************************************************************\n
Rules17.2+=int pow(int num, int exponent)											int pow(int num, int exponent)\n	{										                          
Rules17.2+=																		{  \n
Rules17.2+=  if (exponent > 1) 														int val=num; \n 
Rules17.2+={																		    	\n
Rules17.2+=    num = num * pow(num, exponent-1);									\t\t\twhile (exponent>0) \n
Rules17.2+=  /* Non-compliant; direct recursion */									\t\t\t{   \n
Rules17.2+= }														\t\t\tval*=num; \n
Rules17.2+=  return num;											\t\t\t\t\t--exponent;  /* Compliant*/ \n
Rules17.2+=}												\t\t\t\t} \n
Rules17.2+=																				return val; \n
Rules17.2+=												}\n"


Rules17.3="Rule 17.3: A function shall not be declared implicitly.\n
Rules17.3+=VIolation:														Solution\n
Rules17.3+=**********************************************************************************************************************************************\n
Rules17.3+=\#include <limits.h>												\t\#include <limits.h>	\n	
Rules17.3+=\#include <stdio.h>												\#include <stdio.h> \n
Rules17.3+=foo(void) {														 unsigned int foo(void) {\n
Rules17.3+=  return UINT_MAX;												\treturn UINT_MAX;\n
Rules17.3+=}													 \t\t}	 \n
Rules17.3+=int main(void) {										 \t\t\tint main(void) {\n
Rules17.3+=  long long int c = foo();										\t	long long int c = foo();\n printf(\"%lld\", c);	 	
Rules17.3+= 						\t\t\t\t\t\t\t\t\t\t\tprintf(\"lld\",c);						\n
Rules17.3+=  return 0;															return 0;\n
Rules17.3+=}																}\n
Rules17.3+=/* The compiler assumes that foo() returns a value of type int for this noncompliant        \t\t\t\t The compliant solution explicitly defines the return type\n 
Rules17.3+=code example, UINT_MAX is incorrectly converted to -1. */								   of foo() as unsigned int .As a result the function correctly\n 
Rules17.3+=																							   returns UNIT_MAX.\n"


Rules17.4="Rule 17.4: All exit paths from a function with non-void return type shall have an explicit return statement with an expression.\n
Rules17.4+=Violation:														Solution\n
Rules17.4+=**********************************************************************************************************************************************\n
Rules17.4+=\#if defined(INDICATE_COMPILE_WARNING)							\#if defined(INDICATE_COMPILE_WARNING)\n
Rules17.4+=static int32_t MISRA17_4_func1 ( int32_t v )						static int32_t MISRA17_4_func1 ( int32_t v ) \n
Rules17.4+={												{\n
Rules17.4+=	if ( v < 0 )									\t\t\tif ( v < 0 ) \n
Rules17.4+=	{												{\n
Rules17.4+=		return v;										\t\t\treturn v;\n
Rules17.4+=	}												}\n
Rules17.4+=	/* Non-compliant - control can reach this point without returning a value */					else \n 
Rules17.4+=}												{ \n
Rules17.4+=\# //INDICATE_COMPILE_WARNING									return 12;\n
Rules17.4+=																								\t\t\t\t\t\t\t\t\t\t\t\t}  /* Compliant*/ \n
Rules17.4+=																	\t\t\t\t\t\t\t\t\t\t\t\# //INDICATE_COMPILE_WARNING\n"


Rules17.5="Rule 17.5: The function argument corresponding to a parameter declared to have an array type shall have an appropriate number of elements.\n
Rules17.5+=Violation:										\t\tSolution\n
Rules17.5+=**********************************************************************************************************************************************\n
Rules17.5+=Intent is that function does not access outside the range array1 [ 0 ] .. array1[ 3 ]       \t\tint32_t arr4[ 4 ] = { 0, 1, 2, 3 };\n
Rules17.5+=static void MISRA17_5_func1 ( int32_t array1[ 4 ] )					\t\t/* Compliant-size of the array  matches the prototype */\n
Rules17.5+={														MISRA17_5_func1 ( arr3 );\n
Rules17.5+=	array1[1] = array1[2] +2;							\t\t\t/* Compliant only if ptr points to at least 4 points */\n
Rules17.5+=}														MISRA17_5_func1 ( ptr );\n
Rules17.5+=/* Intent is that function handles arrays of any size */			 				 /* Compliant */\n
Rules17.5+=static void MISRA17_5_func2 ( int32_t array2[ ] )\n
Rules17.5+={\n
Rules17.5+=	array2[1] = array2[2] +2;\n
Rules17.5+=}\n
Rules17.5+=static void MISRA17_5_func3 ( int32_t *ptr )\n
Rules17.5+={\n
Rules17.5+=	int32_t arr3[ 3 ] = { 1, 2, 3 };\n
Rules17.5+=	MISRA17_5_func1 ( arr4 );\n
Rules17.5+=	/* Non-compliant - size of array does not match prototype */ \n"


Rules17.6="Rule 17.6: The declaration of an array parameter shall not contain the static keyword between the [ ] .\n
Rules17.6+=Violation:											\t\t\t\t\tSolution\n
Rules17.6+=*********************************************************************************************************************************************************************\n
Rules17.6+=static uint16_t MISRA17_6_func1( uint16_t n, uint16_t const a[ static 6 ] )		 \t\t\tstatic uint16_t MISRA17_6_func1( uint16_t n, uint16_t const a[ 6 ] )\n
Rules17.6+=  /* Non-Compliant */																/* Compliant */\n
Rules17.6+={															\t\t{\n
Rules17.6+=	uint16_t i;																		uint16_t i;\n
Rules17.6+=	uint16_t sum = 0U;																uint16_t sum = 0U;\n
Rules17.6+= 	for ( i = 0U; i < n; ++i )														for ( i = 0U; i < n; ++i )\n
Rules17.6+=	{														\t\t	{\n
Rules17.6+=		sum = sum + a[ i ];																sum = sum + a[ i ];\n
Rules17.6+=	}															\t\t}\n
Rules17.6+=	return sum;																		return sum;\n
Rules17.6+=}															\t\t}\n"


Rules17.7="Rule 17.7:The value returned by a function having non-void return type shall be used.\n
Rules17.7+=Violation:																Solution\n
Rules17.7+=**********************************************************************************************************************************************\n
Rules17.7+=Static uint16_t MISRA17_7_func1( uint16_t para1)							Static uint16_t MISRA17_7_func1( uint16_t para1)\n
Rules17.7+={													{\n
Rules17.7+= MISRA17_uint16_volatile_helper += 2u;										uint16_t x;\n
Rules17.7+=return para1;										    		  MISRA17_7_func1 (para2);\n
Rules17.7+=} 													  ( void ) MISRA17_7_func1 (para2); /*Compliant*/\n         
Rules17.7+=/* Non-Compliant- The value returned by the function is not used */			\t   x = MISRA17_7_func1 (para2); /*Compliant*/\n
Rules17.7+=																				MISRA17_uint16_volatile_helper = x;\n
Rules17.7+=																			\t\t\t\t\t\t\t\t\t\t\t\t\t}\n"



Rules17.8="Rule 17.8:A function parameter should not be modified.\n
Rules17.8+=Violation:														Solution:\n
Rules17.8+=**********************************************************************************************************************************************\n
Rules17.8+=static void MISRA17_8_func2 ( int32_t *p, int32_t *q )								static void MISRA17_8_func2 ( int32_t *p, int32_t *q )\n
Rules17.8+={															{\n
Rules17.8+=	p = q; 	/* Non-compliant - but Rule 17.8 is switched off  */							*p=*q;   /* Compliant */\n
Rules17.8+=}															}\n"


Rules18.1="Rule 18.1:A pointer resulting from arithmetic on a pointer operand shall address an element of the same array as that pointer operand.\n
Rules18.1+=Violation:												Solution\n
Rules18.1+=**********************************************************************************************************************************************\n
Rules18.1+=static void MISRA18_1_func3 ( void )									static void MISRA18_1_func3 ( void )\n
Rules18.1+={													{\n
Rules18.1+=	struct													    struct\n
Rules18.1+=	{																	{\n
Rules18.1+=		uint16_t x;														uint16_t x;\n
Rules18.1+=		uint16_t y;														uint16_t y;\n
Rules18.1+=		uint16_t z;														uint16_t z;\n
Rules18.1+=		uint16_t a[ 10 ];												\tuint16_t a[ 10 ];\n
Rules18.1+=	} s;											\t\t} s;\n
Rules18.1+=	uint16_t *p;														 uint16_t *p;\n
Rules18.1+=	p = &s.x;													        p = &s.x;\n
Rules18.1+=	p[ 0 ] = 1; /* Non-compliant - undefined, dereference of address 		\t\t\t\t\tp = &s.a[ 0 ]; 	  /* Compliant - p points into s.a */\n
Rules18.1+=				one beyond s.x which is not necessarily the same as s.y */	\t\t\t\t\t\t\tp = p + 8; 	 /* Compliant - p still points into s.a */\n
Rules18.1+=	p[ 1 ] = 2; 	 /* Non-compliant - undefined */						\t\t++p; 		 /* Compliant - p points one beyond s.x */\n
Rules18.1+=	p = p + 3; 	/* Non-compliant-undefined,p points more than one beyond s.a */\n } 
Rules18.1+=	MISRA18_uint16_volatile_helper = s.y;\n
Rules18.1+=	MISRA18_uint16_volatile_helper = s.z;\n
Rules18.1+=	MISRA18_uint16_volatile_helper = *( p - 2 );\n
Rules18.1+=} \t\t\t\t\t\t\t\t\t\t\t\t\t}\n"


Rules18.2="Rule 18.2:Subtraction between pointers shall only be applied to pointers that address elements of the same array.\n
Rules18.2+=Violation:																Solution\n
Rules18.2+=**********************************************************************************************************************************************\n
Rules18.2+=	static void MISRA18_2_func1 ( int32_t const *ptr )						static void MISRA18_2_func1 ( int32_t const *ptr )\n
Rules18.2+={													{\n
Rules18.2+=	int32_t a1[ 10 ];											    int32_t a1[ 10 ];\n
Rules18.2+=	int32_t a2[ 10 ];											    int32_t a2[ 10 ];\n
Rules18.2+=	int32_t const *p1 = &a1[ 1 ];											int32_t const *p1 = &a1[ 1 ];\n
Rules18.2+=	int32_t const *p2 = &a2[ 10 ];											int32_t const *p2 = &a2[ 10 ];\n
Rules18.2+=	int32_t diff;											    	int32_t diff;\n
Rules18.2+=	MISRA18_int32_volatile_helper = diff;									  MISRA18_int32_volatile_helper = diff;\n
Rules18.2+=	diff = p1 - p2;   /* Non-compliant */									  diff = p1 - a1;   /* Compliant */\n
Rules18.2+=	MISRA18_int32_volatile_helper = diff;									  MISRA18_int32_volatile_helper = diff;\n
Rules18.2+=	diff = ptr - p1; /* Non-compliant */									  diff= p2-a2; /* Compliant */\n
Rules18.2+=	MISRA18_int32_volatile_helper = diff;									}\n
Rules18.2+=} \n"


Rules18.3="Rule 18.3:The relational operators >, >=, < and <= shall not be applied to objects of pointer type except where they point into the same object.\n
Rules18.3+=Violation:														 Solution:\n
Rules18.3+=**********************************************************************************************************************************************\n
Rules18.3+=static void MISRA18_3_func1 ( void )								static void MISRA18_3_func1 ( void )\n
Rules18.3+={																{\n
Rules18.3+=	int32_t a1[ 10 ];												int32_t a1[ 10 ];\n
Rules18.3+=	int32_t a2[ 10 ];												int32_t a2[ 10 ];\n
Rules18.3+=	int32_t const *p1 = a1;											int32_t const *p1 = a1;\n
Rules18.3+=	if ( p1 < a2 ) 	/* Non-compliant */								if ( p1 < a1 )\n
Rules18.3+=	{														{\n
Rules18.3+=	}														}\n
Rules18.3+=	if ( p1 >= a2 ) /* Non-compliant */							\t\tif ( p1 >= a1 ) /* Compliant */\n
Rules18.3+=	{														{\n
Rules18.3+=	}														}\n
Rules18.3+=	a1[1] = 1;														 a1[1] = 1;\n
Rules18.3+=	a2[1] = 1;														 a2[1] = 1;\n
Rules18.3+=	MISRA18_int32_volatile_helper = a1[1] + a2[1];					 \t\t\t\tMISRA18_int32_volatile_helper = a1[1] + a2[1];\n
Rules18.3+=}															}\n"


Rules18.4="Rule 18.4: The +, -, += and -= operators should not be applied to an expression of pointer type.\n
Rules18.4+=Violation:												 Solution:\n
Rules18.4+=**********************************************************************************************************************************************\n
Rules18.4+=static void MISRA18_4_func1 ( void )						\t\tstatic void MISRA18_4_func1 ( void )\n
Rules18.4+={															{\n
Rules18.4+=	uint16_t a[ 10 ];										  uint16_t a[ 10 ];\n
Rules18.4+=	uint16_t *ptr;											  uint16_t *ptr;\n
Rules18.4+=	uint16_t index = 0U										  uint16_t index = 0U;\n
Rules18.4+=	*( ptr + 5 ) = 0U; 	/* Non-compliant - but Rule 18.4 is switched off  */         	  \t\t index= index + 1U; /*Compliant*/\n
Rules18.4+=}											\t\ta[index]= 0U;    /*Compliant*/\n
Rules18.4+=}														}\n"


Rules18.5="Rule 18.5:Declarations should contain no more than two levels of pointer nesting.\n
Rules18.5+=Violation: 																				Solution\n
Rules18.5+=**************************************************************************************************************************************************************************************\n
Rules18.5+=typedef int32_t * INTPTR;																typedef int32_t * INTPTR;\n
Rules18.5+=static void MISRA18_5_func1( int32_t ** arrPar[ ] )/* Non-compliant */					\t\t\t\t\t\tstatic void MISRA18_5_func1()\n
Rules18.5+={																	{\n
Rules18.5+=	int32_t x = 1;																		int32_t x = 1;\n
Rules18.5+=	int32_t *px = &x;																	int32_t *px = &x;\n
Rules18.5+=	int32_t a[3] = { 1, 1, 1 };																int32_t a[3] = { 1, 1, 1 };\n
Rules18.5+=	int32_t *pa = &a[0];																	int32_t *pa = &a[0];\n
Rules18.5+=	int32_t *** obj3 = &obj2; 		/* Non-compliant */											\tint32_t ** obj2 = &px; /* Compliant */\n
Rules18.5+=	INTPTR * const * const obj5 = &obj4;	/* Non-compliant */									\t\t\tINTPTR * obj4 = &px;; 	/* Compliant */\n
Rules18.5+=	MISRA18_int32_volatile_helper = ***obj3;													\tint32_t ** arr[ 3 ] = { &pa, &pa, &pa }; 	/* Compliant */\n
Rules18.5+=	MISRA18_int32_volatile_helper = **obj2;														int32_t ** ( *parr )[ 3 ]; 	/* Compliant */\n
Rules18.5+=	MISRA18_int32_volatile_helper = **obj4;														int32_t * ( **pparr )[ 3 ]; /* Compliant */\n
Rules18.5+=	MISRA18_int32_volatile_helper = ***obj5;												}\n
Rules18.5+=	MISRA18_int32_volatile_helper = **arr[0];      /* Non-Compliant*/\n
Rules18.5+=}\n"


Rules18.6="Rule 18.6:The address of an object with automatic storage shall not be copied to another object that persists after the first object has ceased to exist.\n
Rules18.6+=Violation:																				Solution\n
Rules18.6+=****************************************************************************************************************************************************************************\n
Rules18.6+=\#if defined(INDICATE_COMPILE_WARNING)													The address of an object becomes indeterminate when the\n
Rules18.6+=static int32_t *MISRA18_6_func1 ( void )											\tlifetime of that object expires. Any use of an\n
Rules18.6+={															\t\tindeterminate address results in undefined behaviour.\n
Rules18.6+=	int32_t local auto;\n
Rules18.6+=return &local auto; /* Non-compliant - & local auto is indeterminate\n
Rules18.6+=}						when MISRA18_6_func1 returns */\n
Rules18.6+=\# //INDICATE_COMPILE_WARNING\n
Rules18.6+=static int32_t *sp1;\n
Rules18.6+=static void MISRA18_6_func4 ( void )\n
Rules18.6+={\n
Rules18.6+=	static const int32_t *q;\n
Rules18.6+=	int32_t x = 0;\n
Rules18.6+=	q = &x; /* Non-compliant - &x stored in object with greater lifetime */\n
Rules18.6+=	MISRA18_int32_volatile_helper = *q;\n
Rules18.6+=	MISRA18_int32_volatile_helper = *sp1;\n
Rules18.6+=}\n


Rules18.7="Rule 18.7:Flexible array members shall not be declared.\n
Rules18.7+=Violation:																				Solution\n
Rules18.7+=******************************************************************************************************************************************************************************************************\n
Rules18.7+=struct s																	If a flexible array member is declared the array member\n
Rules18.7+={										\t\t						will modify the behavior of size of operator in\n
Rules18.7+=	uint32_t len;								\t\t\t\t\t				different ways that might affect the program.\n
Rules18.7+=	uint32_t data[]; 	/* Non-compliant - flexible array member */\n
Rules18.7+=};\n
Rules18.7+=\#if defined(INDICATE_COMPILE_ERROR_MISRA18)\n
Rules18.7+=static void MISRA18_7_func1 ( const struct s *s1 )\n
Rules18.7+={\n
Rules18.7+=	struct s stru1 = { 1, { 1, 1} };\n
Rules18.7+=	struct s *s2 = &stru1;\n
Rules18.7+=	uint32_t size;\n
Rules18.7+=	stru1.len = 1;\n
Rules18.7+=	size = sizeof ( struct s ) + ( s1->len * sizeof ( uint32_t ) );\n
Rules18.7+=	*s2 = *s1; 			/* Only copies s1->len */\n
Rules18.7+=	MISRA18_int32_volatile_helper = (int32_t) size;\n
Rules18.7+=	MISRA18_int32_volatile_helper = (int32_t) stru1.data[0];\n
Rules18.7+=}\n
Rules18.7+=\# \n"


Rules18.8="Rule 18.8:Variable-length array types shall not be used.\n
Rules18.8+=Violation:																				Solution\n
Rules18.8+=*********************************************************************************************************************************************************************************************\n
Rules18.8+=static void MISRA18_8_func1 ( int32_t n )											static void MISRA18_8_func1 ( int32_t n )																	\n
Rules18.8+={																{							\n
Rules18.8+=	uint16_t x = 10;														uint16_t x = 10;						\n
Rules18.8+=	uint32_t vla[ n ]; 	/* Non-compliant - Undefined if n <= 0 */				\t\t		uint32_t vla[ 4 ];	/*Compliant*/					\n
Rules18.8+=	uint32_t vlb[n][x];  /* Non-compliant - Undefined if n <= 0 or x <=0 */			\t\t\t	uint32_t vlb[3][4];	/*Compliant*/					\n
Rules18.8+=	vla[0] = 1;\n																vla[0] = 1;								\n
Rules18.8+=	MISRA18_int32_volatile_helper = (int32_t) vla[0];										MISRA18_int32_volatile_helper = (int32_t) vla[0];\n	
Rules18.8+=}														       }			\n
Rules18.8+=\# //INDICATE_COMPILE_WARNING\n"


Rules19.1="Rule 19.1:An object shall not be assigned or copied to an overlapping object.\n
Rules19.1+=VIolation:																			Solution\n
Rules19.1+=***************************************************************************************************************************************************************************\n
Rules19.1+=static void MISRA19_1_func1 ( void )													\t\tstruct name\n
Rules19.1+={																		\t{\n
Rules19.1+=	union																\t\t\tint16_t i;\n
Rules19.1+=	{	int16_t i;														\t\t\tint32_t j;\n
Rules19.1+=		int32_t j;																		} a={0},b={1};\n
Rules19.1+=	} a = { 0 }, b = { 1 };  /* also violates Rule 19.2 because it uses unions */			\t\t\t\t\t\t\ta=b;\n
Rules19.1+=	a.j = a.i; 															\t\t\t*/ Compliant -exception 1 */\n
Rules19.1+=	/* Non-compliant */													\t\t\t	static int16_t array[5];\n
Rules19.1+=	static int16_t array[5];													\t\t	int16_t* array_pointer = &array[3];\n
Rules19.1+=	int16_t* array_pointer = &array[3];  									\t\t\t			/* Compliant, index inside of array size */\n
Rules19.1+=	array[0] = *array_pointer;																array[0] = *array_pointer;\n
Rules19.1+=	MISRA19_int16_volatile_helper = a.i;										\t\t			MISRA19_int16_volatile_helper = a.i;\n
Rules19.1+=}																\t\t\t}\n"


Rules19.2="Rule 19.2:The union keyword should not be used.\n
Rules19.2+=Violation:																			Solution\n
Rules19.2+= ***************************************************************************************************************************************************************************************\n
Rules19.2+= static int32_t MISRA19_2_func1 ( int16_t s )											A union is a data type that allows to store different\n
Rules19.2+= {																data types in the same memory location. You can define a\n
Rules19.2+= 	union										\t\t				union with many members, but only one member can contain a\n
Rules19.2+= 	{				       /* Non-compliant */		\t\t					value at any given time.\n
Rules19.2+= 		int32_t ul;\n
Rules19.2+= 		int16_t us;\n
Rules19.2+= 	} tmp;\n
Rules19.2+= 	tmp.us = s;\n
Rules19.2+= 	return tmp.ul;        /* unspecified value */\n
Rules19.2+= }\n"


Rules20.1="Rule 20.1:\#include directives should only be preceded by preprocessor directives or comments.\n
Rules20.1+=Violation:													\t\tSolution\n
Rules20.1+=*****************************************************************************************************************************************************************************\n
Rules20.1+=\#define MACRO_20_1   2;													\#define MACRO_20_1   2;\n
Rules20.1+= static volatile int16_t MISRA20_int16_volatile_helper = 1;						\t\t	\#include "MISRA17.h"	/*Compliant*/\n
Rules20.1+=\#include "Misra18.h"	/* Non-compliant - but Rule 20.1 is switched off */				\t\t\tstatic volatile int16_t MISRA20_int16_volatile_helper = 1;\n
Rules20.1+=static void MISRA20_1_func2 (void)											\tstatic void MISRA20_1_func2 (void)\n
Rules20.1+={															{\n
Rules20.1+=	MISRA20_int16_volatile_helper = MACRO_20_1;						\t\t\t\t	MISRA20_int16_volatile_helper = MACRO_20_1;\n
Rules20.1+=}															}\n"


Rules20.2= "Rule 20.2:The ' , \" or \ charactes and comment character sequences shall not occur in a header file name used between delimiters.\n
Rules20.2+=Violation:															Solution\n
Rules20.2+=*********************************************************************************************************************************************\n
Rules20.2+=\#if defined(INDICATE_COMPILE_ERROR_MISRA20)                                        				 \#include \"Misra5.h\" 	/* Compliant */	\n
Rules20.2+=\#include \"Misr'a19.h\"	      /* Non-compliant */					                \#include ^<Misra5.h^>  /* Compliant */	\n
Rules20.2+=\#include \"Misra6.h\"			 /* Non-compliant */ \n
Rules20.2+=\#include \"Misra//15.h\"	 	/* Non-compliant */   \n
Rules20.2+=\#include \"Misra/*4.h\"	   /* Non-compliant */			\n
Rules20.2+=\#include ^<Misra//1.h^>	 /* Non-compliant */ \n"

Rules20.3="Rules 20.3:The \#include directive shall be followed by either a <filename> or “filename“ sequence.\n
Rules20.3+=Violation:															Solution\n
Rules20.3+=********************************************************************************************************************************************************************************\n
Rules20.3+=\#if defined(INDICATE_COMPILE_ERROR_MISRA20)											\#include "Misra16.h" 	/* Compliant */\n
Rules20.3+=\#include Misra16.h 		/* Non-compliant */										 \#include <Misra16.h>   /* Compliant */\n
Rules20.3+=\# //INDICATE_COMPILE_ERROR_MISRA20									\t\t	\#include HEADER 	   /* Compliant */\n
Rules20.3+=\#define HEADER "Misra14.h"\n
Rules20.3+=\#if defined(INDICATE_COMPILE_ERROR_MISRA20)\n
Rules20.3+=\#define FILENAME Misra13.h\n
Rules20.3+=\#include FILENAME 		/* Non-compliant */\n
Rules20.3+=\# //INDICATE_COMPILE_ERROR_MISRA20 \n"


Rules20.4="Rules 20.4:A macro shall not be defined with the same name as a keyword.\n
Rules20.4+=Violation:																			Solution\n
Rules20.4+=*********************************************************************************************************************************************************************************\n
Rules20.4+=\#if defined(INDICATE_COMPILE_ERROR_MISRA20)											\#define seq( S1, S2 )\n
Rules20.4+=\#define int some_other_type              /* Non-compliant - redefined int */			\t\t\t\tdo { (S1); (S2); } while ( false ) /* Compliant */\n
Rules20.4+=\#include <stdlib.h>														\#define compound( S ) { (S); } 	  /* Compliant */\n
Rules20.4+=\# //INDICATE_COMPILE_ERROR_MISRA20\n
Rules20.4+=static void MISRA20_4_func1( void )										\t\tstatic void MISRA20_4_func1( void )\n
Rules20.4+={																{\n
Rules20.4+=	int16_t x = 1, y = 0;							\t						int16_t x = 1, y = 0;\n
Rules20.4+=	seq( x++, y++ );														seq( x++, y++ );\n
Rules20.4+=	compound( x++ )														compound( x++ )\n
Rules20.4+=	MISRA20_int16_volatile_helper = x + y;									\t\t\tMISRA20_int16_volatile_helper = x + y;\n
Rules20.4+=}																}\n
Rules20.4+=\#define while( E ) for ( ; ( E ) ; ) 	/* Non-compliant - redefined while */			\t\t\t\#define unless ( E ) if ( ! ( E ) ) /* Compliant */\n
Rules20.4+=static void MISRA20_4_func2( _Bool i )\n
Rules20.4+={\n
Rules20.4+=	unless( i ) { }\n
Rules20.4+=	MISRA20_int16_volatile_helper += 2;\n
Rules20.4+=}\n"


Rules20.5="Rules 20.5:\#undef should not be used.\n
Rules20.5+=Violation:																\tSolution\n
Rules20.5+=************************************************************************************************************************************************************************************************\n
Rules20.5+=\#define QUALIFIER volatile															\#define QUALIFIER volatile\n
Rules20.5+=static void MISRA20_5_func1 ( QUALIFIER int32_t p )								\t\t\t	static void MISRA20_5_func1 ( QUALIFIER int32_t p )\n
Rules20.5+={																	{\n
Rules20.5+=	while ( p != 0 )													\t\t	while ( p != 0 )\n
Rules20.5+=	{																			{\n
Rules20.5+=		; 		/* Wait... */													\t\t	; 		/* Wait... */\n
Rules20.5+=	}																			}\n
Rules20.5+=}																	}\n
Rules20.5+=\#undef QUALIFIER  /* Non-compliant */													\t\#define QUALIFIER /* Compliant */  \n"


Rules20.6="Rules 20.6:Tokens that look like a preprocessing directive shall not occur within a macro argument.\n
Rules20.6+=Violation:																Solution\n
Rules20.6+=**************************************************************************************************************************************************************************************************\n
Rules20.6+=#define MM( AAA ) MISRA20_6_func1 ( AAA )											#define MM( AAA ) MISRA20_6_func1 ( AAA )\n
Rules20.6+=static void MISRA20_6_func1(const char *a)											static void MISRA20_6_func1(const char *a)\n
Rules20.6+={																{\n
Rules20.6+=	MISRA20_int16_volatile_helper = (int16_t) *a;											MISRA20_int16_volatile_helper = (int16_t) *a;\n
Rules20.6+=}																}\n
Rules20.6+=static void MISRA20_6_func2 ( void )									\t\t\tstatic void MISRA20_6_func2 ( void )\n
Rules20.6+={																{\n
Rules20.6+=	MM (																	MM (\n
Rules20.6+=\#ifdef SW 					/* Non-compliant */								\#if  defined (macro1)  || !defined (macro2) || defined (macro3)\n
Rules20.6+=			\"Message 1\"													\t	printf( \"Hello!\" );         /*Compliant*/\n
Rules20.6+=\#else 						/* Non-compliant */								\#endif\n
Rules20.6+=			\"Message 2\"															}					\n
Rules20.6+=\#endif 				\t/* Non-compliant */\n
Rules20.6+=	);\n
Rules20.6+=}\n"



Rules20.7="Rules 20.7:Expressions resulting from the expansion of macro parameters shall be enclosed in parentheses.\n
Rules20.7+=Violation:																	Solution\n
Rules20.7+=*************************************************************************************************************************************************************************\n
Rules20.7+=\#define M1( x, y ) ( x * y ) 															\#define M1( x, y ) ( x * y )\n
Rules20.7+=static void MISRA20_7_func1(void)														static void MISRA20_7_func1(void)\n
Rules20.7+={																		{\n
Rules20.7+=	int32_t r;																	int32_t r;\n
Rules20.7+=	r = M1 ( 1 + 2, 3 + 4 ); /* Non-compliant -r = ( 1 + 2 * 3 + 4 ) which is 11,				\t\t\t\t\tMISRA20_int16_volatile_helper = (int16_t) r;\n
Rules20.7+=	 	 	 	 	 	* whereas the result 21 might have been expected */						\t\t\t\t\t\tr = M1 (  (1 + 2) ,  (3 + 4)) /*Compliant*/\n
Rules20.7+=	MISRA20_int16_volatile_helper = (int16_t) r;							\t\t				}\n
Rules20.7+=}\n"


Rules20.8="Rule 20.8:The controlling expression of a \#if or \#elif preprocessing directive shall evaluate to 0 or 1.\n
Rules20.8+=Violation:																	Solution\n
Rules20.8+=********************************************************************************************************************************************************************************\n
Rules20.8+=\#if 10 					/* Non-compliant */									\t\#define APPLFALSE 	0\n
Rules20.8+=\#																		\#if APPLFALSE 			     /* Compliant */\n
Rules20.8+=\#if x > 0 																	\#if A > B /* Compliant assuming A and B are numeric */\n
Rules20.8+=/* x assumed to be zero if not defined */									\t\t\t\t\#\n
Rules20.8+=\# \n"


Rules20.9="Rule 20.9:All identifiers used in the controlling expression of \#if or \#elif preprocessing directives shall be \#define’d before evaluation.\n
Rules20.9+=Violation:														\t\tSolution\n
Rules20.9+=******************************************************************************************************************************************************************************\n
Rules20.9+=\#if N == 0															\#if defined ( P ) /* Compliant - P is not evaluated */\n
Rules20.9+=	/* Non-compliant - does 'N' expand to zero or is it undefined? */		\t\t				\#if P == 0  /* Compliant - P is known to be defined */\n
Rules20.9+=\#																\#\n
Rules20.9+=\#if M>0\n
Rules20.9+=     /* Non-Compliant- M is undefined. */\n
Rules20.9+=\# \n"


Rules20.10="Rule 20.10: The \# and \#\# preprocessor operators should not be used.\n
Rules20.10+=Violation:																	Solution\n
Rules20.10+=**************************************************************************************************************************************************************************\n
Rules20.10+=\#define x1( x, y ) \#x \#\# \#y 		/* Non-Compliant */									\#define x1( x, y ) x \#\# y 		/* Compliant */\n
Rules20.10+=static void MISRA20_10_func ( void )												static void MISRA20_10_func ( void )\n
Rules20.10+={    int16_t y1 = x1( 2, 3 );													{    int16_t y1 = x1( 2, 3 );\n
Rules20.10+=	const char *y2;														 const char *y2;\n
Rules20.10+=	y2 = ( x2( 2, 3 ) );														 y2 = ( x2( 2, 3 ) );\n
Rules20.10+=	MISRA20_int16_volatile_helper = y1;												 MISRA20_int16_volatile_helper = y1;\n
Rules20.10+=	MISRA20_int16_volatile_helper = (int16_t) *y2;											 MISRA20_int16_volatile_helper = (int16_t) *y2;\n
Rules20.10+=}																}\n"


Rules20.11="Rule 20.11:A macro parameter immediately following a \# operator shall not immediately be followed by a \#\# operator *.\n
Rules20.11+=Violation:																	Solution\n
Rules20.11+=*******************************************************************************************************************************************************************************\n
Rules20.11+=\#define C2( x, y ) \#x \#\# y 		/* Non-compliant */										\#define A2( x ) \#x 				/* Compliant */\n
Rules20.11+=static void MISRA20_11_func1 ( void )													\#define B2( x, y ) x \#\# y 		     /* Compliant */\n
Rules20.11+={																	static void MISRA20_11_func1 ( void )\n
Rules20.11+=	int16_t z = B2( 2, 3 );															{\n
Rules20.11+=	const char *w;																int16_t z = B2( 2, 3 );\n
Rules20.11+=	w = ( A2( 2 ) );															const char *w;\n
Rules20.11+=	MISRA20_int16_volatile_helper = z;													w = ( A2( 2 ) );\n
Rules20.11+=	MISRA20_int16_volatile_helper = (int16_t) *w;												MISRA20_int16_volatile_helper = z;\n
Rules20.11+=}																	MISRA20_int16_volatile_helper = (int16_t) *w;\n
Rules20.11+=																						\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t}\n"


Rules20.12="Rule 20.12:A macro parameter used as an operand to the \# or \#\# operators, which is itself subject to further macro replacement,\n
Rules20.12+=shall only be used as an operand to these operators .\n
Rules20.12+=Violation:																	Solution\n
Rules20.12+=*************************************************************************************************************************************************************************************\n
Rules20.12+=\#define AA 0xffff												\#define SCALE( X ) ( ( X ) * X \#\# _scale )\n
Rules20.12+=\#define BB( x ) (( x ) + wow \#\# x) 		/* Non-compliant */						static void MISRA20_12_func2 ( void )\n
Rules20.12+=static void MISAR20_12_func1 ( void )										{\n
Rules20.12+={														int32_t speed = 1;\n
Rules20.12+=	int32_t wowAA = 1;												int32_t speed_scale = 1;\n
Rules20.12+=	wowAA = BB ( AA );												int32_t scaled_speed;\n
Rules20.12+=/* Expands as wowAA = ( 0xffff ) + wowAA; */									scaled_speed = SCALE ( speed ) ;\n
Rules20.12+=/ * Non-Compliant */												}\n
Rules20.12+=	MISRA20_int16_volatile_helper = (int16_t) wowAA;							/* Compliant - expands to scaled_speed =\n
Rules20.12+=}													( ( speed ) * speed_scale ); */\n
Rules20.12+=																									MISRA20_int16_volatile_helper = (int16_t) scaled_speed;\n
Rules20.12+=												}\n


Rules20.13="Rule 20.13:A line whose first token is \# shall be a valid preprocessing directive.\n
Rules20.13+=Violation:																Solution\n
Rules20.13+=**********************************************************************************************************************************************************************************\n
Rules20.13+=\#define AAA 2															\#define AAA 2\n
Rules20.13+=int foo(void)															int foo(void)\n
Rules20.13+={																	{\n
Rules20.13+= int x = 0;																 int x = 0;\n
Rules20.13+=\#ifndef AAA															 \#ifndef AAA\n
Rules20.13+=  x = 1;																\tx = 1;\n
Rules20.13+=\#else1  /* Non-Compliant */                                                    \t\t\t\t\t          \#else /* Compliant*/\n
Rules20.13+=  x = AAA;															\tx = AAA;\n
Rules20.13+=\#endif															\t \#endif\n
Rules20.13+= return x;															\treturn x;\n
Rules20.13+=}															\t\t}\n


Rules20.14="Rule 20.14:All \#else, \#elif and \# preprocessor directives shall reside in the same file as the \#if, \#ifdef or \#ifndef directive\n
Rules20.14+=to which they are related.\n
Rules20.14+=Violation:																	Solution\n
Rules20.14+=******************************************************************************************************************************************************************************\n
Rules20.14+=\#include "Misra11.h"														  \#ifdef A /* Compliant */\n
Rules20.14+=\#endif																 \#include "Misra11.h"\n
Rules20.14+=/* Because of missing \# fatal error occurs while compilation*/			\t\t\t\t\t        \#endif \n
Rules20.14+=   Uncomment the code to check the rule.  */								 \t\t\t \#include "Misra20.h"\n
Rules20.14+= //\#if 1 	/* Non-compliant as the \# is present in "Misra20.h" */		  \t\t\t\t\t	\#endif\n
Rules20.14+= //\#include "Misra20.h"											  \t\t\t//INDICATE_COMPILE_ERROR_MISRA20\n
Rules20.14+=\# //INDICATE_COMPILE_ERROR_MISRA20 \n"


Rules21.1="Rule 21.1:\#define and \#undef shall not be used on a reserved identifier or reserved macro name.\n
Rules21.1+=Violation:																		Solution\n
Rules21.1+=***********************************************************************************************************************************************************************************\n
Rules21.1+=  \#if defined(INDICATE_COMPILE_ERROR_MISRA21)											\#define isneg( x ) ( ( x ) < 0 )\n
Rules21.1+=\#define defined 1		/* Non-compliant - reserved identifier */			\t			/* Compliant - rule doesn't include future library directions */\n
Rules21.1+=\# //INDICATE_COMPILE_ERROR_MISRA21										\t\t\#define LINE 1  /*Compliant*/\n
Rules21.1+=\#define __builtin_sqrtf 2\n
Rules21.1+=/* Non-compliant - the implementation may use __builtin_sqrtf for other purposes\n
Rules21.1+= 	 * e.g. generating a sqrt instruction */\n
Rules21.1+=\#define errno my_errno 	/* Non-compliant - library identifier */ \n"


Rules21.2="Rules 21.2:A reserved identifier or macro name shall not be declared.\n
Rules21.2+=Violation:																	Solution\n
Rules21.2+=************************************************************************************************************************************************************************************\n
Rules21.2+=int8_t _BUILTIN_sqrt = 1;			/* Non-compliant */   								\#ifndef MY_FILE\n
Rules21.2+=int8_t __SIZE_MAX = 10;				/* Non-compliant */							\#define MY_FILE\n
Rules21.2+=int8_t _Areserve = 				/* Non-compliant */								int clean(void *pArg, int len)\n
Rules21.2+=																									\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t{\n
Rules21.2+=																									\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tint i;\n
Rules21.2+=																									\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t}\n
Rules21.2+=																									\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t \#endif\n
Rules21.2+=																									\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t /* Compliant - as the identifier clean is not a\n
Rules21.2+=																									\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t reserved identifier and hence can be defined */\n"


Rules21.3="Rules 21.3:The memory allocation and deallocation functions of <stdlib.h> shall not be used.\n
Rules21.3+=Violation:														Solution\n
Rules21.3+=*********************************************************************************************************************************************\n
Rules21.3+=int *b;												int b[1024]; /* Compliant*/\n
Rules21.3+=void initialize()\n
Rules21.3+={\n
Rules21.3+=  b = (int *b) alloc ( 1024 * sizeof ( int ) );\n
Rules21.3+= /*Non-compliant, could lead to an out-of-storage run-time failure*/\n
Rules21.3+=}\n"


Rules21.4="Rules 21.4: The standard header file <setjmp.h> shall not be used.\n
Rules21.4+=Violation:																Solution\n
Rules21.4+=********************************************************************************************************************************************************************\n
Rules21.4+=  \#include <setjmp.h>	 		/* Non-compliant */								The functions setjmp and longjmp allow the normal\n
Rules21.4+=jmp_buf env_buffer;			/* Non-compliant */								 function call mechanisms to be bypassed.\n
Rules21.4+=static void MISRA21_4_func1 (void)											 This may lead to undefined behavior.\n
Rules21.4+={    int8_t val;\n
Rules21.4+=	val = setjmp( env_buffer )	/* Non-compliant */\n
Rules21.4+=}\n"


Rules21.5="Rules 21.5: The standard header file <signal.h> shall not be used.\n
Rules21.5+=Violation:																Solution\n
Rules21.5+=****************************************************************************************************************************************************************************\n
Rules21.5+=\#include <signal.h>			/* Non-compliant */								The signal handling contains implementation-defined\n
Rules21.5+=static void MISRA21_5_func1 (void)											and undefined behavior.\n
Rules21.5+={\n
Rules21.5+=	int8_t ret = 0;\n
Rules21.5+=	ret = raise(SIGINT);		/* Non-compliant */\n
Rules21.5+=} \n"


Rules21.6="Rules 21.6:The Standard Library input/output functions shall not be used.\n
Rules21.6+=Violation:														Solution\n
Rules21.6+=***********************************************************************************************************************************************************\n
Rules21.6+=static void MISRA21_6_func1 (void)											The standard library input/output function if used\n
Rules21.6+={	FILE *fp;													might cause unexpected behavior.\n
Rules21.6+=	fpos_t p;\n
Rules21.6+=	rename("BSH.txt", "BOSCH.txt");	/* Non-compliant */\n
Rules21.6+=	fp = fopen("BOSCH1.txt", "r");	/* Non-compliant */\n
Rules21.6+=	fgetpos(fp, &p);		/* Non-compliant */\n
Rules21.6+=	if( fp == NULL ) {\n
Rules21.6+=		perror(\"Error: \");		/* Non-compliant */\n
Rules21.6+=	}\n
Rules21.6+=	fclose(fp);			/* Non-compliant */\n
Rules21.6+=	remove(\"BSH.txt\"); 		/* Non-compliant */\n
Rules21.6+=}\n"


Rules21.7="Rules 21.7:The atof, atoi, atol and atoll functions of <stdlib.h> shall not be used.\n
Rules21.7+=Violation:																	Solution\n
Rules21.7+=***********************************************************************************************************************************************************************************\n
Rules21.7+=static void MISRA21_7_func1()															static void MISRA21_7_func1()\n
Rules21.7+={																	{\n
Rules21.7+=	const uint8_t ucStr1[20] = \"123456\";														const uint8_t ucStr1[20] = \"123456\";\n
Rules21.7+=	const uint8_t ucStr2[20] = \"1285.12\";														const uint8_t ucStr2[20] = \"1285.12\";\n
Rules21.7+=	uint16_t uiValue = 0;																uint16_t uiValue = 0;\n
Rules21.7+=	uint32_t ulValue = 0;																uint32_t ulValue = 0;\n
Rules21.7+=	float fValue = 0.0;																float fValue = 0.0;\n
Rules21.7+=	uiValue =  atoi(ucStr1);          /* Non-compliant */												uiValue = strtoi(ucStr1);          /* compliant */\n
Rules21.7+=	fValue = atof(ucStr2);		  /* Non-compliant */												fValue = strtof(ucStr2);		  /* compliant */\n
Rules21.7+=	ulValue = atol(ucStr1);		  /* Non-compliant */											ulValue = strtol(ucStr1);		 /* compliant */\n
Rules21.7+=}																	}\n"


Rules21.8="Rules 21.8: The library functions abort, exit, getenv and system of <stdlib.h> shall not be used.\n
Rules21.8+=Violation:																		Solution\n
Rules21.8+=*******************************************************************************************************************************************************************************\n
Rules21.8+=static void MISRA21_8_func1()													static void MISRA21_8_func1()\n
Rules21.8+={																		{\n
Rules21.8+=	uint8_t uca = 1;															uint8_t uca = 1;\n
Rules21.8+=	if(uca == 1)																if(uca == 1)\n
Rules21.8+=	{																		{\n
Rules21.8+=		exit(0);                  /*Non-compliant*/												return 0; /*Compliant*/\n
Rules21.8+=	}																	}\n
Rules21.8+=	else																	 else\n
Rules21.8+=	{																	{\n
Rules21.8+=		uca++;																		uca++;\n
Rules21.8+=	}																	}\n
Rules21.8+=}																	}\n
Rules21.8+=static void MISRA21_8_func2(uint8_t a, uint8_t b)\n
Rules21.8+={\n
Rules21.8+=	uint8_t uca = 0;\n
Rules21.8+=	uca = a + b;\n
Rules21.8+=	if( uca > 10)\n
Rules21.8+=	{\n
Rules21.8+=		uca++;\n
Rules21.8+=	}\n
Rules21.8+=	else\n
Rules21.8+=	{\n
Rules21.8+=		abort();				/*Non-compliant*/\n
Rules21.8+=	}\n
Rules21.8+=}\n
Rules21.8+=static void MISRA21_8_func3(const uint8_t *name)\n
Rules21.8+={\n
Rules21.8+=	uint8_t *value = '\0';\n
Rules21.8+=	value = getenv(name); 	   /*Non-compliant*/\n
Rules21.8+=}\n"


Rules21.9="Rules 21.9: The library functions bsearch and qsort of <stdlib.h> shall not be used.\n
Rules21.9+=Violation:																	Solution\n
Rules21.9+=************************************************************************************************************************************************************************************\n
Rules21.9+=static void MISRA21_9_func1()														The library function qsort and bsearch if used might\n
Rules21.9+={																	cause unexpected beahviour.\n
Rules21.9+=   bool bResult;\n
Rules21.9+=  int16_t values[] = { 5, 20, 29, 32, 63 };\n
Rules21.9+=   int16_t *item;\n
Rules21.9+=   int16_t key = 32;\n
Rules21.9+=   /*Non-compliant*/\n
Rules21.9+=   item = (int16_t*) bsearch (&key, values, 5);\n
Rules21.9+=   if( item != NULL )\n
Rules21.9+=   {\n
Rules21.9+=      bResult = 1;\n
Rules21.9+=   }\n
Rules21.9+=   else\n
Rules21.9+=   {\n
Rules21.9+=      bResult = 0;\n
Rules21.9+=   }\n
Rules21.9+=}\n
Rules21.9+=static void MISRA21_9_func2()\n
Rules21.9+={\n
Rules21.9+=   int16_t n;\n
Rules21.9+=   int16_t values[] = { 88, 56, 100, 2, 25 };\n
Rules21.9+=   qsort(values, 5, sizeof(int)); /*Non-compliant*/\n
Rules21.9+=}\n"


Rules21.10="Rules 21.10:The Standard Library time and date functions shall not be used.\n
Rules21.10+=Violation:														Solution\n
Rules21.10+=*************************************************************************************************************************************************************************\n
Rules21.10+=\#if defined (INDICATE_COMPILE_ERROR_MISRA21)										The standard library time and date functions shall not be\n
Rules21.10+=static uint8_t* MISRA21_10_func1()											used as it is undefined.\n
Rules21.10+={\n
Rules21.10+=  struct tm t;\n
Rules21.10+=   uint8_t* ucCalenderInfo;\n
Rules21.10+=   t.tm_sec    = 10;\n
Rules21.10+=   t.tm_min    = 10;\n
Rules21.10+=   t.tm_hour   = 6;\n
Rules21.10+=   t.tm_mday   = 25;\n
Rules21.10+=   t.tm_mon    = 2;\n
Rules21.10+=   t.tm_year   = 89;\n
Rules21.10+=   t.tm_wday   = 6;\n
Rules21.10+=   ucCalenderInfo = asctime(&t);      /*Non-compliant*/\n
Rules21.10+=   return ucCalenderInfo;\n
Rules21.10+=}\n"


Rules21.11="Rules 21.11:The standard header file <tgmath.h> shall not be used.\n
Rules21.11+=Violation:															Solution\n
Rules21.11+=*********************************************************************************************************************************************************************************\n
Rules21.11+=\#include<tgmath.h>														\t\#include<math.h>\n
Rules21.11+={																	{\n
Rules21.11+=	float f1, f2 = 1.0f;															float f1, f2 = 1.0f;\n
Rules21.11+=     void f( void )																void f( void )\n
Rules21.11+=	{																\t\t{\n
Rules21.11+=       f1 = sqrt ( f2 );												\t\t\t  f1 = sqrtf ( f2 );\n
Rules21.11+=     } /* Non-Compliant - generic sqrt used */								\t\t\t	} /* Compliant - float version sqrt used */\n
Rules21.11+=}																	}	\n"


Rules21.12="Rules 21.12:The exception handling features of <fenv.h> should not be used.\n
Rules21.12+=Violation:															Solution\n
Rules21.12+=**************************************************************************************************************************************************************************\n
Rules21.12+= \#if defined (INDICATE_COMPILE_ERROR_MISRA21)										The exception handling features of <fenv.h> are undefined\n
Rules21.12+=static void MISRA21_12_func1 ( double x, double y )									and hence should not be used.\n
Rules21.12+={\n
Rules21.12+=	double z;\n
Rules21.12+=	feclearexcept ( FE_DIVBYZERO ); 	 /* Non-compliant */\n
Rules21.12+=	z = x / y;\n
Rules21.12+=	if ( fetestexcept ( FE_DIVBYZERO ) ) 	/* Non-compliant */\n
Rules21.12+=	{\n
Rules21.12+=	}\n
Rules21.12+=	else\n
Rules21.12+=	{\n
Rules21.12+=\#pragma STDC FENV_ACCESS ON\n
Rules21.12+=		z = x * y;\n
Rules21.12+=	}\n
Rules21.12+=	if ( z > x )\n
Rules21.12+=	{\n
Rules21.12+=\#pragma STDC FENV_ACCESS OFF\n
Rules21.12+=		if ( fetestexcept ( FE_OVERFLOW ) ) 	/* Non-compliant */\n
Rules21.12+=		{\n
Rules21.12+=		}\n
Rules21.12+=	}\n
Rules21.12+=}\n
Rules21.12+=\# //INDICATE_COMPILE_ERROR_MISRA21 \n"


Rules21.13="Rules 21.13: Any value passed to a function in <ctype.h> shall be representable as an unsigned char or be the value of EOF.\n
Rules21.13+=Violation:																	Solution\n
Rules21.13+=******************************************************************************************************************************************************************************\n
Rules21.13+=static bool MISRA21_13_func1 (uint8_t a)												static bool MISRA21_13_func1 (uint8_t a)\n
Rules21.13+={																	{\n
Rules21.13+=	const char *t = &a;															const char *t = &a;\n
Rules21.13+=	int8_t b = 257;															int8_t b = 257;\n
Rules21.13+=	return (isdigit(*t))               /*Non-compliant*/								\t\t	isalpha((int32_t) 'b')	/*Compliant*/\n
Rules21.13+=			&& isalpha(*t)		   /*Non-compliant*/									\t\t	&& isgraph (EOF)	   /*Compliant*/\n
Rules21.13+=			&& isalnum(*t)          /*Non-compliant*/									\t\t\t}\n
Rules21.13+=			&& isalpha(b)           /*Non-compliant*/\n
Rules21.13+=			&& isspace(*t));          /*Non-compliant*/\n
Rules21.13+=}\n"


Rules21.14="Rules 21.14:The Standard Library function memcmp shall not be used to compare null terminated strings .\n
Rules21.14+=Violation:																	 Solution\n
Rules21.14+=*********************************************************************************************************************************************************************************\n
Rules21.14+= static bool MISRA21_14_func1()															static bool MISRA21_14_func1()\n
Rules21.14+={																	{\n
Rules21.14+=	bool bResult = 0;																bool bResult = 0;\n
Rules21.14+=    char buffer1[10];																char buffer1[10];\n
Rules21.14+=	char buffer2[10];																char buffer2[10];\n
Rules21.14+=	strcpy(buffer1, "data");																strcpy(buffer1, "data");\n
Rules21.14+=	strcpy(buffer2, "data");																strcpy(buffer2, "data");\n
Rules21.14+=	/* The following use of memcmp is non-complaint */												/* The following use of memcmp is compliant */\n
Rules21.14+=	if(memcmp(buffer1, buffer2, sizeof(buffer1) != 0))												static void MISRA21_14_func2(const uint8_t *packet)\n
Rules21.14+=	{																		{\n
Rules21.14+=		bResult = 0;																	unsigned char headerStart[6] = {'1', '2', '3', 152, 256, 'c'};\n
Rules21.14+=	}																							}\n
Rules21.14+=	else																   		if((NULL!=packet )&& (memcmp(packet, headerStart, 6) == 0))\n
Rules21.14+=	{																			  {\n
Rules21.14+=		bResult = 1;																				;\n
Rules21.14+=	}																			  }\n
Rules21.14+=	return bResult;															}\n
Rules21.14+=}\n"


Rules21.15="Rules 21.15:The pointer arguments to the Standard Library functions memcpy, memmove and memcmp shall be pointers to qualified\n
Rules21.15+=or unqualified version of compatible types.\n
Rules21.15+=Violation:															Solution\n
Rules21.15+=*****************************************************************************************************************************************************************************\n
Rules21.15+=static void  MISRA21_15_func1(uint8_t s1[8], uint16_t s2[8])					  static void  MISRA21_15_func2(const char s1[8], const char s2[8])\n
Rules21.15+={													{\n
Rules21.15+=   memcpy(s1,s2,8);          /*Non-compliant*/							\t(void)memcmp(s1,s2,8);          /*Compliant*/\n
Rules21.15+=   memmove(s1, s2, 5);       /*Non-compliant*/								}\n
Rules21.15+=   (void)memcmp(s1, s2, 5);  /*Non-compliant*/\n
Rules21.15+=}\n"


Rules21.16="Rules 21.16:The pointer arguments to the Standard Library function memcmp shall point to either a\n
Rules21.16+= pointer type, an essentially signed type, an essentially unsigned type, an essentially\n
Rules21.16+= Boolean type or an essential enum type.\n
Rules21.16+=Violation:															Solution\n
Rules21.16+=*********************************************************************************************************************************************************************\n
Rules21.16+=struct S															static bool MISRA21_16_func3(const uint8_t s1[7],\n
Rules21.16+={																const uint8_t s2[7])\n
Rules21.16+=	uint8_t *ucRule;															{\n
Rules21.16+=	uint8_t ucID;													   		return(memcmp(s1,s2,8));      /*Compliant */\n
Rules21.16+=};																	}\n
Rules21.16+=union U\n
Rules21.16+={\n
Rules21.16+=	uint32_t range;\n
Rules21.16+=	uint32_t height;\n
Rules21.16+=};\n
Rules21.16+=static bool MISRA21_16_func1(struct S *s1, struct S *s2)\n
Rules21.16+={\n
Rules21.16+=	return (memcmp(s1, s2, sizeof(struct S))!=0);  /*Non-compliant*/\n
Rules21.16+=}\n
Rules21.16+=static bool MISRA21_16_func2(union U *u1, union U *u2)\n
Rules21.16+={\n
Rules21.16+=	return (memcmp(u1, u2, sizeof(union U))!=0);  /*Non-compliant*/\n
Rules21.16+=}\n"


Rules21.17="Rules 21.17:Use of string handling functions from <string.h> shall not result in accesses beyond the bounds of the objects referenced\n
Rules21.17+= by their pointer parameters.\n
Rules21.17+=Violation:														Solution\n
Rules21.17+=*********************************************************************************************************************************************\n
Rules21.17+=static size_t MISRA21_17_func2()\n								\t			static void MISRA21_17_func1(const uint8_t *str)\n	{										
Rules21.17+= 																		\t{\n
Rules21.17+=const uint8_t text[5]= "token";\n												uint8_t string[]= \"short\";\n
Rules21.17+=/*The following is non-complaint as it results in reads beyond the end of 			strcpy(string, \"Too long to fit\");  \n 
Rules21.17+=	text as there is no null terminator */\n								\t\t\t\t/*complaint use of strcpy as string is only modified if str will fit*/\n
Rules21.17+=	return strlen(text);           /*Non-compliant*/\n						\t\t\t\t\t\tif(strlen(str) < (sizeof(string) - 1u))\n	
Rules21.17+=	}											{\n
Rules21.17+=																		\t\t\t\t\t\t\t\t\t\t\t\t\t\t(void)strcpy(string, str);\n
Rules21.17+=															\t\t\t\t\t\t\t\t\t\t\t\t}\n
Rules21.17+=																	\t\t\t\t\t\t\t\t\t\t\t\t}\n"


Rules21.18="Rules 21.18:The size_t argument passed to any function in <string.h> shall have an appropriate value.\n
Rules21.18+=Violation:														Solution\n
Rules21.18+=*********************************************************************************************************************************************\n
Rules21.18+=static size_t MISRA21_18_func1()								\t\tstatic size_t MISRA21_18_func1()\n
Rules21.18+={																{\n
Rules21.18+=	uint8_t buf1[] = \"12345\";										\tuint8_t buf1[] = \"12345\";\n
Rules21.18+=	uint8_t buf2[] = \"1234567890\";								\t\t\tuint8_t buf2[] = \"1234567890\";\n
Rules21.18+=	if(memcmp(buf1, buf2 , 10) == 0)       /*non-compliant*/							if(memcmp(buf1, buf2 , 5) == 0)        /*compliant*/\n
Rules21.18+=	{																{\n
Rules21.18+=		;																	;\n
Rules21.18+=	}																}\n
Rules21.18+=}\n"

Rules21.19="Rules 21.19:The pointers returned by the Standard Library functions localeconv, getenv, setlocale or,
Rules21.19+=str error shall only be used as if they have pointer to const-qualified type\n
Rules21.19+=Violation:															Solution\n
Rules21.19+=************************************************************************************************************************************************************************\n
Rules21.19+=\#if defined(INDICATE_COMPILE_ERROR_MISRA21)											void MISRA21_19_func2(void)\n
Rules21.19+={																uint8_t str[128];\n
Rules21.19+=	uint8_t *s1 = setlocale (LC_ALL, 0);   /*Non-compliant*/									strcpy (str, setlocale(LC_ALL, 0));\n
Rules21.19+=	struct lconv *conv = localeconv();	  /*Non-compliant*/									/*Compliant - 2nd parameter to strcpy takes a const char*/\n
Rules21.19+=	s1[1] = 'A';                          /* Undefined behaviour */								const struct lconv *conv = localeconv();  /*Compliant*/\n
Rules21.19+=	conv -> decimal_point = \"^\";         /* Undefined behaviour */									conv -> decimal_point = \"^\";       /*Constraint violation*/\n
Rules21.19+=}																}\n
Rules21.19+=\#endif \n" 


Rules21.20="Rule 21.20:The pointer returned by the Standard Library functions asctime, ctime, gmtime, localtime,\n
Rules21.20+=localeconv, getenv, setlocale or strerror shall not be used following a subsequent call to\n
Rules21.20+=the same function.\n
Rules21.20+=Violation:															Solution\n
Rules21.20+=*********************************************************************************************************************************************\n
Rules21.20+=static void MISRA21_20_func1(void)												static void MISRA21_20_func1(void)\n
Rules21.20+={																{\n
Rules21.20+=	const char *res1;														const char *res1;\n
Rules21.20+=	const char *res2;														const char *res2;\n
Rules21.20+=	char copy[128];														char copy[128];\n
Rules21.20+=	res1 = setlocale(LC_ALL, 0);													res1 = setlocale(LC_ALL, 0);\n
Rules21.20+=	(void)strcpy(copy, res1);													(void)strcpy(copy, res1);\n
Rules21.20+=	res2 = setlocale(LC_MONETARY, \"French\");											res2 = setlocale(LC_MONETARY, \"French\");\n printf(\"^%s\")
Rules21.20+=	printf(\"%s\", res1);	/*Non-compliant - use after subsequent call*/				\t\t\t\tprintf(\"%s\", copy);\n
Rules21.20+=}																/*Compliant- copy made before subsequent call*/\n
Rules21.20+=																					\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t}\n"


Rules22.1="Rule 22.1:All resources obtained dynamically by means of Standard Library functions shall be explicitly released.\n
Rules22.1+=Violation:															Solution\n
Rules22.1+=*********************************************************************************************************************************************\n
Rules22.1+=static int32_t MISRA22_1_func1 ( void )											static int32_t MISRA22_1_func1 ( void )\n
Rules22.1+={																{\n
Rules22.1+=	const void *b = malloc ( 40 );													void *b = malloc(40);\n
Rules22.1+=	return 1;															free(b);\n
Rules22.1+=	/* Non-compliant - dynamic memory not released */										return 1;	/* Compliant - dynamic memory released */\n
Rules22.1+=}																}\n"


Rules22.2="Rules 22.2:A block of memory shall only be freed if it was allocated by means of a Standard Library function.\n
Rules22.2+=Violation:															Solution\n
Rules22.2+=****************************************************************************************************************************************************************************\n
Rules22.2+=static void MISRA22_2_func1( void )												static void MISRA22_2_func1( void )\n
Rules22.2+={																{\n
Rules22.2+=	int32_t a;															int32_t a;\n
Rules22.2+=	free ( &a );/* Non-compliant - a does not point to allocated storage */				\t\t\tchar *a = ( char * ) malloc ( 1024 );\n
Rules22.2+=}																free ( &a );/*\n
Rules22.2+=																							\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t	compliant - a does  point to allocated storage */\n
Rules22.2+=																							\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t}\n"


Rules22.3="Rules 22.3:The same file shall not be open for read and write access at the same time on different streams.\n
Rules22.3+=Violation:																				Solution\n
Rules22.3+=************************************************************************************************************************************************************************************\n
Rules22.3+=\#if (! defined (__IAR_SYSTEMS_ICC__))													\#if (! defined (__IAR_SYSTEMS_ICC__))\n
Rules22.3+=|| (defined (INDICATE_COMPILE_ERROR_MISRA22) && defined (__IAR_SYSTEMS_ICC__))			\t\t\t\t\t|| (defined (INDICATE_COMPILE_ERROR_MISRA22) && defined (__IAR_SYSTEMS_ICC__))\n
Rules22.3+=static void MISRA22_3_func1 ( void )													static void MISRA22_3_func1 ( void )\n
Rules22.3+={																	{\n
Rules22.3+=	FILE *fw = fopen ( \"tmp\", \"r+\" ); 		/* \"r+\" opens for read/write */					\t\t\tFILE *fw = fopen ( \"tmp\", \"r+\" ); /* \"r+\" opens for read/write */\n
Rules22.3+=	FILE *fr = fopen ( \"tmp\", \"r\" ); 		/* Non-compliant */						\t\t\tfclose ( fw );\n
Rules22.3+=}																	FILE *fr = fopen ( \"tmp\", \"r\" ); 		/* Compliant */\n
Rules22.3+=\#endif																\tfclose(fr);\n
Rules22.3+=																							\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t}\n
Rules22.3+=																							\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t	\#endif \n"


Rules22.4="Rules 22.4:There shall be no attempt to write to a stream which has been opened as read-only.\n
Rules22.4+=Violation:														Solution\n
Rules22.4+=********************************************************************************************************************************************************************************************\n
Rules22.4+=if (! defined (__IAR_SYSTEMS_ICC__))											if (! defined (__IAR_SYSTEMS_ICC__))\n
Rules22.4+=|| (defined (INDICATE_COMPILE_ERROR_MISRA22) && defined (__IAR_SYSTEMS_ICC__)						|| (defined (INDICATE_COMPILE_ERROR_MISRA22) && defined (__IAR_SYSTEMS_ICC__))\n
Rules22.4+=static void MISRA22_4_func1 ( void )											static void MISRA22_4_func1 ( void )\n
Rules22.4+={															{\n
Rules22.4+=	FILE *fp = fopen ( \"tmp\", \"r\" );											FILE *fp = fopen ( \"tmp\", \"r\" );\n
Rules22.4+=	( void ) fprintf ( fp, \"What happens now?\" ); /* Non-compliant */			\t\t\t\t}\n
Rules22.4+=	( void ) fclose ( fp );												\#endif\n
Rules22.4+=}\n
Rules22.4+=\#endif \n"


Rules22.5="Rules 22.5:A pointer to a FILE object shall not be dereferenced.\n
Rules22.5+=Violation:															Solution\n
Rules22.5+=*********************************************************************************************************************************************************************\n
Rules22.5+=\#if defined(INDICATE_COMPILE_ERROR_MISRA22)											\#if defined(INDICATE_COMPILE_ERROR_MISRA22)\n
Rules22.5+=static void MISRA22_5_func1 (void)												static void MISRA22_5_func1 (void)\n
Rules22.5+={																{\n
Rules22.5+=	FILE *pf1;															FILE *pf1;\n
Rules22.5+=	FILE *pf2;															FILE *pf2;\n
Rules22.5+=	FILE f3;															FILE f3;\n
Rules22.5+=	f3 = *pf2;	/* Non-compliant */												pf2 = pf1; 		/* Compliant */\n
Rules22.5+=	pf1->pos = 0; /* Non-compliant - assumed that FILE * specifies a 				\t\t\t	}\n
Rules22.5+=       complete type with a member named pos */											\#endif\n
Rules22.5+=}\n
Rules22.5+=\#endif //INDICATE_COMPILE_ERROR_MISRA22 \n"


Rules22.6="Rules 22.6:The value of a pointer to a FILE shall not be used after the associated stream has been closed.\n
Rules22.6+=Violation:															Solution\n
Rules22.6+=************************************************************************************************************************************************************************\n
Rules22.6+=\#if defined(INDICATE_COMPILE_WARNING)												\#if defined(INDICATE_COMPILE_WARNING)\n
Rules22.6+=static void MISRA22_6_func1 ( void )												static void MISRA22_6_func1 ( void )\n
Rules22.6+={																{\n
Rules22.6+=	FILE *fp;															FILE *fp;\n
Rules22.6+=	void *p;															void *p;\n
Rules22.6+=	const void *helppointer;													const void *helppointer;\n
Rules22.6+=	fp = fopen ( "tmp", "w" );														fp = fopen ( "tmp", "w" );\n
Rules22.6+=	if ( fp == NULL )														if ( fp == NULL )\n
Rules22.6+=	{																{\n
Rules22.6+=	}																}\n
Rules22.6+=	fclose ( fp );															fprintf ( fp, "?" ); 	/* Compliant */\n
Rules22.6+=	fprintf ( fp, "?" ); 			/* Non-compliant */									p = fp; 					/* Compliant */\n
Rules22.6+=	p = fp; 					/* Non-compliant */								helppointer = p;\n
Rules22.6+=	helppointer = p;														fclose ( fp );\n
Rules22.6+=}																}\n
Rules22.6+=\#endif //INDICATE_COMPILE_WARNING 												\#endif //INDICATE_COMPILE_WARNING \n"


Rules22.7="Rules 22.7:The macro EOF shall only be compared with the unmodified return value from any Standard Library function capable of returning EOF.\n
Rules22.7+=Violation:																Solution\n
Rules22.7+=*********************************************************************************************************************************************\n
Rules22.7+=static void MISRA22_7_func1 ( void )													static void MISRA22_7_func3 ( void )\n
Rules22.7+={																	{\n
Rules22.7+=	char ch;																int32_t i_ch;\n
Rules22.7+=	ch = ( char ) getchar();														i_ch = getchar();\n
Rules22.7+=	/*																	* The following test is Compliant.It will be reliable\n
Rules22.7+=	 * The following test is Non-compliant.It will not be reliable as the									as the unconverted return value is used when checking for EOF*/\n
Rules22.7+=	 * return value is cast to a narrower type before checking for EOF									if ( EOF != i_ch )\n
Rules22.7+=	 */																		{\n
Rules22.7+=	if ( EOF != ch )																char ch;\n
Rules22.7+=	{																		ch = ( char ) i_ch;\n
Rules22.7+=	}																		}\n
Rules22.7+=}																	}\n


Rules22.8="Rules 22.8:The value of errno shall be set to zero prior to a call to an errno-setting-function .\n
Rules22.8+=Violation:																		Solution\n
Rules22.8+=***************************************************************************************************************************************************************\n
Rules22.8+=\#if defined(INDICATE_COMPILE_WARNING)													\#if defined(INDICATE_COMPILE_WARNING)\n
Rules22.8+=static void MISRA22_8_func1 ( void )													static void MISRA22_8_func1 ( void )\n
Rules22.8+={																	{\n
Rules22.8+=	FILE *f2;																FILE *f2;\n
Rules22.8+=	f2 = fopen ( "tmp", "w" );															f2 = fopen ( "tmp", "w" );\n
Rules22.8+=	fpos_t p1;																fpos_t p1;\n
Rules22.8+=	ftell( f2 );     /* Non-compliant*/													if ( 0 == errno )\n
Rules22.8+=		if ( 0 == errno )															{\n
Rules22.8+=		{																	 fgetpos( f2, &p1 ); /* Compliant by exception */\n
Rules22.8+=		}																	}\n
Rules22.8+=	}																	else\n
Rules22.8+=	else																	{\n
Rules22.8+=	{																	errno = 0;\n
Rules22.8+=		errno = 0;																fsetpos( f2, &p1 ); /* Compliant */\n
Rules22.8+=		if ( 0 == errno )															if ( 0 == errno )\n
Rules22.8+=		{																	{\n
Rules22.8+=		}																		}\n
Rules22.8+=	}																	}\n
Rules22.8+=	fclose(f2);																	fclose(f2);\n
Rules22.8+=}																		}\n"


Rules22.9="Rules 22.9:The value of errno shall be tested against zero after calling an errno-setting-function .\n
Rules22.9+=Violation:																	Solution\n
Rules22.9+=*********************************************************************************************************************************************************************\n
Rules22.9+=static void MISRA22_9_func1 ( void )													static void MISRA22_9_func1 ( void )\n
Rules22.9+={																	{\n
Rules22.9+=	FILE *f3;																FILE *f3;\n
Rules22.9+=	f3 = fopen ( "tmp", "w" );															f3 = fopen ( "tmp", "w" );\n
Rules22.9+=	fpos_t p2;																fpos_t p2;\n
Rules22.9+=	errno = 0;																errno = 0;\n
Rules22.9+=	fgetpos( f3, &p2 );															fgetpos( f3, &p2 );\n
Rules22.9+=	MISRA22_8_func1();       /* Non-COmpliant - function call */										if ( 0 != errno )\n
Rules22.9+=	if ( 0 != errno )															{\n
Rules22.9+=	{																	}\n
Rules22.9+=	}																	errno = 0;\n
Rules22.9+=	errno = 0;																fsetpos( f3, &p2 );\n
Rules22.9+=	fsetpos( f3, &p2 );															if ( 0 != errno )      /* Compliant */\n
Rules22.9+=	fclose(f3);																fclose(f3)\n
Rules22.9+=}																	}\n"


Rules22.10="Rules 22.10:The value of errno shall only be tested when the last function to be called was an errno-setting-function.\n
Rules22.10+=Violation:																Solution\n
Rules22.10+=****************************************************************************************************************************************************************************\n
Rules22.10+=static void MISRA22_10_func1 ( void )													static void MISRA22_10_func1 ( void )\n
Rules22.10+={																	{\n
Rules22.10+=	float32_t f32;																float32_t f32;\n
Rules22.10+=	errno = 0;																errno = 0;\n
Rules22.10+=	f32 = atof( "A.12" );															f32 =strtof( "A.12" );\n
Rules22.10+=	if ( 0 == errno )      /* Non - Compliant */												f32 = strtod ( "A.12", NULL )\n
Rules22.10+=	{																	if ( 0 == errno )      /* Compliant */\n
Rules22.10+=		/* f32 may not have a valid value in here */												{\n
Rules22.10+=	}																	/* f32 may have a valid value in here */\n
Rules22.10+=	errno = 0;																}\n
Rules22.10+=	f32 = strtod ( "A.12", NULL )														}\n
Rules22.10+=}\n"



ifeq ($(misra),2.1)
cure:
	@$(eval $(Rules2.1_output))
#	@$(call prntGr,$(Rules2.1))	
else ifeq ($(misra),2.2)
cure: 
	@$(call prntGr,$(Rules2.2))	
else ifeq ($(misra),2.3)
cure: 
	@$(call prntGr,$(Rules2.3))	
else ifeq ($(misra),2.4)
cure: 
	@$(call prntGr,$(Rules2.4))	
else ifeq ($(misra),2.5)
cure: 
	@$(call prntGr,$(Rules2.5))	
else ifeq ($(misra),2.6)
cure: 
	@$(call prntGr,$(Rules2.6))	
else ifeq ($(misra),2.7)
cure: 
	@$(call prntGr,$(Rules2.7))	
else ifeq ($(misra),3.1)
cure: 
	@$(call prntGr,$(Rules3.1))	
else ifeq ($(misra),3.2)
cure: 
	@$(call prntGr,$(Rules3.2))	
else ifeq ($(misra),4.1)
cure: 
	@$(call prntGr,$(Rules4.1))	
else ifeq ($(misra),4.2)
cure: 
	@$(call prntGr,$(Rules4.2))	
else ifeq ($(misra),5.1)
cure: 
	@$(call prntGr,$(Rules5.1))	
else ifeq ($(misra),5.2)
cure: 
	@$(call prntGr,$(Rules5.2))	
else ifeq ($(misra),5.3)
cure: 
	@$(call prntGr,$(Rules5.3))	
else ifeq ($(misra),5.4)
cure: 
	@$(call prntGr,$(Rules5.4))	
else ifeq ($(misra),5.5)
cure: 
	@$(call prntGr,$(Rules5.5))	
else ifeq ($(misra),5.6)
cure: 
	@$(call prntGr,$(Rules5.6))	
else ifeq ($(misra),5.7)
cure: 
	@$(call prntGr,$(Rules5.7))	
else ifeq ($(misra),5.8)
cure: 
	@$(call prntGr,$(Rules5.8))	
else ifeq ($(misra),5.9)
cure: 
	@$(call prntGr,$(Rules5.9))	
else ifeq ($(misra),6.1)
cure: 
	@$(call prntGr,$(Rules6.1))	
else ifeq ($(misra),6.2)
cure: 
	@$(call prntGr,$(Rules6.2))	
else ifeq ($(misra),7.1)
cure: 
	@$(call prntGr,$(Rules7.1))	
else ifeq ($(misra),7.2)
cure: 
	@$(call prntGr,$(Rules7.2))	
else ifeq ($(misra),7.3)
cure: 
	@$(call prntGr,$(Rules7.3))	
else ifeq ($(misra),7.4)
cure: 
	@$(call prntGr,$(Rules7.4))	
else ifeq ($(misra),8.1)
cure: 
	@$(call prntGr,$(Rules8.1))	
else ifeq ($(misra),8.2)
cure: 
	@$(call prntGr,$(Rules8.2))	
else ifeq ($(misra),8.3)
cure: 
	@$(call prntGr,$(Rules8.3))	
else ifeq ($(misra),8.4)
cure: 
	@$(call prntGr,$(Rules8.4))	
else ifeq ($(misra),8.5)
cure: 
	@$(call prntGr,$(Rules8.5))	
else ifeq ($(misra),8.6)
cure: 
	@$(call prntGr,$(Rules8.6))	
else ifeq ($(misra),8.7)
cure: 
	@$(call prntGr,$(Rules8.7))	
else ifeq ($(misra),8.8)
cure: 
	@$(call prntGr,$(Rules8.8))	
else ifeq ($(misra),8.9)
cure: 
	@$(call prntGr,$(Rules8.9))	
else ifeq ($(misra),8.10)
cure: 
	@$(call prntGr,$(Rules8.10))	
else ifeq ($(misra),8.11)
cure: 
	@$(call prntGr,$(Rules8.11))	
else ifeq ($(misra),8.12)
cure: 
	@$(call prntGr,$(Rules8.12))	
else ifeq ($(misra),8.13)
cure: 
	@$(call prntGr,$(Rules8.13))	
else ifeq ($(misra),8.14)
cure: 
	@$(call prntGr,$(Rules8.14))	
else ifeq ($(misra),9.1)
cure: 
	@$(call prntGr,$(Rules9.1))	
else ifeq ($(misra),9.2)
cure: 
	@$(call prntGr,$(Rules9.2))	
else ifeq ($(misra),9.3)
cure: 
	@$(call prntGr,$(Rules9.3))	
else ifeq ($(misra),9.4)
cure: 
	@$(call prntGr,$(Rules9.4))	
else ifeq ($(misra),9.5)
cure: 
	@$(call prntGr,$(Rules9.5))	
else ifeq ($(misra),10.1)
cure: 
	@$(call prntGr,$(Rules10.1))	
else ifeq ($(misra),10.2)
cure: 
	@$(call prntGr,$(Rules10.2))	
else ifeq ($(misra),10.3)
cure: 
	@$(call prntGr,$(Rules10.3))	
else ifeq ($(misra),10.4)
cure: 
	@$(call prntGr,$(Rules10.4))	
else ifeq ($(misra),10.5)
cure: 
	@$(call prntGr,$(Rules10.5))	
else ifeq ($(misra),10.6)
cure: 
	@$(call prntGr,$(Rules10.6))	
else ifeq ($(misra),10.7)
cure: 
	@$(call prntGr,$(Rules10.7))	
else ifeq ($(misra),10.8)
cure: 
	@$(call prntGr,$(Rules10.8))	
else ifeq ($(misra),11.1)
cure: 
	@$(call prntGr,$(Rules11.1))	
else ifeq ($(misra),11.2)
cure: 
	@$(call prntGr,$(Rules11.2))	
else ifeq ($(misra),11.3)
cure: 
	@$(call prntGr,$(Rules11.3))	
else ifeq ($(misra),11.4)
cure: 
	@$(call prntGr,$(Rules11.4))	
else ifeq ($(misra),11.5)
cure: 
	@$(call prntGr,$(Rules11.5))	
else ifeq ($(misra),11.6)
cure: 
	@$(call prntGr,$(Rules11.6))	
else ifeq ($(misra),11.7)
cure: 
	@$(call prntGr,$(Rules11.7))	
else ifeq ($(misra),11.8)
cure: 
	@$(call prntGr,$(Rules11.8))	
else ifeq ($(misra),11.9)
cure: 
	@$(call prntGr,$(Rules11.9))	
else ifeq ($(misra),12.1)
cure: 
	@$(call prntGr,$(Rules12.1))	
else ifeq ($(misra),12.2)
cure: 
	@$(call prntGr,$(Rules12.2))	
else ifeq ($(misra),12.3)
cure: 
	@$(call prntGr,$(Rules12.3))	
else ifeq ($(misra),12.4)
cure: 
	@$(call prntGr,$(Rules12.4))	
else ifeq ($(misra),12.5)
cure: 
	@$(call prntGr,$(Rules12.5))	
else ifeq ($(misra),13.1)
cure: 
	@$(call prntGr,$(Rules13.1))	
else ifeq ($(misra),13.2)
cure: 
	@$(call prntGr,$(Rules13.2))	
else ifeq ($(misra),13.3)
cure: 
	@$(call prntGr,$(Rules13.3))	
else ifeq ($(misra),13.4)
cure: 
	@$(call prntGr,$(Rules13.4))	
else ifeq ($(misra),13.5)
cure: 
	@$(call prntGr,$(Rules13.5))	
else ifeq ($(misra),13.6)
cure: 
	@$(call prntGr,$(Rules13.6))	
else ifeq ($(misra),14.1)
cure: 
	@$(call prntGr,$(Rules14.1))	
else ifeq ($(misra),14.2)
cure: 
	@$(call prntGr,$(Rules14.2))	
else ifeq ($(misra),14.3)
cure: 
	@$(call prntGr,$(Rules14.3))	
else ifeq ($(misra),14.4)
cure: 
	@$(call prntGr,$(Rules14.4))	
else ifeq ($(misra),15.1)
cure: 
	@$(call prntGr,$(Rules15.1))	
else ifeq ($(misra),15.2)
cure: 
	@$(call prntGr,$(Rules15.2))	
else ifeq ($(misra),15.3)
cure: 
	@$(call prntGr,$(Rules15.3))	
else ifeq ($(misra),15.4)
cure: 
	@$(call prntGr,$(Rules15.4))	
else ifeq ($(misra),15.5)
cure: 
	@$(call prntGr,$(Rules15.5))	
else ifeq ($(misra),15.6)
cure: 
	@$(call prntGr,$(Rules15.6))	
else ifeq ($(misra),15.7)
cure: 
	@$(call prntGr,$(Rules15.7))	
else ifeq ($(misra),16.1)
cure: 
	@$(call prntGr,$(Rules16.1))	
else ifeq ($(misra),16.2)
cure: 
	@$(call prntGr,$(Rules16.2))	
else ifeq ($(misra),16.3)
cure: 
	@$(call prntGr,$(Rules16.3))	
else ifeq ($(misra),16.4)
cure: 
	@$(call prntGr,$(Rules16.4))	
else ifeq ($(misra),16.5)
cure: 
	@$(call prntGr,$(Rules16.5))	
else ifeq ($(misra),16.6)
cure: 
	@$(call prntGr,$(Rules16.6))	
else ifeq ($(misra),16.7)
cure: 
	@$(call prntGr,$(Rules16.7))	
else ifeq ($(misra),17.1)
cure: 
	@$(call prntGr,$(Rules17.1))	
else ifeq ($(misra),17.2)
cure: 
	@$(call prntGr,$(Rules17.2))	
else ifeq ($(misra),17.3)
cure: 
	@$(call prntGr,$(Rules17.3))	
else ifeq ($(misra),17.4)
cure: 
	@$(call prntGr,$(Rules17.4))	
else ifeq ($(misra),17.5)
cure: 
	@$(call prntGr,$(Rules17.5))	
else ifeq ($(misra),17.6)
cure: 
	@$(call prntGr,$(Rules17.6))	
else ifeq ($(misra),17.7)
cure: 
	@$(call prntGr,$(Rules17.7))	
else ifeq ($(misra),17.8)
cure: 
	@$(call prntGr,$(Rules17.8))	
else ifeq ($(misra),18.1)
cure: 
	@$(call prntGr,$(Rules18.1))	
else ifeq ($(misra),18.2)
cure: 
	@$(call prntGr,$(Rules18.2))	
else ifeq ($(misra),18.3)
cure: 
	@$(call prntGr,$(Rules18.3))	
else ifeq ($(misra),18.4)
cure: 
	@$(call prntGr,$(Rules18.4))	
else ifeq ($(misra),18.5)
cure: 
	@$(call prntGr,$(Rules18.5))	
else ifeq ($(misra),18.6)
cure: 
	@$(call prntGr,$(Rules18.6))	
else ifeq ($(misra),18.7)
cure: 
	@$(call prntGr,$(Rules18.7))	
else ifeq ($(misra),18.8)
cure: 
	@$(call prntGr,$(Rules18.8))	
else ifeq ($(misra),19.1)
cure: 
	@$(call prntGr,$(Rules19.1))	
else ifeq ($(misra),19.2)
cure: 
	@$(call prntGr,$(Rules19.2))	
else ifeq ($(misra),20.1)
cure: 
	@$(call prntGr,$(Rules20.1))	
else ifeq ($(misra),20.2)
cure: 
	@$(call prntGr,$(Rules20.2))	
else ifeq ($(misra),20.3)
cure: 
	@$(call prntGr,$(Rules20.3))	
else ifeq ($(misra),20.4)
cure: 
	@$(call prntGr,$(Rules20.4))	
else ifeq ($(misra),20.5)
cure: 
	@$(call prntGr,$(Rules20.5))	
else ifeq ($(misra),20.6)
cure: 
	@$(call prntGr,$(Rules20.6))	
else ifeq ($(misra),20.7)
cure: 
	@$(call prntGr,$(Rules20.7))	
else ifeq ($(misra),20.8)
cure: 
	@$(call prntGr,$(Rules20.8))	
else ifeq ($(misra),20.9)
cure: 
	@$(call prntGr,$(Rules20.9))	
else ifeq ($(misra),20.10)
cure: 
	@$(call prntGr,$(Rules20.10))	
else ifeq ($(misra),20.11)
cure: 
	@$(call prntGr,$(Rules20.11))	
else ifeq ($(misra),20.12)
cure: 
	@$(call prntGr,$(Rules20.12))	
else ifeq ($(misra),20.13)
cure: 
	@$(call prntGr,$(Rules20.13))	
else ifeq ($(misra),20.14)
cure: 
	@$(call prntGr,$(Rules20.14))	
else ifeq ($(misra),21.1)
cure: 
	@$(call prntGr,$(Rules21.1))	
else ifeq ($(misra),21.2)
cure: 
	@$(call prntGr,$(Rules21.2))	
else ifeq ($(misra),21.3)
cure: 
	@$(call prntGr,$(Rules21.3))	
else ifeq ($(misra),21.4)
cure: 
	@$(call prntGr,$(Rules21.4))	
else ifeq ($(misra),21.5)
cure: 
	@$(call prntGr,$(Rules21.5))	
else ifeq ($(misra),21.6)
cure: 
	@$(call prntGr,$(Rules21.6))	
else ifeq ($(misra),21.7)
cure: 
	@$(call prntGr,$(Rules21.7))	
else ifeq ($(misra),21.8)
cure: 
	@$(call prntGr,$(Rules21.8))	
else ifeq ($(misra),21.9)
cure: 
	@$(call prntGr,$(Rules21.9))	
else ifeq ($(misra),21.10)
cure: 
	@$(call prntGr,$(Rules21.10))	
else ifeq ($(misra),21.11)
cure: 
	@$(call prntGr,$(Rules21.11))	
else ifeq ($(misra),21.12)
cure: 
	@$(call prntGr,$(Rules21.12))	
else ifeq ($(misra),21.13)
cure: 
	@$(call prntGr,$(Rules21.13))	
else ifeq ($(misra),21.14)
cure: 
	@$(call prntGr,$(Rules21.14))	
else ifeq ($(misra),21.15)
cure: 
	@$(call prntGr,$(Rules21.15))	
else ifeq ($(misra),21.16)
cure: 
	@$(call prntGr,$(Rules21.16))	
else ifeq ($(misra),21.17)
cure: 
	@$(call prntGr,$(Rules21.17))	
else ifeq ($(misra),21.18)
cure: 
	@$(call prntGr,$(Rules21.18))	
else ifeq ($(misra),21.19)
cure: 
	@$(call prntGr,$(Rules21.19))	
else ifeq ($(misra),21.20)
cure: 
	@$(call prntGr,$(Rules21.20))	
else ifeq ($(misra),22.1)
cure: 
	@$(call prntGr,$(Rules22.1))	
else ifeq ($(misra),22.2)
cure: 
	@$(call prntGr,$(Rules22.2))	
else ifeq ($(misra),22.3)
cure: 
	@$(call prntGr,$(Rules22.3))	
else ifeq ($(misra),22.4)
cure: 
	@$(call prntGr,$(Rules22.4))	
else ifeq ($(misra),22.5)
cure: 
	@$(call prntGr,$(Rules22.5))	
else ifeq ($(misra),22.6)
cure: 
	@$(call prntGr,$(Rules22.6))	
else ifeq ($(misra),22.7)
cure: 
	@$(call prntGr,$(Rules22.7))	
else ifeq ($(misra),22.8)
cure: 
	@$(call prntGr,$(Rules22.8))	
else ifeq ($(misra),22.9)
cure: 
	@$(call prntGr,$(Rules22.9))	
else ifeq ($(misra),22.10)
cure: 
	@$(call prntGr,$(Rules22.10))	
else
cure:
	@$(call prntGr, invalid_rule)
endif	