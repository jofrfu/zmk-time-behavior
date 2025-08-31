target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_TIME app PRIVATE src/behaviors/behavior_time_digit.c)
target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_TIME app PRIVATE src/behaviors/behavior_time_clear.c)
target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_TIME app PRIVATE src/behaviors/behavior_time_commit.c)

zephyr_include_directories(include)

target_link_libraries(app PRIVATE
    zephyr_subsys__bluetooth__controller
    zephyr_drivers__counter
)