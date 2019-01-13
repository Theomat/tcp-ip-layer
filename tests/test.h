//   Copyright 2018 MATRICON Theo
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//   ------------------------------------------------------------------------
//   Usage:
//      1 - call TESTS_INIT()
//            init everything we need
//      2 - Now you can use everywhere in the same file :
//        o - TESTCASE(bool, printf args)
//              print test results to stdout
//        o - TESTCASE_RUN(bool, code, printf args)
//              execute code in a nested block along with TESTCASE(bool, printf,
//              args)
//        o - TESTCASE_FATAL(bool, printf args)
//              exit the program on test failure
//        o - TESTCASE_COMPOUND(string)
//              start a testcase with multiple booleans
//        o - TESTCASE_COMPOUND_AND(bool)
//              add the bool condition to the current compound test
//        o - TESTCASE_COMPOUND_END()
//              end a compound testcase
//      3 - call TESTS_REPORT()
//              print the total end report
//
//   To remove colors :
//      #define NO_TEST_COLOR 1
//   To remove the START line before the test :
//      #define NO_START_LINE 1
//
//   Dependencies:
//      - printf  -> stdio.h
//      - exit    -> stdlib.h (Only with TESTCASE_FATAL)
//
//   ------------------------------------------------------------------------
#ifndef TEST_H
#define TEST_H

#if NO_TEST_COLOR
#define TEST_STR_PASSED "PASSED"
#define TEST_STR_FAILED "FAILED"
#define TEST_STR_START "START>"
#define TEST_STR_PASSED_FATAL "FATAL:PASSED"
#define TEST_STR_FAILED_FATAL "FATAL:FAILED"
#define TEST_STR_START_FATAL "FATAL:START>"
#define TEST_STR_ABORT_FATAL "FATAL: TEST FAILED > TESTS ABORTED"
#else
#define TEST_STR_PASSED "\x1B[32mPASSED\x1B[0m"
#define TEST_STR_FAILED "\x1B[31mFAILED\x1B[0m"
#define TEST_STR_START "\x1B[34mSTART>\x1B[0m"
#define TEST_STR_PASSED_FATAL "\x1B[33mFATAL:\x1B[32mPASSED\x1B[0m"
#define TEST_STR_FAILED_FATAL "\x1B[33mFATAL:\x1B[31mFAILED\x1B[0m"
#define TEST_STR_START_FATAL "\x1B[33mFATAL:\x1B[34mSTART>\x1B[0m"
#define TEST_STR_ABORT_FATAL                                                   \
  "\x1B[33mFATAL\x1B[0m: TEST \x1B[31mFAILED\x1B[0m > TESTS ABORTED"
#endif

#define TESTS_INIT()                                                           \
  static long testh_tests_count     = 0;                                       \
  static long testh_successes_count = 0;                                       \
  static long testh_test_result     = 0;                                       \
  static char* testh_compound_desc  = 0;                                       \
  if (testh_compound_desc == 0 && testh_test_result == 0)                      \
    testh_compound_desc = 0;

#define TESTCASE_COMPOUND_AND(bool)                                            \
  do {                                                                         \
    testh_test_result = testh_test_result && (bool);                           \
  } while (0)

#define TESTCASE_COMPOUND_END()                                                \
  do {                                                                         \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] %s\n", __FILE__,                                          \
           testh_test_result ? TEST_STR_PASSED : TEST_STR_FAILED,              \
           testh_compound_desc);                                               \
  } while (0)

#define TESTS_REPORT()                                                         \
  printf("{%s}[%s] %ld / %ld\n", __FILE__,                                     \
         testh_successes_count == testh_tests_count ? TEST_STR_PASSED          \
                                                    : TEST_STR_FAILED,         \
         testh_successes_count, testh_tests_count);

#if NO_START_LINE
#define TESTCASE(predicate, desc...)                                           \
  do {                                                                         \
    testh_tests_count++;                                                       \
    testh_test_result = predicate;                                             \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] ", __FILE__,                                              \
           testh_test_result ? TEST_STR_PASSED : TEST_STR_FAILED);             \
    printf(desc);                                                              \
    printf("\n");                                                              \
  } while (0)

#define TESTCASE_COMPOUND(desc)                                                \
  do {                                                                         \
    testh_compound_desc = desc;                                                \
    testh_tests_count++;                                                       \
    testh_test_result = 1;                                                     \
  } while (0)

#define TESTCASE_RUN(predicate, code, desc...)                                 \
  do {                                                                         \
    code;                                                                      \
    testh_tests_count++;                                                       \
    testh_test_result = predicate;                                             \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] ", __FILE__,                                              \
           testh_test_result ? TEST_STR_PASSED : TEST_STR_FAILED);             \
    printf(desc);                                                              \
    printf("\n");                                                              \
  } while (0)

#define TESTCASE_FATAL(predicate, desc...)                                     \
  do {                                                                         \
    testh_tests_count++;                                                       \
    testh_test_result = predicate;                                             \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] ", __FILE__,                                              \
           testh_test_result ? TEST_STR_PASSED_FATAL : TEST_STR_FAILED_FATAL); \
    printf(desc);                                                              \
    printf("\n");                                                              \
    if (!testh_test_result) {                                                  \
      TESTS_REPORT();                                                          \
      printf("{%s}%s\n", __FILE__, TEST_STR_ABORT_FATAL);                      \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#else
#define TESTCASE(predicate, desc...)                                           \
  do {                                                                         \
    testh_tests_count++;                                                       \
    printf("{%s}[%s] ", __FILE__, TEST_STR_START);                             \
    printf(desc);                                                              \
    printf("\n");                                                              \
    testh_test_result = predicate;                                             \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] ", __FILE__,                                              \
           testh_test_result ? TEST_STR_PASSED : TEST_STR_FAILED);             \
    printf(desc);                                                              \
    printf("\n");                                                              \
  } while (0)

#define TESTCASE_COMPOUND(desc)                                                \
  do {                                                                         \
    testh_compound_desc = desc;                                                \
    testh_tests_count++;                                                       \
    printf("{%s}[%s] ", __FILE__, TEST_STR_START);                             \
    printf(desc);                                                              \
    printf("\n");                                                              \
    testh_test_result = 1;                                                     \
  } while (0)

#define TESTCASE_RUN(predicate, code, desc...)                                 \
  do {                                                                         \
    code;                                                                      \
    testh_tests_count++;                                                       \
    printf("{%s}[%s] ", __FILE__, TEST_STR_START);                             \
    printf(desc);                                                              \
    printf("\n");                                                              \
    testh_test_result = predicate;                                             \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] ", __FILE__,                                              \
           testh_test_result ? TEST_STR_PASSED : TEST_STR_FAILED);             \
    printf(desc);                                                              \
    printf("\n");                                                              \
  } while (0)

#define TESTCASE_FATAL(predicate, desc...)                                     \
  do {                                                                         \
    testh_tests_count++;                                                       \
    printf("{%s}[%s] ", __FILE__, TEST_STR_START_FATAL);                       \
    printf(desc);                                                              \
    printf("\n");                                                              \
    testh_test_result = predicate;                                             \
    testh_successes_count += testh_test_result;                                \
    printf("{%s}[%s] ", __FILE__,                                              \
           testh_test_result ? TEST_STR_PASSED_FATAL : TEST_STR_FAILED_FATAL); \
    printf(desc);                                                              \
    printf("\n");                                                              \
    if (!testh_test_result) {                                                  \
      TESTS_REPORT();                                                          \
      printf("{%s}%s\n", __FILE__, TEST_STR_ABORT_FATAL);                      \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#endif
#endif
