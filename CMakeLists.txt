find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zmk_behavior_time)

zephyr_library()

zephyr_library_sources(
    src/behaviors/behavior_time_digit.c
    src/behaviors/behavior_time_clear.c
    src/behaviors/behavior_time_commit.c
)

zephyr_include_directories(include)
