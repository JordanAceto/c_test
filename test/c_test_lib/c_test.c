/*
--|----------------------------------------------------------------------------|
--| FILE DESCRIPTION:
--|   c_test.c provides the implementation for the simple c_test framework.
--|  
--|----------------------------------------------------------------------------|
--| NOTES:
--|   None.
--|  
--|----------------------------------------------------------------------------|
--| REFERENCES:
--|   None.
--|
--|----------------------------------------------------------------------------|
*/

/*
--|----------------------------------------------------------------------------|
--| INCLUDE FILES
--|----------------------------------------------------------------------------|
*/

#include "c_test.h"
#include <stdio.h>
#include <stdlib.h>

/*
--|----------------------------------------------------------------------------|
--| PRIVATE DEFINES
--|----------------------------------------------------------------------------|
*/

/*
--| NAME: MAX_COMPARISON_FEILD_LENGTH
--| DESCRIPTION: the maximum string length of a comparison description
--| TYPE: unsigned integer
*/
#define MAX_COMPARISON_FEILD_LENGTH 16u

/*
--|----------------------------------------------------------------------------|
--| PRIVATE TYPES
--|----------------------------------------------------------------------------|
*/

/*
--| NAME: c_test_comparison_t
--| DESCRIPTION: storage for a single test comparison
*/
typedef struct c_test_comparison_type
{
    char description[MAX_TEST_DESCRIPTION_LENGTH];
    char expected[MAX_COMPARISON_FEILD_LENGTH];
    char actual[MAX_COMPARISON_FEILD_LENGTH];
    bool result;
} c_test_comparison_t;

/*
--| NAME: c_test_case_t
--| DESCRIPTION: storage for a test case, which consists of many comparisons
*/
typedef struct c_test_case_type
{
    char description[MAX_TEST_DESCRIPTION_LENGTH];
    c_test_comparison_t comparisons[MAX_NUM_COMPARISONS_PER_TEST_CASE];
    uint32_t num_comparisons;
    uint32_t num_failing_comparisons;
} c_test_case_t;

/*
--|----------------------------------------------------------------------------|
--| PRIVATE VARIABLES
--|----------------------------------------------------------------------------|
*/

/*
--| NAME: c_test_case_t
--| DESCRIPTION: array of test cases, each contains an array of test comparisons
--| TYPE: c_test_case_t[]
*/
static c_test_case_t test_cases[MAX_NUM_TEST_CASES];

/*
--| NAME: Test_Case_Number
--| DESCRIPTION: the current test case number, each test case is made up of a 
--|   number of test comparisons
--| TYPE: uint32_t
*/
static uint32_t Test_Case_Number;

/*
--| NAME: Total_Num_Comparisons
--| DESCRIPTION: the total number of comparisons done so far
--| TYPE: uint32_t
*/
static uint32_t Total_Num_Comparisons;

/*
--| NAME: Total_Num_Passing_Comparisons
--| DESCRIPTION: the current number of comparisons that have passed
--| TYPE: uint32_t
*/
static uint32_t Total_Num_Passing_Comparisons;

/*
--| NAME: Total_Num_Failing_Comparisons
--| DESCRIPTION: the current number of comparisons that have failed
--| TYPE: uint32_t
*/
static uint32_t Total_Num_Failing_Comparisons;

/*
--| NAME: c_test_output_mode_enum_t
--| DESCRIPTION: the output mode, show all or only show failing
--| TYPE: c_test_output_mode_enum_t
*/
static c_test_output_mode_enum_t output_mode;

/*
--|----------------------------------------------------------------------------|
--| PRIVATE HELPER FUNCTION PROTOTYPES
--|----------------------------------------------------------------------------|
*/

/*------------------------------------------------------------------------------
Function Name:
    abs_fl32

Function Description:
    Return the absolute value of the given float. 

Parameters:
    x: the float to take the abs of.

Returns:
    float: the abs(x)

Assumptions/Limitations:
    None
------------------------------------------------------------------------------*/
static float abs_fl32(float x);

/*
--|----------------------------------------------------------------------------|
--| PUBLIC FUNCTION DEFINITIONS
--|----------------------------------------------------------------------------|
*/

void C_Test_Initialize(void)
{
    Test_Case_Number = 0u;
    Total_Num_Comparisons = 0u;
    Total_Num_Passing_Comparisons = 0u;
    Total_Num_Failing_Comparisons = 0u;
}



void C_Test_Configure_Output_Mode(c_test_output_mode_enum_t output_mode_enum)
{
    output_mode = output_mode_enum;
}



