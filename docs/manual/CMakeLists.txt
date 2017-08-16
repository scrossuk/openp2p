
# 
# Find LaTeX
# 

find_package(LATEX)
if(NOT LATEX_FOUND)
    message(STATUS "Disabled generation of documentation (missing LaTeX).")
    return()
endif()


# 
# Target name
# 

set(target docs-manual)
message(STATUS "Doc ${target}")


# 
# Input and output files
# 

set(openp2p_source "${CMAKE_CURRENT_SOURCE_DIR}/OpenP2P.tex")
set(openp2p_pdf    "${CMAKE_CURRENT_BINARY_DIR}/OpenP2P.pdf")
set(rootnetwork_source "${CMAKE_CURRENT_SOURCE_DIR}/Root_Network.tex")
set(rootnetwork_pdf    "${CMAKE_CURRENT_BINARY_DIR}/Root_Network.pdf")


# 
# Create documentation
# 

# Invoke LaTeX
add_custom_command(
    OUTPUT            ${openp2p_pdf}
    DEPENDS           ${openp2p_source}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND           ${PDFLATEX_COMPILER} \"${openp2p_source}\"
    COMMAND           ${PDFLATEX_COMPILER} \"${openp2p_source}\"
    COMMAND           ${PDFLATEX_COMPILER} \"${openp2p_source}\"
    COMMENT           "Creating LaTeX documentation."
)
add_custom_command(
    OUTPUT            ${rootnetwork_pdf}
    DEPENDS           ${rootnetwork_source}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND           ${PDFLATEX_COMPILER} \"${rootnetwork_source}\"
    COMMAND           ${PDFLATEX_COMPILER} \"${rootnetwork_source}\"
    COMMAND           ${PDFLATEX_COMPILER} \"${rootnetwork_source}\"
    COMMENT           "Creating LaTeX documentation."
)

# Declare target
add_custom_target(${target} ALL DEPENDS ${openp2p_pdf} ${rootnetwork_pdf})
add_dependencies(docs ${target})


# 
# Deployment
# 

# PDF file
install(FILES ${openp2p_pdf} ${rootnetwork_pdf}
    DESTINATION "${INSTALL_DOC}"
    COMPONENT docs
)
