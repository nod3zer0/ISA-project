---
title: ISA - LDAP server
author: René Češka xceska06 <xceska06@fit.vutbr.cz>
date: 20.11.2023
---

# Použití

./isa-ldapserver {-p \<port\>} -f \<soubor\>
Význam parametrů a jejich hodnot:

-p \<port\>: Umožňuje specifikovat konkrétní port, na kterém začne server naslouchat požadavkům klientů. Výchozí hodnota čísla portu je 389.
-f \<soubor\>: Cesta k textovému soubor ve formátu CSV.

Aplikaci lze ukončit pomocí klávesové zkratky Ctrl+C (SIGINT).

## Sestavení

Sestavení probíhá pomocí příkazu `make`. Výsledkem je spustitelný soubor `isa-ldapserver`. Veškeré zdrojové soubory jsou ve složce src, a hlavičkové soubory ve složce inc.

# Popis programu

Jedná se o jednoduchý LDAP server, pracující s csv databází. Podporuje simple bind(bind request), a vyhledávání (search request). Z filtrů podporuje pouze equality match, substrings, and, or, not. Podporuje také size limit.
Server podporuje jak IPv4, tak IPv6.

# Rozšíření oproti zadání

## Výběr atributů

Aplikace podporuje možnost vybrat si jaké atributy chce uživatel vyhledat. V případě, že uživatel nevybere žádný atribut, jsou mu vráceny všechny atributy. Neexistující atributy jsou ignorovány.

## Notice of Disconnection

Pokud dojde k chybě, která nejde oznámit přes odpovídající odpověď na požadavek, je klientovi oznámeno ukončení komunikace pomocí Notice of Disconnection.

## Přihlášení kdykoli

Není třeba se přihlásit před bind requestem, ale je možné se přihlásit kdykoli během komunikace (LDAP3). Jelikož server podporuje jen podporuje jen simple bind, bez autentizace tak to nemá význam. Ale je možné potom velice jednoduše rozšířit aplikaci pro další typy autentizace.

# Příklady spuštění

## Spuštění serveru

### 1. příklad

./isa-ldapserver -p 1234 -f ./data.csv

spustí server na portu 123 a použije databázi data.csv

### 2. příklad

./isa-ldapserver -f ./data.csv

spustí server na portu 389 a použije databázi data.csv

# Seznam souborů

- manual.md
- manual.pdf
- Doxyfile
- Makefile
- docs/
  - refman.pdf
- src/
  - AndFilterObject.cpp
  - BerEnumObject.cpp
  - BerObject.cpp
  - BerSetObject.cpp
  - DatabaseController.cpp
  - EqualityMatchFilterObject.cpp
  - isa-ldapserver.cpp
  - OrFilterObject.cpp
  - argument_helper_functions.cpp
  - ber_helper_functions.cpp
  - BerParser.cpp
  - BerStringObject.cpp
  - database_helper_functions.cpp
  - filter_helper_functions.cpp
  - ldap_comunication.cpp
  - server.cpp
  - BerBoolObject.cpp
  - BerIntObject.cpp
  - BerSequenceObject.cpp
  - BerUndefinedObject.cpp
  - DatabaseObject.cpp
  - FilterObject.cpp
  - NotFilterObject.cpp
  - SubstringFilterObject.cpp
- inc/
  - AndFilterObject.h
  - ber_constants.h
  - BerIntObject.h
  - BerSequenceObject.h
  - BerUndefinedObject.h
  - DatabaseObject.h
  - FilterObject.h
  - OrFilterObject.h
  - argument_helper_functions.h
  - BerEnumObject.h
  - BerObject.h
  - BerSetObject.h
  - DatabaseController.h
  - EqualityMatchFilterObject.h
  - ldap_comunication.h
  - server.h
  - BerBoolObject.h
  - ber_helper_functions.h
  - BerParser.h
  - BerStringObject.h
  - database_helper_functions.h
  - filter_helper_functions.h
  - NotFilterObject.h
  - SubstringFilterObject.h
- img/
  - BerObject-UML.png
  - FilterObject-UML.png
- obj/


