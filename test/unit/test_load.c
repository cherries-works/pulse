#include <criterion/criterion.h>

#include "parse.h"

Test(load, parses_load_averages) {
    char buffer[] = "0.42 1.37 2.85 3/421 12345\n";
    Load load = getLoad(sizeof(buffer), buffer);

    cr_assert_float_eq(load.load1, 0.42f, 0.001f);
    cr_assert_float_eq(load.load5, 1.37f, 0.001f);
    cr_assert_float_eq(load.load15, 2.85f, 0.001f);
}

Test(load, handles_zero_values) {
    char buffer[] = "0.00 0.00 0.00 1/100 1000\n";
    Load load = getLoad(sizeof(buffer), buffer);

    cr_assert_float_eq(load.load1, 0.0f, 0.001f);
    cr_assert_float_eq(load.load5, 0.0f, 0.001f);
    cr_assert_float_eq(load.load15, 0.0f, 0.001f);
}

Test(load, handles_incomplete_input) {
    char buffer[] = "0.42 1.37\n";
    Load load = getLoad(sizeof(buffer), buffer);

    cr_assert_float_eq(load.load1, 0.42f, 0.001f);
    cr_assert_float_eq(load.load5, 0.0f, 0.001f);
    cr_assert_float_eq(load.load15, 0.0f, 0.001f);
}

Test(load, handles_integer_values) {
    char buffer[] = "1 2 3 4/100 500\n";
    Load load = getLoad(sizeof(buffer), buffer);

    cr_assert_float_eq(load.load1, 1.0f, 0.001f);
    cr_assert_float_eq(load.load5, 2.0f, 0.001f);
    cr_assert_float_eq(load.load15, 3.0f, 0.001f);
}
