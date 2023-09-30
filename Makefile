build:	isa-ldapserver.cpp
	c++ --std=c++20 -Wall -g isa-ldapserver.cpp -o isa-ldapserver
	c++ --std=c++20 -Wall -g buildMessage.cpp -o buildMessage

clean:
	rm isa-ldapserver

