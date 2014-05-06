#include <iostream>

#include <p2p/Crypt.hpp>
#include <p2p/Transport.hpp>

using namespace p2p::Crypt;

int main() {
	std::cout << "Create private key" << std::endl;
	
	AutoSeededRandomPool rand;
	ECDSA::PrivateKey privateKey(rand, ECDSA::brainpoolP256r1);
	
	std::cout << "Create sign stream" << std::endl;
	ECDSA::SignStream signStream(rand, privateKey);
	p2p::BinaryOStream binSign(signStream);
	
	std::cout << "Signing..." << std::endl;
	
	p2p::Binary::WriteUint32(binSign, 42);
	
	p2p::Buffer signature = signStream.signature();
	
	std::cout << "Signed: size = " << signature.size() << " bytes" << std::endl;
	
	std::cout << "Make public key from private key" << std::endl;
	ECDSA::PublicKey publicKey(rand, privateKey);
	
	std::cout << "Create verify stream" << std::endl;
	ECDSA::VerifyStream verifyStream(publicKey, signature);
	p2p::BinaryOStream binVerify(verifyStream);
	
	
	std::cout << "Verifying..." << std::endl;
	
	p2p::Binary::WriteUint32(binVerify, 42);
	
	std::cout << "Signature is " << (verifyStream.isSignatureValid() ? "valid" : "not valid") << std::endl;
	
	return 0;
}

