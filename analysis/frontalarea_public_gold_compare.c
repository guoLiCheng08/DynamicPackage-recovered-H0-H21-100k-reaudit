#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

int main(void)
{
    double vector_data[3] = {1.0, -2.0, 3.0};
    double first_data[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    double second_data[9] = {2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0};
    DpVector vector = {3, 0, vector_data};
    DpMatrix first = {3, 3, 3, 0, first_data};
    DpMatrix second = {3, 3, 3, 0, second_data};
    double expected_vector_data[3];
    double expected_first_data[9];
    double expected_second_data[9];
    DpVector expected_vector;
    DpMatrix expected_first;
    DpMatrix expected_second;

    memcpy(expected_vector_data, vector_data, sizeof(vector_data));
    memcpy(expected_first_data, first_data, sizeof(first_data));
    memcpy(expected_second_data, second_data, sizeof(second_data));
    expected_vector = vector;
    expected_first = first;
    expected_second = second;
    if (FrontalArea(&vector, &first, &second, 9.25) != 0 ||
        memcmp(&vector, &expected_vector, sizeof(vector)) != 0 ||
        memcmp(&first, &expected_first, sizeof(first)) != 0 ||
        memcmp(&second, &expected_second, sizeof(second)) != 0 ||
        memcmp(vector_data, expected_vector_data, sizeof(vector_data)) != 0 ||
        memcmp(first_data, expected_first_data, sizeof(first_data)) != 0 ||
        memcmp(second_data, expected_second_data, sizeof(second_data)) != 0) {
        (void)puts("FrontalArea observable ABI mismatch");
        return 1;
    }
    (void)puts("PASS: FrontalArea controlled original-ELF gold compare");
    return 0;
}