void C_Test_Start_Next_Test_Case(const char * description)
{
    Test_Case_Number++;

    test_cases[Test_Case_Number].num_comparisons = 0u;

    sprintf(test_cases[Test_Case_Number].description, "%s", description);
}



void C_Test_Add_Description_To_Next_Comparison(const char * description)
{
    sprintf(test_cases[Test_Case_Number].
            comparisons[test_cases[Test_Case_Number].num_comparisons + 1u].
            description, 
            "%s",
            description);
}



bool C_Test_Assert_Equals_ui32(uint32_t expected, uint32_t actual)
{
    Total_Num_Comparisons++;
    test_cases[Test_Case_Number].num_comparisons++;

    bool result = (expected == actual);

    if (result)
    {
        Total_Num_Passing_Comparisons++;
    }
    else
    {
        Total_Num_Failing_Comparisons++;
        test_cases[Test_Case_Number].num_failing_comparisons++;
    }
    
    c_test_comparison_t * p_current_comparison = 
        &(test_cases[Test_Case_Number].
        comparisons[test_cases[Test_Case_Number].num_comparisons]);

    snprintf(p_current_comparison->expected, 16, "0x%x", expected);
    snprintf(p_current_comparison->actual,   16, "0x%x", actual);
    p_current_comparison->result = result;
    
    return result;
}



bool C_Test_Assert_Equals_fl32(float expected, float actual, float epsilon)
{
    Total_Num_Comparisons++;
    test_cases[Test_Case_Number].num_comparisons++;

    float diff = expected - actual;

    bool result = abs_fl32(diff) < epsilon;

    if (result)
    {
        Total_Num_Passing_Comparisons++;
    }
    else
    {
        Total_Num_Failing_Comparisons++;
        test_cases[Test_Case_Number].num_failing_comparisons++;
    }
    
    c_test_comparison_t * p_current_comparison = 
        &(test_cases[Test_Case_Number].
        comparisons[test_cases[Test_Case_Number].num_comparisons]);

    snprintf(p_current_comparison->expected, 16, "%ff", expected);
    snprintf(p_current_comparison->actual,   16, "%ff", actual);
    p_current_comparison->result = result;
    
    return result;
}



void C_Test_Show_Test_Results_In_Terminal(void)
{
    for (uint8_t test_case = 1u; test_case <= Test_Case_Number; test_case++)
    {    
        // show all the results if configured to do so or if there is a failing comparison in this test case
        if (output_mode == C_TEST_OUTPUT_MODE_SHOW_ALL_RESULTS || 
            test_cases[test_case].num_failing_comparisons > 0u)
        {
            printf("\n\n--| Test Case %2d: %-32s --- expected --------- actual --------- result ---\n\n", 
                test_case, 
                test_cases[test_case].description);

            for (uint8_t comparison = 1u; comparison <= test_cases[test_case].num_comparisons; comparison++)
            {
                printf("#%02d.%02d: %-38s %16s %16s %16s\n", 
                    test_case, 
                    comparison, 
                    test_cases[test_case].comparisons[comparison].description,
                    test_cases[test_case].comparisons[comparison].expected,
                    test_cases[test_case].comparisons[comparison].actual,
                    test_cases[test_case].comparisons[comparison].result == true ? "PASS" : "FAIL");  
            }
        }

        printf("\n--| Summary of test case %2d: %45d/%2d passed %12s\n", 
            test_case, 
            test_cases[test_case].num_comparisons - test_cases[test_case].num_failing_comparisons,
            test_cases[test_case].num_comparisons,
            test_cases[test_case].num_failing_comparisons == 0u ? "PASS" : "FAIL");
    }

    printf("\n\n----- Final Test Results -----\n------------------------------\n");
    printf("---| Num test cases:  %u\n", Test_Case_Number);
    printf("---| Num comparisons: %u\n", Total_Num_Comparisons);
    printf("---|\n");
    printf("---| Passing tests: %u\n", Total_Num_Passing_Comparisons);
    printf("---| Failing tests: %u\n", Total_Num_Failing_Comparisons);
    printf("\n---| Test %s\n\n", Total_Num_Failing_Comparisons == 0u ? "PASSED" : "FAILED");
}

/*
--|----------------------------------------------------------------------------|
--| PRIVATE HELPER FUNCTION DEFINITIONS
--|----------------------------------------------------------------------------|
*/

static float abs_fl32(float x)
{
    return x > 0.0f ? x : x * -1.0f;
}
