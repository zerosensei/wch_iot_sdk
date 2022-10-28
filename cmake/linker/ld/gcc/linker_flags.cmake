# The coverage linker flag is specific for gcc.

# Using a config check is ok for now, but in future it would be desired if
# linker flags themselves are not depending on actual configurations.
# All flags should be described, and the caller should know the flag name to use.

check_set_linker_property(TARGET linker APPEND PROPERTY gprof -pg)
