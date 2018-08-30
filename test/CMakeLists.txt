configure_file(lit.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/lit.cfg)

add_custom_target(check
    COMMAND lit -v ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS weak-bindgen)

add_test(NAME lit-tests
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                                 --target check)
