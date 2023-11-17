

## Teorie

## Použití

./isa-ldapserver {-p \<port\>} -f \<soubor\>
Význam parametrů a jejich hodnot:

-p \<port\>: Umožňuje specifikovat konkrétní port, na kterém začne server naslouchat požadavkům klientů. Výchozí hodnota čísla portu je 389.
-f \<soubor\>: Cesta k textovému soubor ve formátu CSV.

### Sestavení

Sestavení probíhá pomocí příkazu `make`. Výsledkem je spustitelný soubor `isa-ldapserver`.

## Popis funkce aplikace

Hlavní smyčka aplikace je v souboru server.cpp, ve kterém se prvně nastaví poslouchání na uživatelem zadaném portu. Následně aplikace čeká dokud nepříjde požadavek od ldap klienta. Následně dojde k forku, a v dceřiném procesu se zpracuje požadavek klienta.

Na požadavek podprocese reaguje dle jeho typu. Podporuje 3 druhy požadavků od klienta. Tyto požadavky jsou:

- BIND_REQUEST
- SEARCH_REQUEST
- UNBIND_REQUEST

### BIND_REQUEST

Bind request může přijít kdykoli a nemusí být prvním požadavkem. Zde se ověřuje jestli client žádá o správný typ přihlášení. Aplikace podporuje pouze simple, pokud klient zažádá o jinou, je mu vrácena chyba, a komunikace ukončena. Pokud je vše v pořádku, je klientovi vrácen úspěšný BIND_RESPONSE.

### SEARCH_REQUEST

Search request může přijít kdykoli a není třeba aby navazoval na bindrequest. Podproces následně zpracuje tento požadavek, vyhledá v databázi odpovídající záznamy a pomocí odpovědi SearchResultEntry je vrátí klientovi.
Nakonec pošle SearchResultDone, kterým oznámý klientovi, že je vyhledávání dokončeno.

### UNBIND_REQUEST

Jakmile příjde tento požadavek, podproces ukončí komunikaci s klientem a ukončí se.

## Rozšíření oproti zadání

### Výběr atributů

Aplikace podporuje možnost vybrat si jaké atributy chce uživatel vyhledat. V případě, že uživatel nevybere žádný atribut, jsou mu vráceny všechny atributy. Neexistující attributy jsou ignorovány.

### Notice of Disconnection

Pokud dojde k chybě, která nejde oznámit přes odpovídající odpověd na požadavek, je klientovi oznámeno ukončení komunikace pomocí Notice of Disconnection.


## Zdrojový kód

Program byl psát v jazyku CPP s objektovým přístupem. Zdrojový kód byl dokumentován pomocí Doxygenu. Výsledná dokumentace je k dispozici v adresáři docs v souboru refman.pdf.

Zde je pouze stručný popis zajímavých tříd, jejich významu a použití.

### Třídy

#### BerObject

Je bázovým objektem pro objekty reprezentující BER struktury. Obsahuje základní metody pro ostatní objekty.

Pomocí funkce `getBerRepresentation`  lze získat reprezentaci objektu v podobě BER bytového pole. Které lze snadno odeslat clientovy.
Pro deserializaci je třeba využít funkce `ParseBerObject` která vrací ukazatel na nově vytvořený objekt.

Z těchto objektů lze snadno vytvářet BER struktury a pracovat s nimy.

![BerObject](docs/BerObject-UML.png)

#### FilterObject

Je bázovým objektem pro objekty reprezentující LDAP filtry. Jeho podtřídy jsou obsahují metody pro snadnou práci s nimy.

![FilterObject](docs/FilterObject-UML.png)

#### DatabaseController

Třída DatabaseController slouží pro práci s csv databází. Obsahuje metody pro načítání řádků z databáze a vrací je v podobě objektů třídy DatabaseObject.

#### DatabaseObject

Třída DatabaseObject slouží pro reprezentaci řádku v databázi. Obsahuje metody pro získání hodnot atributů.

## Testování

### Testovací prostředí

