find_package(LATEX)
include(UseLATEX.cmake)
add_latex_document("OpenP2P.tex" DEFAULT_PDF MANGLE_TARGET_NAMES)
add_latex_document("Root Network.tex" DEFAULT_PDF MANGLE_TARGET_NAMES)
