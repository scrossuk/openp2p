add_library(openp2p-crypt STATIC
	AutoSeededRandomPool.cpp
	ECDSA/PrivateKey.cpp
	ECDSA/PublicKey.cpp
	ECDSA/SignStream.cpp
	ECDSA/VerifyStream.cpp
	Hash/MD5Stream.cpp
	Hash/SHA256Stream.cpp
	)
