# ISA LDAP Server

A multi-client **LDAPv3 server** in **C++20** (ISA course project, FIT VUT Brno). It serves directory data from a CSV file and speaks the LDAP wire protocol end-to-end, with a hand-rolled ASN.1/BER codec.

## Features

- **LDAPv3 protocol**: `bind` (simple authentication), `search` requests and responses, `sizeLimit` handling.
- **Search filters**: equality match, substrings, and recursive boolean composition (`and`, `or`, `not`).
- **Attribute selection**: clients may request specific attributes; absent/unknown attributes are ignored, no selection returns all.
- **CSV datastore**: the directory is loaded from a CSV file (`-f`).
- **IPv4 and IPv6** support.

## Design

- **BER codec as a class hierarchy**: each ASN.1 type is its own `BerObject` subclass (`BerIntObject`, `BerStringObject`, `BerSequenceObject`, …) that encodes/decodes itself — see UML in `img/`.
- **Filters as objects**: `FilterObject` subclasses (`AndFilterObject`, `OrFilterObject`, `NotFilterObject`, `EqualityMatchFilterObject`, `SubstringFilterObject`) compose recursively, mirroring the LDAP filter grammar.
- **Notice of Disconnection** extended response when an error cannot be reported via a normal reply.
- Bind can happen at any point in the session (LDAP3 semantics), which makes adding other auth types easy.

## Build

```sh
make         # produces ./isa-ldapserver (C++20, g++)
```

## Run

```sh
./isa-ldapserver -f data.csv [-p 389]
```

`Ctrl+C` (SIGINT) stops the server. API reference: `docs/refman.pdf` (Doxygen), user manual: `manual.md`.
