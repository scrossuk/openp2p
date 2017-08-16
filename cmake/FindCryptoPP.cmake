# - Find CryptoPP

if(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
   set(CRYPTOPP_FOUND TRUE)

else(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
  find_path(CryptoPP_INCLUDE_DIR cryptlib.h
      /usr/include/crypto++
      /usr/include/cryptopp
      /usr/local/include/crypto++
      /usr/local/include/cryptopp
      /opt/local/include/crypto++
      /opt/local/include/cryptopp
      $ENV{SystemDrive}/Crypto++/include
      )

  find_library(CryptoPP_LIBRARIES NAMES cryptopp
      PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      $ENV{SystemDrive}/Crypto++/lib
      )

  if(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
    set(CRYPTOPP_FOUND TRUE)
    message(STATUS "Found CryptoPP: ${CRYPTOPP_INCLUDE_DIR}, ${CRYPTOPP_LIBRARIES}")
  else(CRYPTOPP_INCLUDE_DIR AND CRYPTOPP_LIBRARIES)
    set(CRYPTOPP_FOUND FALSE)
    message(STATUS "CryptoPP not found.")
  endif(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)

  mark_as_advanced(CryptoPP_INCLUDE_DIR CryptoPP_LIBRARIES)

endif(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
