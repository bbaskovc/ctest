/***********************************************************************************************************************
 *
 * @file        ctest.h
 * @brief       Defines environment-specific macros for configuring CTest behavior.
 * @author      Blaz Baskovc
 * @copyright   Copyright 2025 Blaz Baskovc
 * @date        2025-03-11
 *
 **********************************************************************************************************************/

 #ifndef CTEST_H
 #define CTEST_H
 
 // --- Includes --------------------------------------------------------------------------------------------------------
 
 #include <stdarg.h>
 #include <stdbool.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 
 // --- Public Defines --------------------------------------------------------------------------------------------------
 
 /**
  * @brief   Codes to color messages.
  */
 #define CTEST_GRY  "\e[0;37m"
 #define CTEST_GRYB "\e[1;37m"
 #define CTEST_RED  "\e[1;31m"
 #define CTEST_GRN  "\e[1;32m"
 #define CTEST_RST  "\e[0m"
 
 // --- Public Macros ---------------------------------------------------------------------------------------------------
 
 /**
  * @brief   Macro that evaluates a condition and increments the failed_assertions counter if the assertion fails, while
  *          logging the condition, file, function, and line number.
  */
 #define CTEST_ASSERT(condition)                                                                                        \
     failed_assertions += !ctest__assert((condition), #condition, __FILE__, __FUNCTION__, __LINE__, "") ? 1 : 0
 
 /**
  * @brief   Macro that evaluates a condition and increments the failed_assertions counter if the assertion fails,
  *          logging the condition, file, function, line number, and an optional custom message with additional
  *          arguments.
  */
 #define CTEST_ASSERT_MSG(condition, msg, ...)                                                                          \
     failed_assertions +=                                                                                               \
         !ctest__assert((condition), #condition, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__) ? 1 : 0
 
 /**
  * @brief   Asserts that two values are equal.
  */
 #define CTEST_ASSERT_EQ(a, b) CTEST_ASSERT((a) == (b))
 
 /**
  * @brief   Asserts that two values are equal with a custom message.
  */
 #define CTEST_ASSERT_EQ_MSG(a, b, msg, ...) CTEST_ASSERT_MSG((a) == (b), msg, ##__VA_ARGS__)
 
 /**
  * @brief   Asserts that two strings are equal.
  */
 #define CTEST_ASSERT_EQ_STR(a, b) CTEST_ASSERT(strcmp((a), (b)) == 0)
 
 /**
  * @brief   Asserts that two strings are equal with a custom message.
  */
 #define CTEST_ASSERT_EQ_STR_MSG(a, b, msg, ...) CTEST_ASSERT_MSG(strcmp((a), (b)) == 0, msg, ##__VA_ARGS__)
 
 /**
  * @brief   Defines a test function with a given name and body.
  */
 #define CTEST_TEST(name, ...)                                                                                          \
     static int test_##name(void)                                                                                       \
     {                                                                                                                  \
         int failed_assertions = 0;                                                                                     \
         __VA_ARGS__ return failed_assertions;                                                                          \
     }
 
 /**
  * @brief   Runs all defined tests and returns the result.
  */
 #define CTEST_RUN_TESTS()                                                                                              \
     int main(void)                                                                                                     \
     {                                                                                                                  \
         return ctest__run_tests() ? 0 : 1;                                                                             \
     }
 
 #define ADD(name) static int test_##name(void);
 
 #ifdef TESTS
 TESTS
 #endif /* TESTS */
 #undef ADD
 
 // --- Public Functions Prototypes -------------------------------------------------------------------------------------
 
 static bool ctest__assert(bool result, const char *expression, const char *file, const char *test_name, const int line,
                           const char *msg, ...);
 static bool ctest__run_tests(void);
 static char *ctest__get_timestamp(void);
 
 // --- Public Functions Definitions ------------------------------------------------------------------------------------
 
 static bool ctest__assert(bool result, const char *expression, const char *file, const char *test_name, const int line,
                           const char *msg, ...)
 {
     if (result)
     {
         return true;
     }
     else
     {
         fprintf(stderr, "❌ %s:%d -> %s\n💬 Assertion of '%s' failed\n📝 ", file, line, test_name, expression);
         va_list args;
         va_start(args, msg);
         vfprintf(stderr, msg, args);
         fprintf(stderr, "\n");
         return false;
     }
 }
 
 static bool ctest__run_tests(void)
 {
 #ifndef TESTS
 #define TESTS // Defined to omit useless warnings when compiling
     fprintf(stderr, "ERROR: No tests are defined!\n");
     exit(1);
 #endif // !TESTS
 
     int test_count = 0;
 #define ADD(name) test_count++;
     TESTS
 #undef ADD
     printf(CTEST_GRY "INFO: Running a total of %d tests.\n\n", test_count);
 
     int fail_test_count = 0;
     int failed_assertions = 0;
     time_t start_time = time(NULL);
     // #define ADD(name) fail_test_count += test_##name() ? 1 : 0;
 #define ADD(name)                                                                                                      \
     failed_assertions = test_##name();                                                                                 \
     if (failed_assertions > 0)                                                                                         \
     {                                                                                                                  \
         fprintf(stderr, "💥 Test " CTEST_GRYB "%s" CTEST_GRY " failed %d assertions!\n", #name, failed_assertions);    \
         fail_test_count++;                                                                                             \
     }                                                                                                                  \
     else                                                                                                               \
     {                                                                                                                  \
         fprintf(stderr, "✅ Test " CTEST_GRYB "%s" CTEST_GRY " passed.\n", #name);                                     \
     }
     TESTS
 #undef ADD
     time_t end_time = time(NULL);
 
     printf("\n");
     int pass_test_count = test_count - fail_test_count;
     printf(CTEST_GRY "    Tests  " CTEST_RED "%d failed" CTEST_GRY " | " CTEST_GRN "%d passed" CTEST_GRY
                      " (%d)\n" CTEST_RST,
            fail_test_count, pass_test_count, test_count);
     printf(CTEST_GRY " Start at  " CTEST_RST "%s\n", ctest__get_timestamp());
     printf(CTEST_GRY " Duration  " CTEST_RST "%lds\n", end_time - start_time);
     if (fail_test_count > 0)
         return false;
     return true;
 }
 
 static char *ctest__get_timestamp(void)
 {
     time_t rawtime;
     struct tm *timeinfo;
 
     char *buffer = (char *)malloc(9 * sizeof(char)); // HH:MM:SS + null terminator
     if (buffer == NULL)
     {
         fprintf(stderr, "ERROR: Could not allocate memory for timestamp!\n");
         exit(1);
     }
 
     time(&rawtime);
     timeinfo = localtime(&rawtime);
     strftime(buffer, 9, "%H:%M:%S", timeinfo);
 
     return buffer;
 }
 
 #endif /* CTEST_H */
 
 // --- EOF -------------------------------------------------------------------------------------------------------------
 