- client
  - program ldapsearch
  - OS Manjaro Linux 23.0.4
- server
  - program isa-ldapserver
  - OS CentOS Linux 7
- databáze uživatelů
  - soubor [ldap-lidi-ascii.csv](http://nes.fit.vutbr.cz/ivesely/ldap-lidi-ascii.csv)
- shell
  - zsh 5.9

### Testovací scénáře

#### Vyhledávání

##### Vyhledávání všech uživatelů

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=x*)"`

- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal všechny uživatele, jejichž uid začíná na x(kompletní soubor).

##### Vyhledávání uživatele pokročilejší filty

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(&(|(uid=xves*)(cn=*imir))(email=*stud*))"`

- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal všechny uživatele, jejichž uid začíná na xves, nebo cn obsahuje imir a email obsahuje stud.

##### forkování procesů

###### 10 klientů

- příkaz pro jednoho client `nc merlin.fit.vutbr.cz 10013`

Výsledek prošel dle očekávání a clienti se mohli připojovat a odpojovat kdykoliv chtěli.

###### Ukončení serveru při připojení několika clientů

- příkaz pro jednoho client `nc merlin.fit.vutbr.cz 10013`

Výsledek prošel dle očekávání a server se ukončil a všechny své podprocesy zabil. Také klientům, kteří byli ještě připojeni odeslal Notice of Disconnection s chybovým kódem unavailable.

##### IPV6 a IPV4

Toto testování proběhlo pouze na lokálním stroji(Manjaro), jelikož nemám IPV6 připojení k dispozici.

###### IPV6

- příkaz `ldapsearch -H ldap://\[::1\]:10013 -x "(uid=xvesel38)"`
- Wireshark: nehlásil chybu v komunikaci
Výsledek prošel dle očekávání a program vypsal uživatele s uid xvesel38.

###### IPV4

- příkaz `ldapsearch -H ldap://localhost:10013 -x "(uid=xvesel38)"`
- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal uživatele s uid xvesel38.


##### Různé parametry

###### špatné argumenty při spuštění serveru

- příkaz `./isa-ldapserver -p 10000000 -f ldap-lidi-ascii.csv`
- příkaz `./isa-ldapserver -p 0 -f ldap-lidi-ascii.csv`
- příkaz `./isa-ldapserver -p -f ldap-lidi-ascii.csv`
- příkaz `./isa-ldapserver -f neexistujici_soubor`

Při všech těchto příkazech došlo k ukončení serveru a vypsání chybového výstupu.

###### ldapsearch bez -x (bez simple bind)

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 "(uid=xvesel38)"`
- Wireshark: nehlásil chybu v komunikaci

Výsledek dle očekávání vrátil `ldap_sasl_interactive_bind: No such object (32)` jelikož tato implementace ldap serveru nepodporuje presence filtry, takže ldapsearch nenašel žádný záznam o použitých technologiích k přihlášení a ukončil komunikaci, bez pokusu o přihlášení.

###### size limit

####### size limit 0

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=x*)" -z 0`
- Wireshark: nehlásil chybu v komunikaci

Výsledek dle očekávání vrátil všechny uživatele, jelikož size limit byl nastaven na 0, a tedy deaktivován.

####### size limit 1

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=x*)" -z 1`
- Wireshark: nehlásil chybu v komunikaci

Výsledek dle očekávání vrátil pouze jednoho uživatele, jelikož size limit byl nastaven na 1.

####### size limit 850

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=x*)" -z 850`
- Wireshark: nehlásil chybu v komunikaci

Výsledek dle očekávání vrátil 850 uživatelů, jelikož size limit byl nastaven na 850.

####### size limit 200000

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=x*)" -z 200000`
- Wireshark: nehlásil chybu v komunikaci

Výsledek dle očekávání vrátil všechny uživatele, jelikož size limit byl nastaven na 200000, což je více než je počet uživatelů v databázi.

##### Jednotlivé filtry

###### Equality Match

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=xvesel38)"`
- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(cn=Vesely Vladimir)"`
- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(email=xvesel38@stud.fit.vutbr.cz)`

- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal u všech příkazů následující výstup:

```
# extended LDIF
#
# LDAPv3
# base <> (default) with scope subtree
# filter: (uid=xvesel38)
# requesting: ALL
#

#
dn: xvesel38
cn: Vesely Vladimir
email: xvesel38@stud.fit.vutbr.cz
uid: xvesel38

# search result
search: 2
result: 0 Success

# numResponses: 2
# numEntries: 1

```

###### Substrings

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uid=*ve*el*)"`

- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal všechny uživatele, jejichž uid obsahuje `ve` a `el`.

###### AND

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(&(uid=xve*)(uid=*sel38))"`

- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal všechny uživatele s uid xvesel38.

```
# extended LDIF
#
# LDAPv3
# base <> (default) with scope subtree
# filter: (&(uid=xve*)(uid=*sel38))
# requesting: ALL
#

#
dn: xvesel38
cn: Vesely Vladimir
email: xvesel38@stud.fit.vutbr.cz
uid: xvesel38

# search result
search: 2
result: 0 Success

# numResponses: 2
# numEntries: 1

```

###### OR

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(|(uid=xvesel38)(uid=xvesel40)(uid=xvesel39))" `
- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a program vypsal 3 uživatele s uid xvesel38, xvesel40, xvesel39.

```
# extended LDIF
#
# LDAPv3
# base <> (default) with scope subtree
# filter: (|(uid=xvesel38)(uid=xvesel40)(uid=xvesel39))
# requesting: ALL
#

#
dn: xvesel40
cn: Vesely Ales
email: xvesel40@stud.fit.vutbr.cz
uid: xvesel40

#
dn: xvesel39
cn: Vesely Karel
email: xvesel39@stud.fit.vutbr.cz
uid: xvesel39

#
dn: xvesel38
cn: Vesely Vladimir
email: xvesel38@stud.fit.vutbr.cz
uid: xvesel38

# search result
search: 2
result: 0 Success

# numResponses: 4
# numEntries: 3
```

###### NOT

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(|(!(uid=x*))(uid=xvesel38))"`
- Wireshark: nehlásil chybu v komunikaci

Výsledek prošel dle očekávání a vypsal pouze jednoho uživatele s uid xvesel38.

```
# extended LDIF
#
# LDAPv3
# base <> (default) with scope subtree
# filter: (|(!(uid=x*))(uid=xvesel38))
# requesting: ALL
#

#
dn: xvesel38
cn: Vesely Vladimir
email: xvesel38@stud.fit.vutbr.cz
uid: xvesel38

# search result
search: 2
result: 0 Success

# numResponses: 2
# numEntries: 1

```

##### speciální případy

###### Vyhledávání podle neexistujícího atributu

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(uidd=xvesel38)"`
- WireShark: nehlásil chybu v komunikaci

Výsledek dle očekávání neobsahoval žádné záznamy.

###### Negace vyhledávání podle neexistujícího atributu

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(!(uidd=xvesel38))"`
- WireShark: nehlásil chybu v komunikaci

Výsledek dle očekávání neobsahoval žádné záznamy.

###### Dlouhý filtr

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(|(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39)(uid=xvesel38)(uid=xvesel40)(uid=xvesel39))"`

- WireShark: nehlásil chybu v komunikaci

Výsledek dle očekávání vypsal uživatele s uid xvesel38, xvesel40, xvesel39.

###### Dlouhý Equality Match

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(!(uid=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa))"`
- WireShark: nehlásil chybu v komunikaci

Výsledek dle očekávání obsahoval všechny uživatele.

###### Dlouhý Substrings

- příkaz `ldapsearch -H ldap://merlin.fit.vutbr.cz:10013 -x "(!(uid=*aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa*aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa*aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa*aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa*))"`
- WireShark: nehlásil chybu v komunikaci

Výsledek dle očekávání obsahoval všechny uživatele.

